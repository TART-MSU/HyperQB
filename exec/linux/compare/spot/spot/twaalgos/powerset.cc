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
#include <set>
#include <iterator>
#include <vector>
#include <spot/twaalgos/powerset.hh>
#include <spot/twaalgos/sccinfo.hh>
#include <spot/twaalgos/cycles.hh>
#include <spot/twaalgos/product.hh>
#include <spot/twaalgos/sccfilter.hh>
#include <spot/twaalgos/ltl2tgba_fm.hh>
#include <spot/twaalgos/dualize.hh>
#include <spot/twaalgos/split.hh>
#include <spot/misc/bitvect.hh>
#include <spot/misc/bddlt.hh>

namespace spot
{
  namespace
  {
    static power_map::power_state
    bv_to_ps(const bitvect* in)
    {
      power_map::power_state ps;
      unsigned ns = in->size();
      for (unsigned pos = 0; pos < ns; ++pos)
        if (in->get(pos))
          ps.insert(pos);
      return ps;
    }

    struct bv_hash
    {
      size_t operator()(const bitvect* bv) const noexcept
      {
        return bv->hash();
      }
    };

    struct bv_equal
    {
      bool operator()(const bitvect* bvl, const bitvect* bvr) const
      {
        return *bvl == *bvr;
      }
    };
  }

  std::ostream& output_aborter::print_reason(std::ostream& os) const
  {
    os << "more than ";
    if (reason_is_states_)
      os << max_states_ << " states required";
    else
      os << max_edges_ << " edges required";
    return os;
  }

  twa_graph_ptr
  tgba_powerset(const const_twa_graph_ptr& aut, power_map& pm, bool merge,
                const output_aborter* aborter,
                std::vector<unsigned>* accepting_sinks)
  {
    unsigned ns = aut->num_states();
    unsigned nap = aut->ap().size();

    if ((-1UL / ns) >> nap == 0)
      throw std::runtime_error("too many atomic propositions (or states)");

    // we have two ways of "spliting" the labels when determinizing.
    // One is to iterate over 2^AP, the second is to partition the set
    // of edges labels.  We don't have a very clean rule to chose. The
    // former is expansive when we have a lot of AP.  The latter is
    // good when we have few distinct labels.  With too many different
    // labels that may have nonempty intersections, the
    // partition-based approach can consume a lot of memory.
    bool will_use_labels = nap > 5;
    edge_separator es;
    if (will_use_labels)
      {
        // Gather all labels, but stop if we see too many.  The
        // threshold below is arbitrary: adjust if you know better.
        will_use_labels = es.add_to_basis(aut, 256 * nap);
      }

    // Build a correspondence between conjunctions of APs and unsigned
    // indexes.
    std::vector<bdd> num2bdd;
    num2bdd.reserve(1UL << nap);
    std::map<bdd, unsigned, bdd_less_than> bdd2num;
    bdd allap = aut->ap_vars();

    if (!will_use_labels)
      for (bdd one: minterms_of(bddtrue, allap))
        {
          bdd2num.emplace(one, num2bdd.size());
          num2bdd.emplace_back(one);
        }
    else
      for (bdd one: es.basis())
        {
          bdd2num.emplace(one, num2bdd.size());
          num2bdd.emplace_back(one);
        }

    size_t nc = num2bdd.size();        // number of conditions
    assert(will_use_labels || nc == (1UL << nap));

    // Conceptually, we represent the automaton as an array 'bv' of
    // ns*nc bit vectors of size 'ns'.  Each original state is
    // represented by 'nc' consecutive bitvectors representing the
    // possible destinations for each condition.
    //
    //  src  cond
    //  0   !a&!b   [...bit vector of size ns...]
    //      !a&b    [...bit vector of size ns...]
    //       a&!b   [...bit vector of size ns...]
    //       a&b    [...bit vector of size ns...]
    //  1   !a&!b   [...bit vector of size ns...]
    //      !a&b    [...bit vector of size ns...]
    //       a&!b   [...bit vector of size ns...]
    //       a&b    [...bit vector of size ns...]
    //  2   !a&!b   [...bit vector of size ns...]
    //      !a&b    [...bit vector of size ns...]
    //       a&!b   [...bit vector of size ns...]
    //       a&b    [...bit vector of size ns...]
    //  ...
    //
    // Since there are nc possible "cond" value, and ns sources, the
    // ns*nc bitvectors of ns bits each can take a lot of space.  In
    // issue #302, we had the case of an automaton with ns=8777
    // states, and 8 atomic propositions (nc=256): this large array
    // would require 2.3GB, causing out-of-memory error on small
    // systems.
    //
    // To work around this, we reduce the number of states we store in
    // this array to reduced_ns, which we currently limit to 512
    // (chosen arbitrarily), and use it as a least-recently-used
    // cache.  A separate vector of size ns, contains pointers
    // (i.e. iterators) to a list cell that gives an index in this
    // cache.  The purpose of the list is to maintain the
    // least-recently-used order.
    typedef std::list<std::pair<unsigned, unsigned>>::const_iterator iter;
    std::list<std::pair<unsigned, unsigned>> lru; // list of (idx in bv, state#)
    std::vector<iter> iters(ns, lru.end());
    const unsigned reduced_ns = std::min(512U, ns);
    auto bv =
      std::unique_ptr<bitvect_array>(make_bitvect_array(ns, reduced_ns * nc));

    // Get the index of src in bv, filling bv in an LRU-fashion if needed.
    auto index = [&](unsigned src) {
      iter i = iters[src];
      if (i != lru.end())
        {
          // bv has already been filled for this state.  Just move it
          // to the front of the LRU list.
          lru.splice(lru.begin(), lru, i);
          return i->first;
        }

      unsigned idx = lru.size();
      bool reused = false;
      if (idx < reduced_ns)
        {
          lru.emplace_front(idx, src);
        }
      else
        {
          unsigned state;
          std::tie(idx, state) = lru.back();
          iters[state] = lru.end();
          lru.pop_back();
          lru.emplace_front(idx, src);
          reused = true;
        }
      iters[src] = lru.begin();

      size_t base = idx * nc;
      if (reused)
        for (unsigned i = 0; i < nc; ++i)
          bv->at(base + i).clear_all();
      for (auto& t: aut->out(src))
        if (!will_use_labels)
          for (bdd one: minterms_of(t.cond, allap))
            {
              unsigned num = bdd2num[one];
              bv->at(base + num).set(t.dst);
            }
        else
          for (bdd one: es.separate_implying(t.cond))
            {
              unsigned num = bdd2num[one];
              bv->at(base + num).set(t.dst);
            }
      assert(idx == lru.begin()->first);
      return idx;
    };

    typedef power_map::power_state power_state;

    typedef std::unordered_map<bitvect*, unsigned, bv_hash, bv_equal> power_set;
    power_set seen;

    std::vector<const bitvect*> toclean;

    auto res = make_twa_graph(aut->get_dict());
    res->copy_ap_of(aut);

    bitvect* acc_sinks = nullptr;
    if (accepting_sinks)
      {
        acc_sinks = make_bitvect(ns);
        for (unsigned s: *accepting_sinks)
          acc_sinks->set(s);
        toclean.emplace_back(acc_sinks);

        // The accepting sink is the first registered state by
        // convention.
        power_state ps = bv_to_ps(acc_sinks);
        unsigned num = res->new_state();
        seen[acc_sinks] = num;
        assert(pm.map_.size() == num);
        pm.map_.emplace_back(std::move(ps));
      }

    // Add the initial state unless it's a sink.
    if (unsigned init_num = aut->get_init_state_number();
        !acc_sinks || !acc_sinks->get(init_num))
      {
        auto bvi = make_bitvect(ns);
        bvi->set(init_num);
        power_state ps{init_num};
        unsigned num = res->new_state();
        res->set_init_state(num);
        seen[bvi] = num;
        assert(pm.map_.size() == num);
        pm.map_.emplace_back(std::move(ps));
        toclean.emplace_back(bvi);
    }

    // outgoing map
    auto om = std::unique_ptr<bitvect_array>(make_bitvect_array(ns, nc));

    // Map a bitvector to a state number, possibly creating the state.
    auto to_state = [&](bitvect* dst) {
      if (acc_sinks && dst->intersects(*acc_sinks))
        *dst = *acc_sinks;
      auto i = seen.find(dst);
      if (i != seen.end())
        return i->second;
      unsigned dst_num = res->new_state();
      auto dst2 = dst->clone();
      seen[dst2] = dst_num;
      toclean.emplace_back(dst2);
      auto ps = bv_to_ps(dst);
      assert(pm.map_.size() == dst_num);
      pm.map_.emplace_back(std::move(ps));
      return dst_num;
    };
    auto& graph = aut->get_graph();
    for (unsigned src_num = 0; src_num < res->num_states(); ++src_num)
      {
        const power_state& src = pm.states_of(src_num);
        unsigned srcsz = src.size();
        if (srcsz == 0)
          continue;
        om->clear_all();
        // If the meta-state is a singleton {st} with can avoid
        // some bitvector work in case st has 0 or 1 edge.
        if (srcsz == 1)
          {
            unsigned st = *src.begin();
            auto& st_storage = graph.state_storage(st);
            unsigned e = st_storage.succ;
            if (SPOT_UNLIKELY(e == 0U)) // no edge
              continue;
            if (e == st_storage.succ_tail) // single edge
              {
                auto& ed_storage = graph.edge_storage(e);
                bitvect& bv = om->at(0);
                bv.set(ed_storage.dst);
                res->new_edge(src_num, to_state(&bv), ed_storage.cond);
                // Don't bother with the aborter here, as this path is
                // clearly not exploding.
                continue;
              }
          }
        for (auto s: src)
          {
            size_t base = index(s) * nc;
            for (unsigned c = 0; c < nc; ++c)
              om->at(c) |= bv->at(base + c);
          }
        for (unsigned c = 0; c < nc; ++c)
          {
            auto dst = &om->at(c);
            if (dst->is_fully_clear())
              continue;
            unsigned dst_num = to_state(dst);
            res->new_edge(src_num, dst_num, num2bdd[c]);
            if (aborter && aborter->too_large(res))
              {
                for (auto v: toclean)
                  delete v;
                return nullptr;
              }
          }
      }

    for (auto v: toclean)
      delete v;
    if (merge)
      res->merge_edges();
    return res;
  }

  twa_graph_ptr
  tgba_powerset(const const_twa_graph_ptr& aut,
                const output_aborter* aborter,
                std::vector<unsigned>* accepting_sinks)
  {
    power_map pm;
    return tgba_powerset(aut, pm, true, aborter, accepting_sinks);
  }


  namespace
  {

    class fix_scc_acceptance final: protected enumerate_cycles
    {
    public:
      typedef dfs_stack::const_iterator cycle_iter;
      typedef twa_graph_edge_data trans;
      typedef std::set<trans*> edge_set;
      typedef std::vector<edge_set> set_set;
    protected:
      const_twa_graph_ptr ref_;
      power_map& refmap_;
      edge_set reject_;         // set of rejecting edges
      set_set accept_;          // set of cycles that are accepting
      edge_set all_;            // all non rejecting edges
      unsigned threshold_;      // maximum count of enumerated cycles
      unsigned cycles_left_;    // count of cycles left to explore

    public:
      fix_scc_acceptance(const scc_info& sm, const_twa_graph_ptr ref,
                         power_map& refmap, unsigned threshold)
        : enumerate_cycles(sm), ref_(ref), refmap_(refmap),
          threshold_(threshold)
      {
      }

      bool fix_scc(const int m)
      {
        reject_.clear();
        accept_.clear();
        cycles_left_ = threshold_;
        run(m);

//        std::cerr << "SCC #" << m << '\n';
//        std::cerr << "REJECT: ";
//        print_set(std::cerr, reject_) << '\n';
//        std::cerr << "ALL: ";
//        print_set(std::cerr, all_) << '\n';
//        for (set_set::const_iterator j = accept_.begin();
//             j != accept_.end(); ++j)
//          {
//            std::cerr << "ACCEPT: ";
//            print_set(std::cerr, *j) << '\n';
//          }

        auto acc = aut_->acc().all_sets();
        for (auto i: all_)
          i->acc = acc;
        return threshold_ != 0 && cycles_left_ == 0;
      }

      bool is_cycle_accepting(cycle_iter begin, edge_set& ts) const
      {
        auto a = std::const_pointer_cast<twa_graph>(aut_);

        // Build an automaton representing this loop.
        auto loop_a = make_twa_graph(aut_->get_dict());
        int loop_size = std::distance(begin, dfs_.end());
        loop_a->new_states(loop_size);
        int n;
        cycle_iter i;
        for (n = 1, i = begin; n <= loop_size; ++n, ++i)
          {
            trans* t = &a->edge_data(i->succ);
            loop_a->new_edge(n - 1, n % loop_size, t->cond);
            if (reject_.find(t) == reject_.end())
              ts.insert(t);
          }
        assert(i == dfs_.end());

        unsigned loop_a_init = loop_a->get_init_state_number();
        assert(loop_a_init == 0);

        // Check if the loop is accepting in the original automaton.
        bool accepting = false;

        // Iterate on each original state corresponding to the
        // start of the loop in the determinized automaton.
        for (auto s: refmap_.states_of(begin->s))
          {
            // Check the product between LOOP_A, and ORIG_A starting
            // in S.
            if (!product(loop_a, ref_, loop_a_init, s)->is_empty())
              {
                accepting = true;
                break;
              }
          }
        return accepting;
      }

//      std::ostream&
//      print_set(std::ostream& o, const edge_set& s) const
//      {
//        o << "{ ";
//        for (auto i: s)
//          o << i << ' ';
//        o << '}';
//        return o;
//      }

      virtual bool
      cycle_found(unsigned start) override
      {
        cycle_iter i = dfs_.begin();
        while (i->s != start)
          ++i;
        edge_set ts;
        bool is_acc = is_cycle_accepting(i, ts);
        do
          ++i;
        while (i != dfs_.end());
        if (is_acc)
          {
            accept_.emplace_back(ts);
            all_.insert(ts.begin(), ts.end());
          }
        else
          {
            for (auto t: ts)
              {
                reject_.insert(t);
                for (auto& j: accept_)
                  j.erase(t);
                all_.erase(t);
              }
          }

        // Abort this algorithm if we have seen too many cycles, i.e.,
        // when cycle_left_ *reaches* 0.  (If cycle_left_ == 0, that
        // means we had no limit.)
        return (cycles_left_ == 0) || --cycles_left_;
      }
    };

    static bool
    fix_dba_acceptance(twa_graph_ptr det,
                       const_twa_graph_ptr ref, power_map& refmap,
                       unsigned threshold)
    {
      det->copy_acceptance_of(ref);

      scc_info sm(det, scc_info_options::NONE);

      unsigned scc_count = sm.scc_count();

      fix_scc_acceptance fsa(sm, ref, refmap, threshold);

      for (unsigned m = 0; m < scc_count; ++m)
        if (!sm.is_trivial(m))
          if (fsa.fix_scc(m))
            return true;
      return false;
    }
  }

  twa_graph_ptr
  tba_determinize(const const_twa_graph_ptr& aut,
                  unsigned threshold_states, unsigned threshold_cycles)
  {
    power_map pm;
    // Do not merge edges in the deterministic automaton.  If we
    // add two self-loops labeled by "a" and "!a", we do not want
    // these to be merged as "1" before the acceptance has been fixed.

    unsigned max_states = aut->num_states() * threshold_states;
    if (max_states == 0)
      max_states = ~0U;
    output_aborter aborter(max_states);
    auto det = tgba_powerset(aut, pm, false, &aborter);
    if (!det)
      return nullptr;
    if (fix_dba_acceptance(det, aut, pm, threshold_cycles))
      return nullptr;
    det->merge_edges();
    return det;
  }

  twa_graph_ptr
  tba_determinize_check(const twa_graph_ptr& aut,
                        unsigned threshold_states,
                        unsigned threshold_cycles,
                        formula f,
                        const_twa_graph_ptr neg_aut)
  {
    if (f == nullptr && neg_aut == nullptr)
      return nullptr;
    if (aut->num_sets() > 1)
      return nullptr;

    auto det = tba_determinize(aut, threshold_states, threshold_cycles);

    if (!det)
      return nullptr;

    if (neg_aut == nullptr)
      {
        neg_aut = ltl_to_tgba_fm(formula::Not(f), aut->get_dict());
        // Remove useless SCCs.
        neg_aut = scc_filter(neg_aut, true);
      }

    if (!det->intersects(neg_aut) && !aut->intersects(dualize(det)))
      // Determinization was safe.
      return det;

    return aut;
  }
}
