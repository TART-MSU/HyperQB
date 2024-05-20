// -*- coding: utf-8 -*-
// Copyright (C) by the Spot authors, see the AUTHORS file for details.
//
// This file is part of Spot, a model checking library.
//
// Spot is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// Spot is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "config.h"
#include <queue>
#include <map>
#include <utility>
#include <numeric>
#include <spot/twaalgos/simulation.hh>
#include <spot/misc/minato.hh>
#include <spot/twa/bddprint.hh>
#include <spot/twaalgos/sccfilter.hh>
#include <spot/twaalgos/sepsets.hh>
#include <spot/twaalgos/isdet.hh>
#include <spot/misc/bddlt.hh>
#include <spot/twaalgos/cleanacc.hh>
#include <spot/twaalgos/split.hh>
#include <spot/priv/robin_hood.hh>

// Work around GCC bug 80947 (dominates_edge is causing spurious
// visibility warnings)
#if __GNUC__ <= 7
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#endif


//  Simulation-based reduction, implemented using bdd-based signatures.
//
//  The signature of a state is a Boolean function (implemented as a
//  BDD) representing the set of outgoing transitions of that state.
//  Two states with the same signature are equivalent and can be
//  merged.
//
// We define signatures in a way that implications between signatures
//  entail language inclusion.  These inclusions are used to remove
//  redundant transitions, but this occurs implicitly while
//  transforming the signature into irredundant-sum-of-product before
//  building the automaton: redundant terms that are left over
//  correspond to ignored transitions.
//
//  See our Spin'13 paper for background on this procedure.

namespace spot
{
  namespace
  {
    // Used to get the signature of the state.
    typedef std::vector<bdd> vector_state_bdd;

    // Get the list of state for each class.
    typedef std::map<bdd, std::list<unsigned>,
                     bdd_less_than> map_bdd_lstate;

    // This class helps to compare two automata in term of
    // size.
    struct automaton_size final
    {
      automaton_size()
        : edges(0),
          states(0)
      {
      }

      automaton_size(const const_twa_graph_ptr& a)
        : edges(a->num_edges()),
          states(a->num_states())
      {
      }

      void set_size(const const_twa_graph_ptr& a)
      {
        states = a->num_states();
        edges = a->num_edges();
      }

      inline bool operator!=(const automaton_size& r)
      {
        return edges != r.edges || states != r.states;
      }

      inline bool operator<(const automaton_size& r)
      {
        if (states < r.states)
          return true;
        if (states > r.states)
          return false;
        return edges < r.edges;
      }

      inline bool operator>(const automaton_size& r)
      {
        if (states < r.states)
          return false;
        if (states > r.states)
          return true;

        if (edges < r.edges)
          return false;
        if (edges > r.edges)
          return true;

        return false;
      }

      int edges;
      int states;
    };

    // The direct_simulation. If Cosimulation is true, we are doing a
    // cosimulation.
    template <bool Cosimulation, bool Sba>
    class direct_simulation final
    {
    protected:
      typedef std::map<bdd, bdd, bdd_less_than> map_bdd_bdd;
      int acc_vars;
      acc_cond::mark_t all_inf_;
    public:

      bdd mark_to_bdd(acc_cond::mark_t m)
      {
        // FIXME: Use a cache.
        bdd res = bddtrue;
        for (auto n: m.sets())
          res &= bdd_ithvar(acc_vars + n);
        return res;
      }

      acc_cond::mark_t bdd_to_mark(bdd b)
      {
        // FIXME: Use a cache.
        std::vector<unsigned> res;
        while (b != bddtrue)
          {
            res.emplace_back(bdd_var(b) - acc_vars);
            b = bdd_high(b);
          }
        return acc_cond::mark_t(res.begin(), res.end());
      }

      direct_simulation(const const_twa_graph_ptr& in,
                        int trans_pruning,
                        std::vector<bdd>* implications = nullptr)
        : po_size_(0),
          all_class_var_(bddtrue),
          original_(in),
          trans_pruning_(trans_pruning),
          record_implications_(implications)
      {
        if (!has_separate_sets(in))
          throw std::runtime_error
            ("direct_simulation() requires separate Inf and Fin sets");
        if (!in->is_existential())
          throw std::runtime_error
            ("direct_simulation() does not yet support alternation");

        unsigned ns = in->num_states();
        size_a_ = ns;
        unsigned init_state_number = in->get_init_state_number();

        auto all_inf = in->get_acceptance().used_inf_fin_sets().first;
        all_inf_ = all_inf;

        // Replace all the acceptance conditions by their complements.
        // (In the case of Cosimulation, we also flip the edges.)
        if (Cosimulation)
          {
            a_ = make_twa_graph(in->get_dict());
            a_->copy_ap_of(in);
            a_->copy_acceptance_of(in);
            a_->new_states(ns);

            for (unsigned s = 0; s < ns; ++s)
              {
                for (auto& t: in->out(s))
                  {
                    acc_cond::mark_t acc;
                    if (Sba)
                      {
                        // If the acceptance is interpreted as
                        // state-based, to apply the reverse simulation
                        // on a SBA, we should pull the acceptance of
                        // the destination state on its incoming arcs
                        // (which now become outgoing arcs after
                        // transposition).
                        acc = {};
                        for (auto& td: in->out(t.dst))
                          {
                            acc = td.acc ^ all_inf;
                            break;
                          }
                      }
                    else
                      {
                        acc = t.acc ^ all_inf;
                      }
                    a_->new_edge(t.dst, s, t.cond, acc);
                  }
                a_->set_init_state(init_state_number);
              }
          }
        else
          {
            a_ = make_twa_graph(in, twa::prop_set::all());

            // When we reduce automata with state-based acceptance to
            // obtain transition-based acceptance, it helps to pull
            // the state-based acceptance on the incoming edges
            // instead of the outgoing edges.  A typical example
            //
            // ltl2tgba -B GFa | autfilt --small
            //
            // The two-state Büchi automaton generated for GFa will
            // not be reduced to one state if we push the acceptance
            // marks to the outgoing edges.  However it will be
            // reduced to one state if we pull the acceptance to the
            // incoming edges.
            if (!Sba && !in->prop_state_acc().is_false()
                && !record_implications_)
              {
                // common_out[i] is the set of acceptance set numbers common to
                // all outgoing edges of state i.
                std::vector<acc_cond::mark_t> common_out(ns);
                for (unsigned s = 0; s < ns; ++s)
                  {
                    bool first = true;
                    for (auto& e : a_->out(s))
                      if (first)
                        {
                          common_out[s] = e.acc;
                          first = false;
                        }
                      else if (common_out[s] != e.acc)
                        {
                          // If the automaton does not have
                          // state-based acceptance, do not change
                          // pull the marks.  Mark the input as
                          // "not-state-acc" so that we remember
                          // that.
                          std::const_pointer_cast<twa_graph>(in)
                            ->prop_state_acc(false);
                          goto donotpull;
                        }
                  }
                // Pull the common outgoing sets to the incoming
                // edges.  Doing so seems to favor cases where states
                // can be merged.
                for (auto& e : a_->edges())
                  e.acc = ((e.acc - common_out[e.src]) | common_out[e.dst])
                    ^ all_inf;
              }
            else
              {
              donotpull:
                for (auto& t: a_->edges())
                  t.acc ^= all_inf;
              }
          }
        assert(a_->num_states() == size_a_);

        want_implications_ = !is_deterministic(a_);

        // Now, we have to get the bdd which will represent the
        // class. We register one bdd by state, because in the worst
        // case, |Class| == |State|.
        unsigned set_num = a_->get_dict()
          ->register_anonymous_variables(size_a_ + 1, this);

        unsigned n_acc = a_->num_sets();
        acc_vars = a_->get_dict()
          ->register_anonymous_variables(n_acc, this);

        all_proms_ = bddtrue;
        for (unsigned v = acc_vars; v < acc_vars + n_acc; ++v)
          all_proms_ &= bdd_ithvar(v);

        bdd_initial = bdd_ithvar(set_num++);
        bdd init = bdd_ithvar(set_num++);

        used_var_.emplace_back(init);

        // Initialize all classes to init.
        previous_class_.resize(size_a_);
        for (unsigned s = 0; s < size_a_; ++s)
          previous_class_[s] = init;

        // Put all the anonymous variable in a queue, and record all
        // of these in a variable all_class_var_ which will be used
        // to understand the destination part in the signature when
        // building the resulting automaton.
        all_class_var_ = init;
        for (unsigned i = set_num; i < set_num + size_a_ - 1; ++i)
          {
            free_var_.push(i);
            all_class_var_ &= bdd_ithvar(i);
          }

        relation_[init] = init;
      }


      // Reverse all the acceptance condition at the destruction of
      // this object, because it occurs after the return of the
      // function simulation.
      virtual ~direct_simulation()
      {
        a_->get_dict()->unregister_all_my_variables(this);
      }

      // Update the name of the classes.
      void update_previous_class()
      {
        std::list<bdd>::iterator it_bdd = used_var_.begin();

        // We run through the map bdd/list<state>, and we update
        // the previous_class_ with the new data.
        for (auto& p: sorted_classes_)
          {
            // If the signature of a state is bddfalse (no
            // edges) the class of this state is bddfalse
            // instead of an anonymous variable. It allows
            // simplifications in the signature by removing a
            // edge which has as a destination a state with
            // no outgoing edge.
            if (p->first == bddfalse)
              for (unsigned s: p->second)
                previous_class_[s] = bddfalse;
            else
              for (unsigned s: p->second)
                previous_class_[s] = *it_bdd;
            ++it_bdd;
          }
      }

      void main_loop()
      {
        unsigned int nb_partition_before = 0;
        unsigned int nb_po_before = po_size_ - 1;
        while (nb_partition_before != bdd_lstate_.size()
               || nb_po_before != po_size_)
          {
            update_previous_class();
            nb_partition_before = bdd_lstate_.size();
            nb_po_before = po_size_;
            po_size_ = 0;
            update_sig();
            go_to_next_it();
            // print_partition();
          }

        update_previous_class();
      }

      // The core loop of the algorithm.
      twa_graph_ptr run()
      {
        main_loop();
        return build_result();
      }

      // Take a state and compute its signature.
      bdd compute_sig(unsigned src)
      {
        bdd res = bddfalse;

        for (auto& t: a_->out(src))
          {
            bdd acc = mark_to_bdd(t.acc);

            // to_add is a conjunction of the acceptance condition,
            // the label of the edge and the class of the
            // destination and all the class it implies.
            bdd to_add = acc & t.cond & relation_[previous_class_[t.dst]];

            res |= to_add;
          }

        // When we Cosimulate, we add a special flag to differentiate
        // the initial state from the other.
        if (Cosimulation && src == a_->get_init_state_number())
          res |= bdd_initial;

        return res;
      }


      void update_sig()
      {
        bdd_lstate_.clear();
        sorted_classes_.clear();
        for (unsigned s = 0; s < size_a_; ++s)
          {
            bdd sig = compute_sig(s);
            auto p = bdd_lstate_.emplace(std::piecewise_construct,
                                         std::make_tuple(sig),
                                         std::make_tuple());
            p.first->second.emplace_back(s);
            if (p.second)
              sorted_classes_.emplace_back(p.first);
          }
      }


      // This method renames the color set, updates the partial order.
      void go_to_next_it()
      {
        int nb_new_color = bdd_lstate_.size() - used_var_.size();


        // If we have created more partitions, we need to use more
        // variables.
        for (int i = 0; i < nb_new_color; ++i)
          {
            assert(!free_var_.empty());
            used_var_.emplace_back(bdd_ithvar(free_var_.front()));
            free_var_.pop();
          }


        // If we have reduced the number of partition, we 'free' them
        // in the free_var_ list.
        for (int i = 0; i > nb_new_color; --i)
          {
            assert(!used_var_.empty());
            free_var_.push(bdd_var(used_var_.front()));
            used_var_.pop_front();
          }


        assert((bdd_lstate_.size() == used_var_.size())
               || (bdd_lstate_.find(bddfalse) != bdd_lstate_.end()
                   && bdd_lstate_.size() == used_var_.size() + 1));

        // This vector links the tuple "C^(i-1), N^(i-1)" to the
        // new class coloring for the next iteration.
        std::vector<std::pair<bdd, bdd>> now_to_next;
        unsigned sz = bdd_lstate_.size();
        now_to_next.reserve(sz);

        std::list<bdd>::iterator it_bdd = used_var_.begin();

        for (auto& p: sorted_classes_)
          {
            // If the signature of a state is bddfalse (no edges) the
            // class of this state is bddfalse instead of an anonymous
            // variable. It allows simplifications in the signature by
            // removing an edge which has as a destination a state
            // with no outgoing edge.
            bdd acc = bddfalse;
            if (p->first != bddfalse)
              acc = *it_bdd;
            now_to_next.emplace_back(p->first, acc);
            ++it_bdd;
          }

        // Update the partial order.
        //
        // Do not compute implication between classes if we have more
        // classes than trans_pruning_, or if the automaton is
        // deterministic (in which case want_implications_ was
        // initialized to false).  The number of classes should only
        // augment, so if we exceed trans_pruning_, it's safe to
        // disable want_implications_ for good.

        if (want_implications_
            && trans_pruning_ >= 0
            && static_cast<unsigned>(trans_pruning_) < sz)
          want_implications_ = false;

        for (unsigned n = 0; n < sz; ++n)
          {
            bdd n_sig = now_to_next[n].first;
            bdd n_class = now_to_next[n].second;
            if (want_implications_)
              for (unsigned m = 0; m < sz; ++m)
                {
                  if (n == m)
                    continue;
                  if (bdd_implies(n_sig, now_to_next[m].first))
                    {
                      n_class &= now_to_next[m].second;
                      ++po_size_;
                    }
                }
            relation_[now_to_next[n].second] = n_class;
          }
      }

      // Build the simplified automaton.
      twa_graph_ptr build_result()
      {
        twa_graph_ptr res = make_twa_graph(a_->get_dict());
        res->copy_ap_of(a_);
        res->copy_acceptance_of(a_);

        // We have two ways of "spliting" a signature to create the
        // outgoing edges.  One is to iterate over 2^AP, then collect
        // the destinations.  The second is to first create a coarser
        // basis for the original set of labels, and then iterate on
        // this basis.  The latter is good when we have few distinct
        // labels.  With too many different labels that may have
        // nonempty intersections, the basis approach can consume a
        // lot of memory.  We have to heuristically select between
        // those two.
        unsigned nap = res->ap().size();
        bool will_use_basis = nap > 5;
        edge_separator es;
        if (will_use_basis)
          // Gather all labels, but stop if we see too many.  The
          // threshold below is arbitrary: adjust if you know better.
          will_use_basis = es.add_to_basis(a_, 256 * nap);
        // We use a cache to avoid the costly loop over the basis.
        //
        // Cache entries have the form (bdd, [begin, end]) where bdd
        // what should be split using the basis, and begin/end denotes
        // a range of existing transition numbers that cover the
        // split.
        //
        // std::pair causes some noexcept warnings when used in
        // robin_hood::unordered_map with GCC 9.4.  Use robin_hood::pair
        // instead.
        typedef robin_hood::pair<unsigned, unsigned> cached_t;
        robin_hood::unordered_map<bdd, cached_t, bdd_hash> split_cond;

        auto state_mapping = new std::vector<unsigned>();
        state_mapping->resize(a_->num_states());
        res->set_named_prop("simulated-states", state_mapping);

        // Non atomic propositions variables (= acc and class)
        bdd nonapvars = all_proms_ & bdd_support(all_class_var_);

        auto* gb = res->create_namer<int>();

        if (record_implications_)
          record_implications_->resize(bdd_lstate_.size());
        // Create one state per class.
        for (auto& p: sorted_classes_)
          {
            bdd cl = previous_class_[p->second.front()];
            // A state may be referred to either by
            // its class, or by all the implied classes.
            auto s = gb->new_state(cl.id());
            gb->alias_state(s, relation_[cl].id());
            // update state_mapping
            for (auto& st : p->second)
              (*state_mapping)[st] = s;
            if (record_implications_)
              (*record_implications_)[s] = relation_[cl];
          }

        std::vector<bdd> signatures;
        signatures.reserve(sorted_classes_.size());

        // Acceptance of states.  Only used if Sba && Cosimulation.
        std::vector<acc_cond::mark_t> accst;
        if (Sba && Cosimulation)
          accst.resize(res->num_states(), acc_cond::mark_t({}));

        stat.states = bdd_lstate_.size();
        stat.edges = 0;

        unsigned nb_minterms = 0;
        unsigned nb_minato = 0;

        auto all_inf = all_inf_;
        unsigned srcst = 0;

        auto create_edges = [&](int srcid, bdd one, bdd dest) {
          if (dest == bddfalse)
            return;
          // Iterate over all possible destination classes.  We
          // use minato_isop here, because if the same valuation
          // of atomic properties can go to two different
          // classes C1 and C2, iterating on C1 + C2 with other
          // means would see C1 then (!C1)C2, instead of C1 then
          // C2.  With minatop_isop, we ensure that no negative
          // class variable will be seen (likewise for
          // promises).
          minato_isop isop(dest);

          ++nb_minterms;
          bdd cond_acc_dest;
          while ((cond_acc_dest = isop.next()) != bddfalse)
            {
              ++stat.edges;
              ++nb_minato;

              // Take the edge, and keep only the variable which
              // are used to represent the class.
              bdd dst = bdd_existcomp(cond_acc_dest, all_class_var_);

              // Keep only ones who are acceptance condition.
              auto acc = bdd_to_mark(bdd_existcomp(cond_acc_dest,
                                                   all_proms_));

              // Because we have complemented all the Inf
              // acceptance conditions on the input automaton,
              // we must revert them to create a new edge.
              acc ^= all_inf;
              if (Cosimulation)
                {
                  if (Sba)
                    {
                      // acc should be attached to src, or rather,
                      // in our edge-based representation)
                      // to all edges leaving src.  As we
                      // can't do this here, store this in a table
                      // so we can fix it later.
                      accst[srcst] = acc;
                      acc = {};
                    }
                  gb->new_edge(dst.id(), srcid, one, acc);
                }
              else
                {
                  gb->new_edge(srcid, dst.id(), one, acc);
                }
            }
        };

        // For each class, we will create
        // all the edges between the states.
        for (auto& p: sorted_classes_)
          {
            // All states in p.second have the same class, so just
            // pick the class of the first one first one.
            bdd src = previous_class_[p->second.front()];
            assert(gb->get_state(src.id()) == srcst);

            // Get the signature to derive successors.
            bdd sig = compute_sig(p->second.front());
            assert(signatures.size() == srcst);
            signatures.push_back(sig);

            if (Cosimulation)
              sig = bdd_compose(sig, bddfalse, bdd_var(bdd_initial));

            // Get all the variables in the signature.
            bdd sup_sig = bdd_support(sig);

            // Get the variables in the signature which represent the
            // conditions.
            bdd sup_all_atomic_prop = bdd_exist(sup_sig, nonapvars);

            // Get the part of the signature composed only with the atomic
            // proposition.
            bdd all_atomic_prop = bdd_exist(sig, nonapvars);

            if (!will_use_basis)
              {

                for (bdd one: minterms_of(all_atomic_prop, sup_all_atomic_prop))
                  create_edges(src.id(), one, bdd_restrict(sig, one));
              }
            else
              {
                auto& [begin, end] = split_cond[all_atomic_prop];
                if (begin == end)
                  {
                    begin = res->num_edges() + 1;
                    for (bdd label: es.basis())
                      create_edges(src.id(), label,
                                   bdd_relprod(label, sig,
                                               res->ap_vars()));
                    end = res->num_edges() + 1;
                  }
                else
                  {
                    // We have already split all_atomic_prop once, so
                    // we can simply reuse the set of labels we used
                    // then, avoiding the iteration on es.basis().
                    auto& g = res->get_graph();
                    bdd last = bddfalse;
                    for (unsigned i = begin; i < end; ++i)
                      {
                        bdd label = g.edge_storage(i).cond;
                        if (label == last)
                          continue;
                        last = label;
                        create_edges(src.id(), label,
                                     bdd_relprod(label, sig,
                                                 res->ap_vars()));
                      }
                  }
              }

            ++srcst;
          }

        res->set_init_state(gb->get_state(previous_class_
                                          [a_->get_init_state_number()].id()));

        res->merge_edges(); // This helps merging some edges with
                            // identical conditions, but different
                            // mark sets.

        // Mark all accepting state in a second pass, when
        // dealing with SBA in cosimulation.
        if (Sba && Cosimulation)
          {
            unsigned ns = res->num_states();
            for (unsigned s = 0; s < ns; ++s)
              {
                acc_cond::mark_t acc = accst[s];
                if (!acc)
                  continue;
                for (auto& t: res->out(s))
                  t.acc = acc;
              }
          }

        bool need_another_pass = false;
        // Attempt to merge trivial SCCs
        if (!record_implications_ && res->num_states() > 1)
          {
            scc_info si(res, scc_info_options::NONE);
            unsigned nscc = si.scc_count();
            unsigned nstates = res->num_states();
            std::vector<unsigned> redirect(nstates);
            std::iota(redirect.begin(), redirect.end(), 0);
            for (unsigned s = 0; s < nstates; ++s)
              signatures[s] = bdd_exist(signatures[s], all_proms_);
            bool changed = false;
            bool unchanged = false;
            for (unsigned scc = 0; scc < nscc; ++scc)
              if (si.is_trivial(scc))
                {
                  unsigned s = si.one_state_of(scc);
                  bdd ref = signatures[s];
                  for (unsigned i = 0; i < nstates; ++i)
                    if (si.reachable_state(i)
                        && signatures[i] == ref && !si.is_trivial(si.scc_of(i)))
                      {
                        changed = true;
                        redirect[s] = i;
                        goto if_changed;
                      }
                  unchanged = true;
                if_changed:;
                }
            if (changed)
              {
                if (Cosimulation)
                  for (auto& e: res->edges())
                    e.src = redirect[e.src];
                else
                  for (auto& e: res->edges())
                    e.dst = redirect[e.dst];
                res->set_init_state(redirect[res->get_init_state_number()]);
                if (Cosimulation)
                  // Fix chaining of edges with changed sources.
                  res->merge_edges();
                if (unchanged)
                  need_another_pass = true;
              }

            // Remove unreachable states.
            // In the case of co-simulation, changing the sources
            // of edges, might have created dead states.
            //
            // If we recorded implications for the determinization
            // procedure, we should not remove unreachable states, as
            // that will invalidate the contents of the IMPLICATIONS
            // vector.  It's OK not to purge the result in that case,
            // as the determinization will only explore the reachable
            // part anyway.
            if (Cosimulation)
              res->purge_dead_states();
            else
              res->purge_unreachable_states();
          }

        // Push the common incoming sets to the outgoing edges.
        // Doing so cancels the preprocessing we did in the other
        // direction, to prevent marks from moving around the
        // automaton if we apply simulation several times, and to
        // favor state-based acceptance.
        if (!Sba && !Cosimulation && !record_implications_
            && original_->prop_state_acc())
          {
            // common_in[i] is the set of acceptance set numbers
            // common to all incoming edges of state i.  Only edges
            // inside one SCC matter.
            unsigned ns = res->num_states();
            std::vector<acc_cond::mark_t> common_in(ns);
            scc_info si(res, scc_info_options::NONE);
            for (unsigned s = 0; s < ns; ++s)
              {
                unsigned s_scc = si.scc_of(s);
                bool first = true;
                for (auto& e: res->out(s))
                  {
                    if (si.scc_of(e.dst) != s_scc)
                      continue;
                    if (first)
                      {
                        common_in[s] = e.acc;
                        first = false;
                      }
                    else
                      {
                        common_in[s] &= e.acc;
                      }
                  }
              }
            for (auto& e : res->edges())
              e.acc = (e.acc - common_in[e.dst]) | common_in[e.src];
          }

        delete gb;
        res->prop_copy(original_,
                       { false, // state-based acc forced below
                         true,  // weakness preserved
                         false, true, // determinism improved
                         true, // completeness preserved
                         true, // stutter inv.
                       });

        // weakness can actually be improved
        if (res->prop_weak().is_false())
          res->prop_weak(trival::maybe());
        if (res->prop_very_weak().is_false())
          res->prop_very_weak(trival::maybe());
        if (res->prop_inherently_weak().is_false())
          res->prop_inherently_weak(trival::maybe());
        // !unambiguous and !semi-deterministic are not preserved
        if (!Cosimulation && nb_minato == nb_minterms)
          // Note that nb_minato != nb_minterms does not imply
          // non-deterministic, because of the merge_edges() above.
          res->prop_universal(true);
        if (Sba)
          res->prop_state_acc(true);

        if (!need_another_pass)
          return res;

        direct_simulation<Cosimulation, Sba> sim(res, trans_pruning_);
        return sim.run();
      }


      // Debug:
      // In a first time, print the signature, and the print a list
      // of each state in this partition.
      // In a second time, print foreach state, who is where,
      // where is the new class name.
      void print_partition()
      {
        std::cerr << "\n-----\n\nClasses from previous iteration:\n";
        unsigned ps = previous_class_.size();
        for (unsigned p = 0; p < ps; ++p)
          {
            std::cerr << "- " << a_->format_state(a_->state_from_number(p))
                      << " was in class "
                      << bdd_format_set(a_->get_dict(), previous_class_[p])
                      << '\n';
          }
        std::cerr << "\nPartition:\n";
        std::list<bdd>::iterator it_bdd = used_var_.begin();
        for (auto& p: sorted_classes_)
          {
            std::cerr << "- ";
            if (p->first != bddfalse)
              std::cerr << "new class " << *it_bdd++ << " from ";
            std::cerr << "sig "
                      << bdd_format_isop(a_->get_dict(), p->first);
            std::cerr << '\n';
            for (auto s: p->second)
              std::cerr << "    - "
                        << a_->format_state(a_->state_from_number(s))
                        << '\n';
          }
        std::cerr << "\nClass implications:\n";
        for (auto& p: relation_)
          std::cerr << "  - " << p.first << "  =>  " << p.second << '\n';

      }

    protected:
      // The automaton which is simulated.
      twa_graph_ptr a_;

      // Implications between classes.
      map_bdd_bdd relation_;

      // Represent the class of each state at the previous iteration.
      vector_state_bdd previous_class_;

      // The list of states for each class at the current_iteration.
      // Computed in `update_sig'.
      map_bdd_lstate bdd_lstate_;
      // The above map, sorted by states number instead of BDD
      // identifier to avoid non-determinism while iterating over all
      // states.
      std::vector<map_bdd_lstate::const_iterator> sorted_classes_;

      // The queue of free bdd. They will be used as the identifier
      // for the class.
      std::queue<int> free_var_;

      // The list of used bdd. They are in used as identifier for class.
      std::list<bdd> used_var_;

      // Size of the automaton.
      unsigned int size_a_;

      // Used to know when there is no evolution in the partial order.
      unsigned int po_size_;

      // Whether to compute implications between classes.  This is costly
      // and useless for deterministic automata.
      bool want_implications_;

      // All the class variable:
      bdd all_class_var_;

      // The flag to say if the outgoing state is initial or not
      bdd bdd_initial;

      bdd all_proms_;

      automaton_size stat;

      const const_twa_graph_ptr original_;
      int trans_pruning_;
      std::vector<bdd>* record_implications_;
    };

    template<typename Fun, typename Aut>
    twa_graph_ptr
    wrap_simul(Fun f, const Aut& a, int trans_pruning)
    {
      if (has_separate_sets(a))
        return f(a, trans_pruning);
      // If the input has acceptance sets common to Fin and Inf,
      // separate them before doing the simulation, and merge them
      // back afterwards.  Doing will temporarily introduce more sets
      // and may exceed the number of sets we support.  But it's
      // better than refusing to apply simulation-based reductions to
      // automata sharing Fin/Inf sets.
      auto b = make_twa_graph(a, twa::prop_set::all());
      separate_sets_here(b);
      return simplify_acceptance_here(f(b, trans_pruning));
    }

  } // End namespace anonymous.


  twa_graph_ptr
  simulation(const const_twa_graph_ptr& t, int trans_pruning)
  {
    return wrap_simul([](const const_twa_graph_ptr& t, int trans_pruning) {
      direct_simulation<false, false> simul(t, trans_pruning);
      return simul.run();
    }, t, trans_pruning);
  }

  twa_graph_ptr
  simulation(const const_twa_graph_ptr& t,
             std::vector<bdd>* implications, int trans_pruning)
  {
    return wrap_simul([implications](const const_twa_graph_ptr& t,
                                     int trans_pruning) {
      direct_simulation<false, false> simul(t, trans_pruning, implications);
      return simul.run();
    }, t, trans_pruning);
  }

  twa_graph_ptr
  simulation_sba(const const_twa_graph_ptr& t, int trans_pruning)
  {
    return wrap_simul([](const const_twa_graph_ptr& t, int trans_pruning) {
      direct_simulation<false, true> simul(t, trans_pruning);
      return simul.run();
    }, t, trans_pruning);
  }

  twa_graph_ptr
  cosimulation(const const_twa_graph_ptr& t, int trans_pruning)
  {
    return wrap_simul([](const const_twa_graph_ptr& t, int trans_pruning) {
      direct_simulation<true, false> simul(t, trans_pruning);
      return simul.run();
    }, t, trans_pruning);
  }

  twa_graph_ptr
  cosimulation_sba(const const_twa_graph_ptr& t, int trans_pruning)
  {
    return wrap_simul([](const const_twa_graph_ptr& t, int trans_pruning) {
      direct_simulation<true, true> simul(t, trans_pruning);
      return simul.run();
    }, t, trans_pruning);
  }


  template<bool Sba>
  twa_graph_ptr
  iterated_simulations_(const const_twa_graph_ptr& t, int trans_pruning)
  {
    twa_graph_ptr res = nullptr;
    automaton_size prev;
    automaton_size next(t);

    do
      {
        prev = next;
        direct_simulation<false, Sba> simul(res ? res : t, trans_pruning);
        res = simul.run();
        if (res->prop_universal())
          break;

        direct_simulation<true, Sba> cosimul(res, trans_pruning);
        res = cosimul.run();

        if (Sba)
          res = scc_filter_states(res, false);
        else
          res = scc_filter(res, false);

        next.set_size(res);
      }
    while (prev != next);
    return res;
  }

  twa_graph_ptr
  iterated_simulations(const const_twa_graph_ptr& t, int trans_pruning)
  {
    return wrap_simul(iterated_simulations_<false>, t, trans_pruning);
  }

  twa_graph_ptr
  iterated_simulations_sba(const const_twa_graph_ptr& t, int trans_pruning)
  {
    return wrap_simul(iterated_simulations_<true>, t, trans_pruning);
  }

  template <bool Cosimulation, bool Sba>
  class reduce_sim
  {
  public:
    reduce_sim(const const_twa_graph_ptr& aut);

    twa_graph_ptr run();

  private:
    // If aut_ is deterministic, only the lower left triangle is set.
    std::vector<bool> compute_simulation();

    const_twa_graph_ptr aut_;
    const_twa_graph_ptr original_;

    // Only use if Sba && Cosimulation
    std::vector<acc_cond::mark_t> acc_;
  };

  template <bool Cosimulation, bool Sba>
  reduce_sim<Cosimulation, Sba>::reduce_sim(const const_twa_graph_ptr& aut)
    : original_(aut)
  {
    unsigned n = aut->num_states();

    twa_graph_ptr a = make_twa_graph(aut->get_dict());
    a->copy_ap_of(aut);
    a->new_states(n);
    a->set_init_state(aut->get_init_state_number());

    // Whether we simulate or cosimulate, we transform the acceptance into all
    // fin. If we cosimulate, we also reverse all the edges.
    const auto all_inf = aut->get_acceptance().used_inf_fin_sets().first;

    for (unsigned s = 0; s < n; ++s)
      for (const auto& e : aut->out(s))
        if (Cosimulation)
          a->new_edge(e.dst, e.src, e.cond, e.acc ^ all_inf);
        else
          a->new_edge(e.src, e.dst, e.cond, e.acc ^ all_inf);

    if (!Sba)
      {
        bool state_acc = true;

        // common_out[i] is the set of acceptance set numbers common to
        // all outgoing edges of state i.
        std::vector<acc_cond::mark_t> common_out(n);
        for (unsigned s = 0; s < n; ++s)
          {
            bool first = true;
            for (auto& e : a->out(s))
              if (first)
                {
                  common_out[s] = e.acc;
                  first = false;
                }
              else if (common_out[s] != e.acc)
                {
                  state_acc = false;
                  break;
                }

            if (!state_acc)
              break;
          }

      // Pull the common outgoing sets to the incoming
      // edges.  Doing so seems to favor cases where states
      // can be merged.
      if (state_acc)
        for (auto& e : a->edges())
          e.acc = (e.acc - common_out[e.src]) | common_out[e.dst];
      }

    if (Sba && Cosimulation)
      {
        acc_.reserve(n);
        for (unsigned s = 0; s < n; ++s)
          acc_.push_back(original_->state_acc_sets(s) ^ all_inf);
      }

    aut_ = std::move(a);
  }

  template <bool Cosimulation, bool Sba>
  std::vector<bool> reduce_sim<Cosimulation, Sba>::compute_simulation()
  {
    // At the start, we consider that all the pairs of vertices are simulating
    // each other. At each iteration we detect which ones are not simulating
    // and we remove them from the set. This information propagate backwards,
    // so we only need to check the peers whose successors were updated in the
    // previous iteration. To limit the number of iterations, we update them in
    // reverse topological order.

    const size_t n = aut_->num_states();
    const bool only_bisimu = is_deterministic(aut_);

    // We need to have the predecessors of a state for the backward propagation.
    digraph<void, void> reverse(n, aut_->num_edges());
    reverse.new_states(n);

    for (unsigned s = 0; s < n; ++s)
      for (const auto& e : aut_->out(s))
        reverse.new_edge(e.dst, e.src);

    reverse.sort_edges_([](const auto& e1, const auto& e2)
        {
          if (e1.src != e2.src)
            return e1.src < e2.src;
          return e1.dst < e2.dst;
        });

    // Remove all duplicates.
    auto& edges = reverse.edge_vector();
    edges.erase(std::unique(edges.begin() + 1, edges.end()), edges.end());
    reverse.chain_edges_();

    // Compute a reverse topological order for all the states. So that the
    // algorithm iterates as few times as possible, we must update all the
    // successors of a state before updating it.
    std::vector<unsigned> order(n, 0);
    std::vector<unsigned> todo;
    todo.reserve(n);

    unsigned init = aut_->get_init_state_number();

    {
      unsigned i = Cosimulation ? 0 : n - 1;
      std::vector<bool> seen(n, false);
      todo.push_back(init);
      seen[init] = true;
      while (!todo.empty())
        {
          unsigned cur = todo.back();
          todo.pop_back();
          order[i] = cur;
          i += Cosimulation ? 1 : -1;

          for (const auto& e : original_->out(cur))
            {
              if (!seen[e.dst])
                {
                  seen[e.dst] = true;
                  todo.push_back(e.dst);
                }
            }
          }
    }

    std::vector<bool> can_sim(n * n, true);

    // Test if s1 simulates s2.
    const auto test_sim = [&](size_t s1, size_t s2) -> bool
      {
        auto s_edges = aut_->out(s1);
        auto d_edges = aut_->out(s2);

        // s1 simulates s2 only if for all the edges of s2 there is an edges s1
        // with compatible condition, acceptance and the destinations simulate
        // each other.
        return std::all_of(s_edges.begin(), s_edges.end(),
          [&](const auto& s_edge) -> bool
            {
              size_t i = static_cast<size_t>(s_edge.dst) * n;

              return std::find_if(d_edges.begin(), d_edges.end(),
                [&](const auto& d_edge) -> bool
                  {
                    // Checks if the destinations of the spoiler simulates the
                    // duplicator.
                    if (!can_sim[i + static_cast<size_t>(d_edge.dst)])
                      return false;

                    if (Sba && Cosimulation)
                      {
                        if (!(acc_[d_edge.src]).subset(acc_[s_edge.src]))
                        return false;
                      }
                    else
                      {
                        if (!(d_edge.acc).subset(s_edge.acc))
                          return false;
                      }

                    if (Cosimulation)
                      {
                        if (s_edge.dst == init && d_edge.dst != init)
                          return false;
                      }

                    return bdd_implies(s_edge.cond, d_edge.cond);
                  });
            });
      };

    todo.resize(n, true);

    bool has_changed;
    do
      {
        has_changed = false;
        for (unsigned i : order)
          {
            if (!todo[i])
              continue;

            todo[i] = false;

            // Update all the predecessors that changed on last turn.
            for (const auto& re : reverse.out(i))
              {
                size_t u = re.dst;
                size_t idx = u * n;

                if (only_bisimu)
                  {
                    // If the automaton is deterministic, compute only the
                    // bisimulation.
                    for (size_t v = 0; v < u; ++v)
                      {
                        // u doesn't simulate v
                        if (!can_sim[idx + v])
                          continue;

                        if (!test_sim(u, v) || !test_sim(v, u))
                          {
                            can_sim[u * n + v] = false;
                            has_changed = true;
                            todo[u] = true;
                            todo[v] = true;
                          }
                      }
                  }
                else
                  {
                    for (unsigned v = 0; v < n; ++v)
                      {
                        // u doesn't simulate v
                        if (!can_sim[idx + v])
                          continue;

                        if (!test_sim(u, v))
                          {
                            can_sim[idx + v] = false;
                            has_changed = true;
                            todo[u] = true;
                          }
                      }
                  }
              }
          }
      }
    while (has_changed);

    if (Cosimulation)
      {
        if (!aut_->out(init).begin())
          {
            for (unsigned i = 0; i < n; ++i)
              {
                can_sim[i * n + init] = i == init;
                can_sim[init * n + i] = false;
              }
          }
        else
          for (unsigned i = 0; i < n; ++i)
            {
              // i doesn't simulate init
              can_sim[i * n + init] = i == init;
            }
      }

    return can_sim;
  }


  template <bool Cosimulation, bool Sba>
  twa_graph_ptr reduce_sim<Cosimulation, Sba>::run()
  {
    std::vector<bool> can_sim = compute_simulation();

    twa_graph_ptr res = std::make_shared<twa_graph>(original_->get_dict());
    res->copy_ap_of(original_);
    res->copy_acceptance_of(original_);

    twa_graph_ptr no_mark = std::make_shared<twa_graph>(original_->get_dict());
    no_mark->copy_ap_of(original_);
    no_mark->copy_acceptance_of(original_);

    unsigned init = original_->get_init_state_number();

    size_t n = original_->num_states();
    std::vector<unsigned> equiv_states(n);

    // If the automaton is deterministic, there is no simple simulation only
    // bisimulation.
    bool is_deter = is_deterministic(original_);

    // The number of states in the reduced automaton.
    // There is at least an initial state.
    unsigned nr = 1;
    equiv_states[0] = 0;

    std::vector<unsigned> old;
    old.reserve(n);
    old.push_back(0);

    // If two states bisimulate each other, they will be the same state in the
    // reduced automaton.
    for (size_t i = 1; i < n; ++i)
      {
        bool found = false;

        size_t j = 0;
        for (; j < i; ++j)
          {
            if (can_sim[i * n + j] && (is_deter || can_sim[j * n + i]))
              {
                equiv_states[i] = equiv_states[j];
                found = true;
                break;
              }
          }

        if (!found)
          {
            equiv_states[i] = nr++;
            old.push_back(i);
          }
      }
    res->new_states(nr);
    no_mark->new_states(nr);

    const auto& gr = aut_->get_graph();

    // Test if the language recognized by taking e1 is included in e2 (in this
    // case e2 dominates e1).
    const auto dominates_edge = [&](const auto& e2)
      {
        return [&](const auto& e1) -> bool
          {
            if (Cosimulation && e2.dst == init && e1.dst != init)
              return false;

            if (Sba && Cosimulation)
              {
                if (!(acc_[e1.src]).subset(acc_[e2.src]))
                  return false;
              }
            else
              {
                if (!(e1.acc).subset(e2.acc))
                  return false;
              }

            // e1.dst simulates e2.dst
            return can_sim[e2.dst * n + e1.dst]
              // if e2.dst also simulates e1.dst, the edge with the higher
              // index is the dominator (this is arbitrary, but without that
              // we would remove both edges)
              && (!can_sim[e1.dst * n + e2.dst]
                  || gr.index_of_edge(e1) > gr.index_of_edge(e2))
              // of course the condition of e2 should implies that of e1, but
              // this we test this last because it is slow.
              && bdd_implies(e2.cond, e1.cond);
          };
      };

    const auto all_inf = original_->get_acceptance().used_inf_fin_sets().first;
    for (unsigned i = 0; i < nr; ++i)
      {
        auto out = aut_->out(old[i]);

        for (const auto& e : out)
          {
            // If the language recognized by taking e is include in the
            // language of an other edge, we don't want to add it.
            // If the automaton is deterministic, this cannot happen since no
            // simple simulation are possible.
            if (is_deter
                || std::none_of(out.begin(), out.end(), dominates_edge(e)))
              {
                auto acc = e.acc ^ all_inf;
                if (Cosimulation)
                  res->new_edge(equiv_states[e.dst], i, e.cond, acc);
                else
                  res->new_edge(i, equiv_states[e.dst], e.cond, acc);

                no_mark->new_edge(i, equiv_states[e.dst], e.cond);
              }
          }
      }

    res->set_init_state(equiv_states[init]);
    no_mark->set_init_state(equiv_states[init]);
    no_mark->merge_edges();

    scc_info si_no_mark(no_mark, scc_info_options::NONE);
    unsigned nscc = si_no_mark.scc_count();

    std::vector<unsigned> redirect(no_mark->num_states());
    std::iota(redirect.begin(), redirect.end(), 0);

    // Same as in compute_simulation(), but since we are between two sccs, we
    // can ignore the colors.
    const auto test_sim = [&](size_t s1, size_t s2) -> bool
      {
        auto s_edges = no_mark->out(s1);
        auto d_edges = no_mark->out(s2);

        return std::all_of(s_edges.begin(), s_edges.end(),
          [&](const auto& s_edge) -> bool
            {
              size_t idx = static_cast<size_t>(old[s_edge.dst]) * n;

              return std::find_if(d_edges.begin(), d_edges.end(),
                [&](const auto& d_edge) -> bool
                  {
                    if (!can_sim[idx + static_cast<size_t>(old[d_edge.dst])])
                      return false;

                    return bdd_implies(s_edge.cond, d_edge.cond);
                  });
            });
      };

    // Attempt to merge trivial sccs.
    for (unsigned scc = 0; scc < nscc; ++scc)
      {
        if (!si_no_mark.is_trivial(scc))
          continue;

        unsigned s = si_no_mark.one_state_of(scc);

        for (unsigned i = 0; i < nr; ++i)
          {
            if (si_no_mark.reachable_state(i)
                && !si_no_mark.is_trivial(si_no_mark.scc_of(i)))
              {
                if (test_sim(i, s) && test_sim(s, i))
                  {
                    can_sim[old[i] * n + old[s]] = true;
                    can_sim[old[s] * n + old[i]] = true;

                    if (Cosimulation)
                      redirect[i] = s;
                    else
                      redirect[s] = i;

                    break;
                  }
              }
          }
      }

    for (auto& e: res->edges())
      e.dst = redirect[e.dst];

    if (!Sba && !Cosimulation && original_->prop_state_acc())
      {
        // common_in[i] is the set of acceptance set numbers
        // common to all incoming edges of state i.  Only edges
        // inside one SCC matter.
        //
        // ns = nr
        std::vector<acc_cond::mark_t> common_in(nr);
        scc_info si(res, scc_info_options::NONE);

        for (unsigned s = 0; s < nr; ++s)
          {
            unsigned s_scc = si.scc_of(s);
            bool first = true;
            for (auto& e: res->out(s))
              {
                if (si.scc_of(e.dst) != s_scc)
                  continue;
                if (first)
                  {
                    common_in[s] = e.acc;
                    first = false;
                  }
                else
                  {
                    common_in[s] &= e.acc;
                  }
              }
          }
        for (auto& e : res->edges())
          e.acc = (e.acc - common_in[e.dst]) | common_in[e.src];
      }

    res->merge_edges();
    res->set_init_state(redirect[res->get_init_state_number()]);
    res->purge_unreachable_states();
    res->prop_copy(original_,
        { Sba,          // state-based acc
          true,         // weakness preserved,
          false, true,  // determinism improved
          true,         // completeness preserved
          true,         // stutter inv.
        });

    return res;
  }

  twa_graph_ptr reduce_direct_sim(const const_twa_graph_ptr& aut)
  {
    // The automaton must not have dead or unreachable states.
    reduce_sim<false, false> r(scc_filter(aut));
    return r.run();
  }

  twa_graph_ptr reduce_direct_sim_sba(const const_twa_graph_ptr& aut)
  {
    // The automaton must not have dead or unreachable states.
    reduce_sim<false, true> r(scc_filter_states(aut));
    return r.run();
  }

  twa_graph_ptr reduce_direct_cosim(const const_twa_graph_ptr& aut)
  {
    // The automaton must not have dead or unreachable states.
    reduce_sim<true, false> r(scc_filter(aut));
    return r.run();
  }

  twa_graph_ptr reduce_direct_cosim_sba(const const_twa_graph_ptr& aut)
  {
    // The automaton must not have dead or unreachable states.
    reduce_sim<true, true> r(scc_filter_states(aut));
    return r.run();
  }

  template <bool Sba>
  twa_graph_ptr reduce_iterated_(const const_twa_graph_ptr& aut)
  {
    unsigned last_states = aut->num_states();
    unsigned last_edges = aut->num_edges();

    auto a = Sba ? scc_filter_states(aut) : scc_filter(aut);

    reduce_sim<false, Sba> r(a);
    twa_graph_ptr res = r.run();

    bool cosim = true;
    do
      {
        if (is_deterministic(res))
          break;

        last_states = res->num_states();
        last_edges = res->num_edges();

        if (cosim)
          {
            reduce_sim<true, Sba> r(res);
            res = r.run();
          }
        else
          {
            reduce_sim<false, Sba> r(res);
            res = r.run();
          }

        cosim = !cosim;

      }
    while (last_states > res->num_states() || last_edges > res->num_edges());

    return res;
  }

  twa_graph_ptr reduce_iterated(const const_twa_graph_ptr& aut)
  {
    return reduce_iterated_<false>(aut);
  }

  twa_graph_ptr reduce_iterated_sba(const const_twa_graph_ptr& aut)
  {
    return reduce_iterated_<true>(aut);
  }
} // End namespace spot.

#if __GNUC__ <= 7
#pragma GCC diagnostic pop
#endif
