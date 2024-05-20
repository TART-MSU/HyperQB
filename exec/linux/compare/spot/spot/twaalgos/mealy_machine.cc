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
#include <spot/twaalgos/mealy_machine.hh>

#include <queue>
#include <deque>
#include <set>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>

#include <spot/misc/bddlt.hh>
#include <spot/misc/hash.hh>
#include <spot/misc/timer.hh>
#include <spot/misc/minato.hh>
#include <spot/twaalgos/game.hh>
#include <spot/twaalgos/product.hh>
#include <spot/twaalgos/synthesis.hh>
#include <spot/priv/partitioned_relabel.hh>

#include <picosat/picosat.h>


//#define TRACE
#ifdef TRACE
#  define trace std::cerr
#else
#  define trace while (0) std::cerr
#endif

//#define MINTIMINGS
#ifdef MINTIMINGS
#  define dotimeprint std::cerr
#else
#  define dotimeprint while (0) std::cerr
#endif

namespace
{
  using namespace spot;
  bool is_deterministic_(const std::vector<bdd>& ins)
  {
    const unsigned n_ins = ins.size();
    for (unsigned idx1 = 0; idx1 < n_ins - 1; ++idx1)
      for (unsigned idx2 = idx1 + 1; idx2 < n_ins; ++idx2)
        if (bdd_have_common_assignment(ins[idx1], ins[idx2]))
          return false;
    return true;
  }

#ifndef NDEBUG
  bool is_complete_(const const_twa_graph_ptr& m,
                    const bdd& outs)
  {
    auto* sp = m->get_named_prop<region_t>("state-player");
    const auto N = m->num_states();
    for (auto s = 0u; s < N; ++s)
      {
        if (sp && sp->at(s))
          continue; // No need tpo check player states
        bdd all_cond = bddfalse;
        for (const auto& e : m->out(s))
          all_cond |= bdd_exist(e.cond, outs);
        if (all_cond != bddtrue)
          return false;
      }
    return true;
  }
#endif
}

namespace
{
  static std::unique_ptr<std::ofstream> sat_csv_file;
  struct fwrapper{
    std::string fname;
    std::FILE* f;
    fwrapper(const std::string& name)
      : fname{name}
      , f{std::fopen(name.c_str(), "a")}
    {
      if (!f)
        throw std::runtime_error("`" + name +
                                 "' could not be opened for writing.");
    }
    ~fwrapper()
    {
      std::fclose(f);
      f = nullptr;
    }
    fwrapper& operator=(const fwrapper&) = delete;
    fwrapper& operator=(fwrapper&&) = delete;
    fwrapper(const fwrapper&) = delete;
    fwrapper(fwrapper&&) = delete;
  };
  static std::unique_ptr<fwrapper> sat_dimacs_file;
  static std::string sat_instance_name = "";
}


namespace spot
{

  static bdd
  ensure_mealy(const char* function_name,
               const const_twa_graph_ptr& m)
  {
    if (SPOT_UNLIKELY(!m->acc().is_t()))
      throw std::runtime_error(std::string(function_name) +
                               "(): Mealy machines must have "
                               "true acceptance condition");
    bdd* out = m->get_named_prop<bdd>("synthesis-outputs");
    if (SPOT_UNLIKELY(!out))
      throw std::runtime_error(std::string(function_name) +
                               "(): \"synthesis-outputs\" not defined");
    return *out;
  }

  bool
  is_mealy(const const_twa_graph_ptr& m)
  {
    if (!m->acc().is_t())
      {
        trace << "is_mealy(): Mealy machines must have "
          "true acceptance condition.\n";
        return false;
      }

    if (!m->get_named_prop<bdd>("synthesis-outputs"))
      {
        trace << "is_mealy(): \"synthesis-outputs\" not found!\n";
        return false;
      }

    return true;
  }

  bool
  is_separated_mealy(const const_twa_graph_ptr& m)
  {
    if (!is_mealy(m))
      return false;

    auto outs = get_synthesis_outputs(m);

    for (const auto& e : m->edges())
      if ((bdd_exist(e.cond, outs) & bdd_existcomp(e.cond, outs)) != e.cond)
        {
          trace << "is_separated_mealy_machine(): Edge number "
                << m->edge_number(e) << " from " << e.src << " to " << e.dst
                << " with " << e.cond << " is not separated!\n";
          return false;
        }

    return true;
  }

  bool
  is_split_mealy(const const_twa_graph_ptr& m)
  {
    if (!is_mealy(m))
      return false;

    if (!m->get_named_prop<region_t>("state-player"))
      {
        trace << "is_split_mealy(): Split mealy machine must define the named "
                 "property \"state-player\"!\n";
        return false;
      }

    auto sp = get_state_players(m);

    if (sp.size() != m->num_states())
      {
        trace << "\"state-player\" has not the same size as the "
                 "automaton!\n";
        return false;
      }

    if (sp[m->get_init_state_number()])
      {
        trace << "is_split_mealy(): Initial state must be owned by env!\n";
        return false;
      }

    auto outs = get_synthesis_outputs(m);

    for (const auto& e : m->edges())
      {
        if (sp[e.src] == sp[e.dst])
          {
            trace << "is_split_mealy(): Edge number "
                  << m->edge_number(e) << " from " << e.src << " to " << e.dst
                  << " is not alternating!\n";
            return false;
          }
        if (sp[e.src] && (bdd_exist(e.cond, outs) != bddtrue))
          {
            trace << "is_split_mealy(): Edge number "
                  << m->edge_number(e) << " from " << e.src << " to " << e.dst
                  << " depends on inputs, but should be labeled by outputs!\n";
            return false;
          }
        if (!sp[e.src] && (bdd_existcomp(e.cond, outs) != bddtrue))
          {
            trace << "is_split_mealy(): Edge number "
                  << m->edge_number(e) << " from " << e.src << " to " << e.dst
                  << " depends on outputs, but should be labeled by inputs!\n";
            return false;
          }
      }
    return true;
  }

  bool
  is_input_deterministic_mealy(const const_twa_graph_ptr& m)
  {
    if (!is_mealy(m))
      return false;

    const unsigned N = m->num_states();

    auto sp_ptr = m->get_named_prop<region_t>("state-player");
    // Unsplit mealy -> sp_ptr == nullptr
    if (sp_ptr && !is_split_mealy(m))
      return false;

    auto outs = get_synthesis_outputs(m);

    std::vector<bdd> ins;
    for (unsigned s = 0; s < N; ++s)
      {
        if (sp_ptr && (*sp_ptr)[s])
          continue;
        ins.clear();
        for (const auto& e : m->out(s))
          ins.push_back(sp_ptr ? e.cond
                               : bdd_exist(e.cond, outs));
        if (!is_deterministic_(ins))
          {
            trace << "is_input_deterministic_mealy(): State number "
                  << s << " is not input determinist!\n";
            return false;
          }
      }
    return true;
  }

  void
  split_separated_mealy_here(const twa_graph_ptr& m)
  {
    bdd output_bdd = ensure_mealy("split_separated_mealy_here", m);

    struct dst_cond_color_t
    {
      std::pair<unsigned, int> dst_cond;
      acc_cond::mark_t color;
    };

    auto hasher = [](const dst_cond_color_t& dcc) noexcept
      {
        return dcc.color.hash() ^ pair_hash()(dcc.dst_cond);
      };
    auto equal = [](const dst_cond_color_t& dcc1,
                    const dst_cond_color_t& dcc2) noexcept
      {
        return (dcc1.dst_cond == dcc2.dst_cond)
               && (dcc1.color == dcc2.color);
      };

    std::unordered_map<dst_cond_color_t, unsigned,
                       decltype(hasher),
                       decltype(equal)> player_map(m->num_edges(),
                                                   hasher, equal);

    auto get_ps = [&](unsigned dst, const bdd& ocond,
                      acc_cond::mark_t color)
      {
        dst_cond_color_t key{std::make_pair(dst, ocond.id()),
                             color};
        auto [it, inserted] =
            player_map.try_emplace(key, m->num_states());
        if (!inserted)
          return it->second;
        unsigned ns = m->new_state();
        assert(ns == it->second);
        m->new_edge(ns, dst, ocond, color);
        return ns;
      };

    unsigned ne = m->edge_vector().size();
    for (unsigned eidx = 1; eidx < ne; ++eidx)
      {
        const auto& e = m->edge_storage(eidx);
        if (e.next_succ == eidx) // dead edge
          continue;
        bdd incond = bdd_exist(e.cond, output_bdd);
        bdd outcond = bdd_existcomp(e.cond, output_bdd);
        assert(((incond&outcond) == e.cond) && "Precondition violated");
        // Create new state and trans
        // this may invalidate "e".
        unsigned new_dst = get_ps(e.dst, outcond, e.acc);
        // redirect
        auto& e2 = m->edge_storage(eidx);
        e2.dst = new_dst;
        e2.cond = incond;
      }

    auto* sp_ptr = m->get_or_set_named_prop<region_t>("state-player");

    sp_ptr->resize(m->num_states());
    std::fill(sp_ptr->begin(), sp_ptr->end(), false);
    for (const auto& eit : player_map)
      (*sp_ptr)[eit.second] = true;
    //Done
  }

  twa_graph_ptr
  split_separated_mealy(const const_twa_graph_ptr& m)
  {
    bdd outputs = ensure_mealy("split_separated_mealy", m);
    auto m2 = make_twa_graph(m, twa::prop_set::all());
    m2->copy_acceptance_of(m);
    set_synthesis_outputs(m2, outputs);
    split_separated_mealy_here(m2);
    return m2;
  }

  twa_graph_ptr
  unsplit_mealy(const const_twa_graph_ptr& m)
  {
    assert(is_split_mealy(m));
    return unsplit_2step(m);
  }

}

namespace
{
  // Anonymous for reduce_mealy
  using namespace spot;

  // Used to get the signature of the state.
  typedef std::vector<bdd> vector_state_bdd;

  // Get the list of state for each class.
  // Note: Use map as iter. are not invalidated by inserting new elements
  typedef std::map<bdd, std::vector<unsigned>,
                   bdd_less_than> map_bdd_lstate;

  // This part is just a copy of a part of simulation.cc only suitable for
  // deterministic monitors.
  class sig_calculator final
  {
  protected:
    typedef std::unordered_map<bdd, bdd, bdd_hash> map_bdd_bdd;
    int acc_vars;
    acc_cond::mark_t all_inf_;

  public:
    sig_calculator(twa_graph_ptr aut, bool implications) : a_(aut),
        po_size_(0),
        want_implications_(implications)
    {
      size_a_ = a_->num_states();
      // Now, we have to get the bdd which will represent the
      // class. We register one bdd by state, because in the worst
      // case, |Class| == |State|.
      unsigned set_num = a_->get_dict()
                           ->register_anonymous_variables(size_a_, this);

      bdd init = bdd_ithvar(set_num++);

      used_var_.emplace_back(init);

      // Initialize all classes to init.
      previous_class_.resize(size_a_);
      for (unsigned s = 0; s < size_a_; ++s)
        previous_class_[s] = init;
      for (unsigned i = set_num; i < set_num + size_a_ - 1; ++i)
        free_var_.push(i);

      relation_.reserve(size_a_);
      relation_[init] = init;
    }

    // Reverse all the acceptance condition at the destruction of
    // this object, because it occurs after the return of the
    // function simulation.
    virtual ~sig_calculator()
    {
      a_->get_dict()->unregister_all_my_variables(this);
    }

    // Update the name of the classes.
    void update_previous_class()
    {
      auto it_bdd = used_var_.begin();

      // We run through the map bdd/list<state>, and we update
      // the previous_class_ with the new data.
      for (auto& p : sorted_classes_)
      {
        // If the signature of a state is bddfalse (no
        // edges) the class of this state is bddfalse
        // instead of an anonymous variable. It allows
        // simplifications in the signature by removing a
        // edge which has as a destination a state with
        // no outgoing edge.
        if (p->first == bddfalse)
          for (unsigned s : p->second)
            previous_class_[s] = bddfalse;
        else
          for (unsigned s : p->second)
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
      }
      update_previous_class();
    }

    // Take a state and compute its signature.
    bdd compute_sig(unsigned src)
    {
      bdd res = bddfalse;

      for (auto& t : a_->out(src))
      {
        // to_add is a conjunction of the acceptance condition,
        // the label of the edge and the class of the
        // destination and all the class it implies.
        bdd to_add = t.cond & relation_[previous_class_[t.dst]];

        res |= to_add;
      }
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
                                     std::forward_as_tuple(sig),
                                     std::forward_as_tuple(1, s));
        if (p.second)
          sorted_classes_.emplace_back(p.first);
        else
          p.first->second.emplace_back(s);
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

      auto it_bdd = used_var_.begin();

      for (auto& p : sorted_classes_)
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

      // This loop follows the pattern given by the paper.
      // foreach class do
      // |  foreach class do
      // |  | update po if needed
      // |  od
      // od

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

    // The list of states for each class at the current_iteration.
    // Computed in `update_sig'.
    map_bdd_lstate bdd_lstate_;

  protected:
    // The automaton which is reduced.
    twa_graph_ptr a_;

    // Implications between classes.
    map_bdd_bdd relation_;

    // Represent the class of each state at the previous iteration.
    vector_state_bdd previous_class_;

    // The above map, sorted by states number instead of BDD
    // identifier to avoid non-determinism while iterating over all
    // states.
    std::vector<map_bdd_lstate::const_iterator> sorted_classes_;

    // The queue of free bdd. They will be used as the identifier
    // for the class.
    std::queue<int> free_var_;

    // The list of used bdd. They are in used as identifier for class.
    std::deque<bdd> used_var_;

    // Size of the automaton.
    unsigned int size_a_;

    // Used to know when there is no evolution in the partial order.
    unsigned int po_size_;

    // Whether to compute implications between classes.  This is costly
    // and useless when we want to recognize the same language.
    bool want_implications_;
  };

  // An acyclic digraph such that there is an edge q1 -> q2 if
  // q1.label_ ⇒ q2.label_
  class bdd_digraph
  {
  private:
    bdd label_;
    unsigned state_;
    std::vector<std::shared_ptr<bdd_digraph>> children_;

  public:
    bdd_digraph() : label_(bddtrue), state_(-1U) {}

    bdd_digraph(bdd label, unsigned state) : label_(label), state_(state) {}

    void
    all_children_aux_(std::set<std::shared_ptr<bdd_digraph>>& res)
    {
      for (auto c : children_)
        if (res.insert(c).second)
          c->all_children_aux_(res);
    }

    std::set<std::shared_ptr<bdd_digraph>>
    all_children()
    {
      std::set<std::shared_ptr<bdd_digraph>> res;
      all_children_aux_(res);
      return res;
    }

    void
    add_aux_(std::shared_ptr<bdd_digraph>& new_node, std::vector<bool>& done)
    {
      // Avoid doing twice the same state
      if (state_ != -1U)
        done[state_] = true;
      for (auto& ch : children_)
      {
        if (done[ch->state_])
          continue;
        if (bdd_implies(new_node->label_, ch->label_))
          ch->add_aux_(new_node, done);
        else if (bdd_implies(ch->label_, new_node->label_))
        {
          auto ch_nodes = ch->all_children();
          new_node->children_.push_back(ch);
          for (auto& x : ch_nodes)
            new_node->children_.push_back(x);
        }
      }
      assert(bdd_implies(new_node->label_, label_));
      children_.push_back(new_node);
    }

    void
    add(std::shared_ptr<bdd_digraph>& new_node, bool rec,
              unsigned max_state)
    {
      if (new_node->label_ == bddtrue)
      {
        assert(label_ == bddtrue);
        state_ = new_node->state_;
        return;
      }
      if (rec)
      {
        std::vector<bool> done(max_state, false);
        add_aux_(new_node, done);
      }
      else
        children_.push_back(new_node);
    }

    unsigned
    flatten_aux(std::unordered_map<bdd, unsigned, spot::bdd_hash>& res)
    {
      if (children_.empty())
      {
        res.insert({label_, state_});
        return state_;
      }
      auto ch_size = children_.size();
      unsigned pos = ch_size - 1;
      auto my_repr = children_[pos]->flatten_aux(res);
      res.insert({label_, my_repr});
      for (unsigned i = 0; i < ch_size; ++i)
      {
        if (i == pos)
          continue;
        children_[i]->flatten_aux(res);
      }
      return my_repr;
    }

    std::unordered_map<bdd, unsigned, spot::bdd_hash>
    flatten()
    {
      std::unordered_map<bdd, unsigned, spot::bdd_hash> res;
      flatten_aux(res);
      return res;
    }

    // Transforms children_ such that the child with the higher use_count() is
    // at the end.
    void
    sort_nodes()
    {
      if (!children_.empty())
      {
        auto max_pos = std::max_element(children_.begin(), children_.end(),
                  [](const std::shared_ptr<bdd_digraph>& n1,
                     const std::shared_ptr<bdd_digraph>& n2)
                  {
                    return n1.use_count() < n2.use_count();
                  });
        std::iter_swap(max_pos, children_.end() - 1);
      }
    }
  };


  // Associate to a state a representative. The first value of the result
  // is -1U if ∀i repr[i] = i
  std::vector<unsigned>
  get_repres(twa_graph_ptr& a, bool rec)
  {
    const auto a_num_states = a->num_states();

    std::vector<unsigned> repr(a_num_states);
    bdd_digraph graph;
    std::vector<bdd> signatures(a_num_states);
    sig_calculator red(a, rec);
    red.main_loop();
    if (!rec && red.bdd_lstate_.size() == a_num_states)
    {
      repr[0] = -1U;
      return repr;
    }
    for (auto& [sig, states] : red.bdd_lstate_)
    {
      assert(!states.empty());
      bool in_tree = false;
      for (auto state : states)
      {
        signatures[state] = sig;
        // If it is not the first iteration, le BDD is already in the graph.
        if (!in_tree)
        {
          in_tree = true;
          auto new_node =
            std::make_shared<bdd_digraph>(bdd_digraph(sig, state));
          graph.add(new_node, rec, a_num_states);
        }
      }
    }
    graph.sort_nodes();
    auto repr_map = graph.flatten();

    bool is_useless_map = true;
    for (unsigned i = 0; i < a_num_states; ++i)
    {
      repr[i] = repr_map[signatures[i]];
      is_useless_map &= (repr[i] == i);
    }

    if (is_useless_map)
    {
      repr[0] = -1U;
      return repr;
    }
    return repr;
  }
}

namespace spot
{
  twa_graph_ptr reduce_mealy(const const_twa_graph_ptr& mm,
                             bool output_assignment)
  {
    bdd outputs = ensure_mealy("reduce_mealy", mm);
    if (mm->get_named_prop<std::vector<bool>>("state-player"))
      throw std::runtime_error("reduce_mealy(): "
                               "Only works on unsplit machines.\n");

    auto mmc = make_twa_graph(mm, twa::prop_set::all());
    mmc->copy_ap_of(mm);
    mmc->copy_acceptance_of(mm);
    set_synthesis_outputs(mmc, outputs);

    reduce_mealy_here(mmc, output_assignment);

    assert(is_mealy(mmc));
    return mmc;
  }

  void reduce_mealy_here(twa_graph_ptr& mm, bool output_assignment)
  {
    ensure_mealy("reduce_mealy_here", mm);

    // Only consider infinite runs
    mm->purge_dead_states();

    auto repr = get_repres(mm, output_assignment);
    if (repr[0] == -1U)
      return;

    // Change the destination of transitions using a DFT to avoid useless
    // modifications.
    auto init = repr[mm->get_init_state_number()];
    mm->set_init_state(init);
    std::stack<unsigned> todo;
    std::vector<bool> done(mm->num_states(), false);
    todo.emplace(init);
    while (!todo.empty())
      {
        auto current = todo.top();
        todo.pop();
        done[current] = true;
        for (auto& e : mm->out(current))
          {
            auto repr_dst = repr[e.dst];
            e.dst = repr_dst;
            if (!done[repr_dst])
              todo.emplace(repr_dst);
          }
      }
    mm->purge_unreachable_states();
    assert(is_mealy(mm));
  }
}

// Anonymous for mealy_min
namespace
{
  using namespace spot;

  // helper
#ifdef TRACE
  void trace_clause(const std::vector<int>& clause)
  {
    auto it = clause.begin();
    if (*it == 0)
      throw std::runtime_error("Trivially false clause");
    for (; it != clause.end(); ++it)
      {
        trace << *it << ' ';
        if (*it == 0)
          {
            trace << '\n';
            break;
          }
      }
    assert(it != clause.end() && "Clause must be zero terminated.");
  }
#else
  void trace_clause(const std::vector<int>&){}
#endif
  struct satprob_info
  {
    stopwatch sw;

    double premin_time, reorg_time, partsol_time, player_incomp_time,
           incomp_time, split_all_let_time, split_min_let_time,
           split_cstr_time, prob_init_build_time, sat_time,
           build_time, refine_time, total_time;
    long long n_classes, n_refinement, n_lit, n_clauses,
              n_iteration, n_letters_part, n_bisim_let, n_min_states, done;
    std::string task;
    const std::string instance;

    satprob_info(const std::string& instance)
      : premin_time{-1}
      , reorg_time{-1}
      , partsol_time{-1}
      , player_incomp_time{-1}
      , incomp_time{-1}
      , split_all_let_time{-1}
      , split_min_let_time{-1}
      , split_cstr_time{-1}
      , prob_init_build_time{-1}
      , sat_time{-1}
      , build_time{-1}
      , refine_time{-1}
      , total_time{-1}
      , n_classes{-1}
      , n_refinement{-1}
      , n_lit{-1}
      , n_clauses{-1}
      , n_iteration{-1}
      , n_letters_part{-1}
      , n_bisim_let{-1}
      , n_min_states{-1}
      , done{-1}
      , task{}
      , instance{instance+","}
    {
    }

    void start()
    {
      sw.start();
    }
    double stop()
    {
      return sw.stop();
    }
    double restart()
    {
      double res = sw.stop();
      sw.start();
      return res;
    }
    // Writing also "flushes"
    void write()
    {
      if (!sat_csv_file)
        return;
      auto f = [](std::ostream& o, auto& v, bool sep = true)
        {
          if (v >= 0)
            o << v;
          if (sep)
            o.put(',');
          v = -1;
        };

      auto& out = *sat_csv_file;
      if (out.tellp() == 0)
        {
          out << "instance,task,premin_time,reorg_time,partsol_time,"
              << "player_incomp_time,incomp_time,split_all_let_time,"
              << "split_min_let_time,split_cstr_time,prob_init_build_time,"
              << "sat_time,build_time,refine_time,total_time,n_classes,"
              << "n_refinement,n_lit,n_clauses,n_iteration,n_letters_part,"
              << "n_bisim_let,n_min_states,done\n";
        }

      assert(!task.empty());
      out << instance;
      out << task;
      task = "";
      out.put(',');

      std::stringstream ss;

      f(ss, premin_time);
      f(ss, reorg_time);
      f(ss, partsol_time);
      f(ss, player_incomp_time);
      f(ss, incomp_time);
      f(ss, split_all_let_time);
      f(ss, split_min_let_time);
      f(ss, split_cstr_time);
      f(ss, prob_init_build_time);
      f(ss, sat_time);
      f(ss, build_time);
      f(ss, refine_time);
      f(ss, total_time);
      f(ss, n_classes);
      f(ss, n_refinement);
      f(ss, n_lit);
      f(ss, n_clauses);
      f(ss, n_iteration);
      f(ss, n_letters_part);
      f(ss, n_bisim_let);
      f(ss, n_min_states);
      f(ss, done, false);
      out << ss.str();
      out.put('\n');
    }
  };


  template <class CONT>
  bool all_of(const CONT& c)
  {
    return std::all_of(c.begin(), c.end(), [](const auto& e){return e; });
  }
  template <class CONT, class FUN>
  bool all_of(const CONT& c, FUN fun)
  {
    return std::all_of(c.begin(), c.end(), fun);
  }

  template <class CONT>
  size_t find_first_index_of(const CONT& c)
  {
    size_t s = c.size();
    for (unsigned i = 0; i < s; ++i)
      if (c[i])
        return i;
    return s + 1;
  }
  template <class CONT, class PRED>
  size_t find_first_index_of(const CONT& c, PRED pred)
  {
    const size_t s = c.size();
    for (unsigned i = 0; i < s; ++i)
      if (pred(c[i]))
        return i;
    return s;
  }

  // key for multimap
  // Attention when working with signed integers: Then this will not be good
  // in general. It works well with literals (ints) as they are positive
  // in their normal form
  struct
  id_cont_hasher
  {
    template<class CONT>
    size_t operator()(const CONT& v) const
    {

      if constexpr (sizeof(typename CONT::value_type) <= sizeof(size_t)/2)
        {
          constexpr size_t shift_val1 =
              sizeof(typename CONT::value_type)*CHAR_BIT/2;
          constexpr size_t shift_val2 = (shift_val1*2)/3;

          size_t vs = v.size();
          switch (vs)
          {
            case 0:
              return 0;
            case 1:
              return (size_t) *v.begin();
            case 2:
            {
              auto it = v.begin();
              return (((size_t) *it)<<shift_val1) + (size_t) *(++it);
            }
            default:
            {
              size_t h = wang32_hash(vs);
              size_t hh;
              auto it = v.begin();
              const auto it_end = v.end();
              do
                {
                  hh = (size_t) *it;
                  hh <<= shift_val2;
                  ++it;
                  if (it != it_end)
                    {
                      hh += (size_t) *it;
                      hh <<= shift_val2;
                      ++it;
                      if (it != it_end)
                        {
                          hh += (size_t) *it;
                          ++it;
                        }
                    }
                  h ^= wang32_hash(hh);
                } while (it != it_end);
              return h;
            }
          }
        }
      else
        {
          //Implementation for less frequent type sizes is less efficient
          size_t h = wang32_hash(v.size());
          for (auto e : v)
            h ^= wang32_hash(e);
          return h;
        }
    }
  };


  // A class representing a square matrix
  template<class T, bool is_sym>
  class square_matrix: private std::vector<T>
  {
  private:
    size_t dim_;

  public:
    square_matrix()
        : std::vector<T>()
        , dim_(0)
    {}

    square_matrix(size_t dim)
        :  std::vector<T>(dim*dim)
        ,  dim_{dim}
    {}

    square_matrix(size_t dim, const T& t)
        :  std::vector<T>(dim*dim, t)
        ,  dim_{dim}
    {}

    using typename std::vector<T>::value_type;
    using typename std::vector<T>::size_type;
    using typename std::vector<T>::difference_type;
    using typename std::vector<T>::iterator;
    using typename std::vector<T>::const_iterator;

    inline size_t dim() const
    {
      return dim_;
    }
    // i: row number
    // j: column number
    // Stored in row major
    inline size_t idx_(size_t i, size_t j) const
    {
      return i * dim_ + j;
    }
    inline size_t idx(size_t i, size_t j) const
    {
#ifndef NDEBUG
      if (i >= dim_)
        throw std::runtime_error("i exceeds dim");
      if (j >= dim_)
        throw std::runtime_error("j exceeds dim");
#endif
      return idx_(i, j);
    }

    void set(size_t i, size_t j, const T& t)
    {
      (*this)[idx(i, j)] = t;
      if constexpr (is_sym)
        (*this)[idx(j, i)] = t;
    }
    T get(size_t i, size_t j) const
    {
      return (*this)[idx(i, j)];
    }

    std::pair<const_iterator, const_iterator> get_cline(size_t i) const
    {
      assert(i < dim_);
      return {cbegin() + idx(i, 0), cbegin() + idx_(i+1, 0)};
    }
    std::pair<iterator, iterator> get_line(size_t i)
    {
      assert(i < dim_);
      return {begin() + idx(i, 0), begin() + idx_(i+1, 0)};
    }

    const_iterator get_cline_begin(size_t i) const
    {
      assert(i < dim_);
      return cbegin() + idx(i, 0);
    }
    iterator get_line_begin(size_t i)
    {
      assert(i < dim_);
      return begin() + idx(i, 0);
    }
    const_iterator get_cline_end(size_t i) const
    {
      assert(i < dim_);
      return cbegin() + idx(i+1, 0);
    }
    iterator get_line_end(size_t i)
    {
      assert(i < dim_);
      return begin() + idx(i+1, 0);
    }
    using std::vector<T>::begin;
    using std::vector<T>::cbegin;
    using std::vector<T>::end;
    using std::vector<T>::cend;

    std::ostream& print(std::ostream& o) const
      {
        for (size_t i = 0; i < dim_; ++i)
          {
            for (size_t j = 0; j < dim_; ++j)
              o << (int) get(i, j) << ' ';
            o << std::endl;
          }
        return o;
      }
  };

  std::pair<const_twa_graph_ptr, unsigned>
  reorganize_mm(const_twa_graph_ptr mm, const std::vector<bool>& sp)
  {
    // Check if the twa_graph already has the correct form
    {
      auto sp = get_state_players(mm);
      // All player states mus be at the end
      bool is_ok = true;
      bool seen_player = false;
      for (const auto& p : sp)
        {
          if (seen_player & !p)
            {
              is_ok = false;
              break;
            }
          seen_player |= p;
        }
      if (is_ok)
        return {mm,
                mm->num_states()
                  - std::accumulate(sp.begin(), sp.end(), 0)};
    }
    // We actually need to generate a new graph with the correct
    // form
    // Purge unreachable and reorganize the graph
    std::vector<unsigned> renamed(mm->num_states(), -1u);
    const unsigned n_old = mm->num_states();
    unsigned next_env = 0;
    unsigned next_player = n_old;

    std::deque<unsigned> todo;
    todo.push_back(mm->get_init_state_number());
    renamed[todo.front()] = sp[todo.front()] ? (next_player++)
                                             : (next_env++);

    while (!todo.empty())
      {
        unsigned s = todo.front();
        todo.pop_front();

        for (const auto& e : mm->out(s))
          if (renamed[e.dst] == -1u)
            {
              renamed[e.dst] = sp[e.dst] ? (next_player++)
                                         : (next_env++);
              todo.push_back(e.dst);
            }
      }
    // Adjust player number
    const unsigned n_env = next_env;
    const unsigned diff = n_old - n_env;
    for (auto& s : renamed)
      s -= ((s >= n_old) && (s != -1u))*diff;
    const unsigned n_new
        = n_old - std::count(renamed.begin(), renamed.end(), -1u);

    auto omm = make_twa_graph(mm->get_dict());
    omm->copy_ap_of(mm);
    omm->new_states(n_new);

    for (const auto& e : mm->edges())
      {
        const unsigned n_src = renamed[e.src];
        const unsigned n_dst = renamed[e.dst];
        if (n_src != -1u && n_dst != -1u)
          omm->new_edge(n_src, n_dst, e.cond);
      }

    std::vector<bool> spnew(n_new, true);
    std::fill(spnew.begin(), spnew.begin()+n_env, false);
    set_state_players(omm, std::move(spnew));
    set_synthesis_outputs(omm, get_synthesis_outputs(mm));

    // Make sure we have a proper strategy,
    // that is each player state has only one successor
    assert([&]()
       {
         unsigned n_tot = omm->num_states();
         for (unsigned s = n_env; s < n_tot; ++s)
           {
             auto oute = omm->out(s);
             if ((++oute.begin()) != oute.end())
               return false;
           }
         return true;
       }() && "Player states have multiple edges.");

#ifdef TRACE
    trace << "State reorganize mapping\n";
    for (unsigned s = 0; s < renamed.size(); ++s)
      trace << s << " -> " << renamed[s] << '\n';
#endif
    return std::make_pair(omm, n_env);
  }

  square_matrix<bool, true>
  compute_incomp_impl_(const_twa_graph_ptr mm, const unsigned n_env,
                       satprob_info& si, bool is_partitioned)
  {
    const unsigned n_tot = mm->num_states();

    // Final result
    square_matrix<bool, true> inc_env(n_env, false);

    // Helper
    // Have two states already been checked for common pred
    square_matrix<bool, true> checked_pred(n_env, false);

    // Utility function
    auto get_cond = [&mm](unsigned s)->const bdd&
      {return mm->out(s).begin()->cond; };

    // Computing the incompatible player states

    // todo Tradeoff: lookup in the map is usually slower, but
    // if it is significantly smaller, it is still worth it?
    // We want the matrix for faster checks later on,
    // but it is beneficial to first compute the
    // compatibility between the conditions as there might be fewer
    std::unordered_map<std::pair<unsigned, unsigned>, bool, pair_hash>
        cond_comp;
    // Associated condition and id of each player state
    std::vector<std::pair<bdd, unsigned>> ps2c;
    ps2c.reserve(n_tot - n_env);
    // bdd id -> internal index
    std::unordered_map<unsigned, unsigned> all_out_cond;

    for (unsigned s1 = n_env; s1 < n_tot; ++s1)
      {
        const bdd &c1 = get_cond(s1);
        const unsigned c1id = (unsigned)c1.id();
        const auto& [it, inserted] =
            all_out_cond.try_emplace(c1id, all_out_cond.size());
        ps2c.emplace_back(c1, it->second);
#ifdef TRACE
        if (inserted)
          trace << "Register oc " << it->first << ", " << it->second
                << " for state " << s1 << '\n';
#else
        (void)inserted;
#endif
      }
    // Are two player condition ids states incompatible
    // Matrix for incompatibility
    square_matrix<bool, true> inc_player(all_out_cond.size(), false);
    // Matrix whether computed or not
    square_matrix<bool, true> inc_player_comp(all_out_cond.size(), false);
    // Compute. First is id of bdd
    // Lazy eval: Compute incompatibility between out conditions
    // only if demanded

    auto is_p_incomp = [&](unsigned s1, unsigned s2)
      {
        const auto& [s1bdd, s1idx] = ps2c[s1];
        const auto& [s2bdd, s2idx] = ps2c[s2];

        if (!inc_player_comp.get(s1idx, s2idx))
          {
            inc_player_comp.set(s1idx, s2idx, true);
            inc_player.set(s1idx, s2idx,
                           !bdd_have_common_assignment(s1bdd, s2bdd));
          }
        return inc_player.get(s1idx, s2idx);
      };

    si.player_incomp_time = si.restart();

#ifdef TRACE
    trace << "player cond id incomp\n";
    for (const auto& elem : all_out_cond)
      trace << elem.second << " - " << bdd_from_int((int) elem.first) << '\n';
    inc_player.print(std::cerr);
#endif
    // direct incomp: Two env states can reach incompatible player states
    // under the same input
    // The original graph mm is not sorted, and most of the
    // sorting is not rentable
    // However, bdd_have_common_assignment simply becomes equality
    auto direct_incomp = [&](unsigned s1, unsigned s2)
      {
        for (const auto& e1 : mm->out(s1))
          for (const auto& e2 : mm->out(s2))
            {
              if (is_partitioned && (e1.cond != e2.cond))
                continue;
              if (!is_p_incomp(e1.dst - n_env, e2.dst - n_env))
                continue; //Compatible -> no prob
              // Reachable under same letter?
              if (is_partitioned) // -> Yes
                {
                  trace << s1 << " and " << s2 << " directly incomp "
                        "due to successors " << e1.dst << " and " << e2.dst
                        << '\n';
                  return true;
                }
              else if (!is_partitioned
                       && bdd_have_common_assignment(e1.cond, e2.cond))
                {
                  trace << s1 << " and " << s2 << " directly incomp "
                        "due to successors " << e1.dst << " and " << e2.dst
                        << '\n';
                  return true;
                }
            }
        return false;
      };

    // If two states can reach an incompatible state
    // under the same input, then they are incompatible as well

    // Version if the input is not partitioned
    // We also need a transposed_graph
    twa_graph_ptr mm_t = nullptr;
    if (!is_partitioned)
    {
      mm_t = make_twa_graph(mm->get_dict());
      mm_t->copy_ap_of(mm);
      mm_t->new_states(n_env);

      for (unsigned s = 0; s < n_env; ++s)
        {
          for (const auto& e_env : mm->out(s))
            {
              unsigned dst_env = mm->out(e_env.dst).begin()->dst;
              mm_t->new_edge(dst_env, s, e_env.cond);
            }
        }
    }

    auto tag_predec_unpart = [&](unsigned s1, unsigned s2)
      {
        static std::vector<std::pair<unsigned, unsigned>> todo_;
        assert(todo_.empty());

        todo_.emplace_back(s1, s2);

        while (!todo_.empty())
          {
            auto [i, j] = todo_.back();
            todo_.pop_back();
            if (checked_pred.get(i, j))
              continue;
            // If predecs are already marked incomp
            for (const auto& ei : mm_t->out(i))
              for (const auto& ej : mm_t->out(j))
                {
                  if (inc_env.get(ei.dst, ej.dst))
                    // Have already been treated
                    continue;
                  // Now we need to actually check it
                  if (bdd_have_common_assignment(ei.cond, ej.cond))
                    {
                      trace << ei.dst << " and " << ej.dst << " tagged incomp"
                            " due to " << i << " and " << j << '\n';
                      inc_env.set(ei.dst, ej.dst, true);
                      todo_.emplace_back(ei.dst, ej.dst);
                    }
                }
            checked_pred.set(i, j, true);
          }
        // Done tagging all pred
      };

    // Version of taging taking advantaged of partitioned conditions
    struct S
    {
    };
    struct T
    {
      int id;
    };
    std::unique_ptr<digraph<S, T>> mm_t_part;
    if (is_partitioned)
      {
        mm_t_part = std::make_unique<digraph<S, T>>(n_env, mm->num_edges());
        mm_t_part->new_states(n_env);

        for (unsigned s = 0; s < n_env; ++s)
          {
            for (const auto& e_env : mm->out(s))
              {
                unsigned dst_env = mm->out(e_env.dst).begin()->dst;
                mm_t_part->new_edge(dst_env, s, e_env.cond.id());
              }
          }

        // Now we need to sort the edge to ensure that
        // the next algo works correctly
        mm_t_part->sort_edges_srcfirst_([](const auto& e1, const auto& e2)
                                          {return e1.id < e2.id; });
        mm_t_part->chain_edges_();
      }

    auto tag_predec_part = [&](unsigned s1, unsigned s2)
      {
        static std::vector<std::pair<unsigned, unsigned>> todo_;
        assert(todo_.empty());

        todo_.emplace_back(s1, s2);

        while (!todo_.empty())
          {
            auto [i, j] = todo_.back();
            todo_.pop_back();
            if (checked_pred.get(i, j))
              continue;
            // If predecs are already marked incomp
            auto e_it_i = mm_t_part->out(i);
            auto e_it_j = mm_t_part->out(j);

            auto e_it_i_e = e_it_i.end();
            auto e_it_j_e = e_it_j.end();

            auto e_i = e_it_i.begin();
            auto e_j = e_it_j.begin();

            // Joint iteration over both edge groups
            while ((e_i != e_it_i_e) && (e_j != e_it_j_e))
              {
                if (e_i->id < e_j->id)
                  ++e_i;
                else if (e_j->id < e_i->id)
                  ++e_j;
                else
                  {
                    assert(e_j->id == e_i->id);
                    trace << e_i->dst << " and " << e_j->dst << " tagged incomp"
                            " due to " << e_i->id << '\n';
                    inc_env.set(e_i->dst, e_j->dst, true);
                    todo_.emplace_back(e_i->dst, e_j->dst);
                    ++e_i;
                    ++e_j;
                  }
              }
            checked_pred.set(i, j, true);
          }
        // Done tagging all pred
      };

    for (unsigned s1 = 0; s1 < n_env; ++s1)
      for (unsigned s2 = s1 + 1; s2 < n_env; ++s2)
        {
          if (inc_env.get(s1, s2))
            continue; // Already done

          // Check if they are incompatible for some letter
          // We have to check all pairs of edges
          if (direct_incomp(s1, s2))
            {
              inc_env.set(s1, s2, true);
              if (is_partitioned)
                tag_predec_part(s1, s2);
              else
                tag_predec_unpart(s1, s2);

            }
        }

#ifdef TRACE
    trace << "Env state incomp\n";
    inc_env.print(std::cerr);
#endif
    si.incomp_time = si.restart();
    return inc_env;
  } // incomp no partition

  square_matrix<bool, true>
  compute_incomp(const_twa_graph_ptr mm, const unsigned n_env,
                 satprob_info& si, int max_letter_mult)
  {
    // Try to generate a graph with partitioned env transitions
    auto mm2 = make_twa_graph(mm, twa::prop_set::all());
    set_state_players(mm2, get_state_players(mm));
    set_synthesis_outputs(mm2, get_synthesis_outputs(mm));

    // todo get a good value for cutoff
    auto relabel_maps
        = partitioned_game_relabel_here(mm2, true, false, true,
                                        false, -1u, max_letter_mult);
    bool succ = !relabel_maps.env_map.empty();

    si.n_letters_part = relabel_maps.env_map.size();

#ifdef TRACE
    if (succ)
      std::cout << "Relabeling succesfull with " << relabel_maps.env_map.size()
                << " letters\n";
    else
      std::cout << "Relabeling aborted\n";
#endif

    return compute_incomp_impl_(succ ? const_twa_graph_ptr(mm2) : mm,
                                n_env, si, succ);
  }

  struct part_sol_t
  {
    std::vector<unsigned> psol;
    std::vector<unsigned> is_psol;
    std::vector<unsigned> incompvec;
  };

  // Partial solution: List of pairwise incompatible states.
  // Each of these states will be associated to a class.
  // It becomes the founding state of this class and has to belong to it
  part_sol_t get_part_sol(const square_matrix<bool, true>& incompmat)
  {
    // Use the "most" incompatible states as partial sol
    unsigned n_states = incompmat.dim();
    std::vector<std::pair<unsigned, unsigned>> incompvec(n_states);

    // square_matrix is row major!
    for (size_t ns = 0; ns < n_states; ++ns)
      {
        auto line_it = incompmat.get_cline(ns);
        incompvec[ns] = {ns,
                         std::count(line_it.first,
                                    line_it.second,
                                    true)};
      }

    // Sort in reverse order
    std::sort(incompvec.begin(), incompvec.end(),
              [](const auto& p1, const auto& p2)
                {return p1.second > p2.second; });

    part_sol_t part_sol;
    auto& psol = part_sol.psol;
    // Add states that are incompatible with ALL states in part_sol
    for (auto& p : incompvec)
      {
        auto ns = p.first;
        if (std::all_of(psol.begin(), psol.end(),
                        [&](auto npart)
                          {
                            return incompmat.get(ns, npart);
                          }))
          psol.push_back(ns);
      }
    // Note: this is important for look-up later on
    std::sort(psol.begin(), psol.end());
    part_sol.is_psol = std::vector<unsigned>(n_states, -1u);
    {
      unsigned counter = 0;
      for (auto s : psol)
        part_sol.is_psol[s] = counter++;
    }

    // Also store the states in their compatibility order
    part_sol.incompvec.resize(n_states);
    std::transform(incompvec.begin(), incompvec.end(),
                   part_sol.incompvec.begin(),
                   [](auto& p){return p.first; });
#ifdef TRACE
    std::cerr << "partsol\n";
    for (auto e : psol)
      std::cerr << e << ' ';
    std::cerr << "\nAssociated classes\n";
    for (unsigned e : part_sol.is_psol)
      std::cerr << (e == -1u ? -1 : (int) e) << ' ';
    std::cerr << '\n';
#endif
    return part_sol;
  }

  struct reduced_alphabet_t
  {
    unsigned n_groups;
    std::vector<unsigned> which_group;
    std::vector<std::vector<bdd>> universal_letters; //todo
    // minimal_letters:
    // Letters which can represent other letters
    // bdd: letter
    // associated set[0]: list of bdd ids which are implied by the letter
    //                    and that occur in the actual graph
    // associated set[1]: list of bdd ids corresponding to the covered letters
    //                    and which are represented by this one
    std::vector<
        std::unordered_map<
            bdd, std::pair<std::set<int>, std::set<int>>, bdd_hash>>
                minimal_letters;
    // In the sat problem, the minimal letters are simply enumerated
    // in the same order as the in vector below
    std::vector<std::vector<bdd>> minimal_letters_vec;

    // Bisimilar vector: a letter representing multiple
    // minimal letters
    // Store the indices fo bisimilar letters
    // First one is the representative
    std::vector<std::vector<std::vector<unsigned>>> bisim_letters;

    // all_letters:
    // bdd: letter
    // associated set: list of bdd ids which are implied by the letter
    //                 and that occur in the actual graph
    std::vector<std::vector<std::pair<bdd, std::set<int>>>> all_letters;

    // Indicator if two groups share the same alphabet
    // group uses the alphabet of share_sigma_with[group]
    // We make copies as the memory gained is small compared to the
    // code overhead
    std::vector<unsigned> share_sigma_with;
    std::vector<unsigned> share_bisim_with;
    // all_min_letters
    // Set of all minimal letters, ignoring their respective groups
    unsigned n_red_sigma;
  };

  // Computes the "transitive closure of compatibility"
  // Only states that are transitively compatible need to
  // agree on the letters
  std::pair<unsigned, std::vector<unsigned>>
  trans_comp_classes(const square_matrix<bool, true>& incompmat)
  {
    const unsigned n_env = incompmat.dim();

    std::vector<unsigned> which_group(n_env, -1u);

    std::vector<unsigned> stack_;

    unsigned n_group = 0;
    for (unsigned s = 0; s < n_env; ++s)
      {
        if (which_group[s] != -1u)
          continue;

        which_group[s] = n_group;

        stack_.emplace_back(s);

        while (!stack_.empty())
          {
            unsigned sc = stack_.back();
            stack_.pop_back();

            for (unsigned scp = s + 1; scp < n_env; ++scp)
              {
                if (which_group[scp] != -1u || incompmat.get(sc, scp))
                  continue;
                which_group[scp] = n_group;
                stack_.push_back(scp);
              }
          }
        ++n_group;
      }
#ifdef TRACE
      trace << "We found " << n_group << " groups.\n";
      for (unsigned s = 0; s < n_env; ++s)
        trace << s << " : " << which_group.at(s) << '\n';
#endif
    return std::make_pair(n_group, which_group);
  }

  // Helper function
  // Computes the set of all original letters implied by the leaves
  // This avoids transposing the graph


  // Computes the letters of each group
  // Letters here means bdds such that for all valid
  // assignments of the bdd we go to the same dst from the same source
  void compute_all_letters(reduced_alphabet_t& red,
                           const_twa_graph_ptr& mmw,
                           const unsigned n_env)
  {
    //To avoid recalc
    std::set<int> all_bdd;
    std::vector<bdd> all_bdd_v;
    std::unordered_map<unsigned, unsigned> node2idx;

    std::unordered_multimap<size_t, std::pair<unsigned, std::set<int>>>
        sigma_map;

    const unsigned n_groups = red.n_groups;
    for (unsigned groupidx = 0; groupidx < n_groups; ++groupidx)
      {
        all_bdd.clear();
        // List all bdds occuring in this group, no matter the order
        for (unsigned s = 0; s < n_env; ++s)
          {
            if (red.which_group[s] != groupidx)
              continue;
            for (const auto& e : mmw->out(s))
              all_bdd.insert(e.cond.id());
          }
        // Check if we have already decomposed them
        const size_t sigma_hash = id_cont_hasher()(all_bdd);
        {
          auto eq_range = sigma_map.equal_range(sigma_hash);
          bool treated = false;
          for (auto it = eq_range.first; it != eq_range.second; ++it)
            {
              if (all_bdd == it->second.second)
                {
                  red.all_letters.
                    emplace_back(red.all_letters[it->second.first]);
                  red.share_sigma_with.push_back(it->second.first);
                  trace << "Group " << groupidx << " shares an alphabet with "
                        << it->second.first << '\n';
                  treated = true;
                  break;
                }
            }
          if (treated)
            continue;
          else
            {
              // Store bdds as vector for compatibility
              all_bdd_v.clear(); // Note: sorted automatically by id
              std::transform(all_bdd.begin(), all_bdd.end(),
                            std::back_inserter(all_bdd_v),
                            [](int i){return bdd_from_int(i); });
              // Insert it already into the sigma_map
              trace << "Group " << groupidx << " generates a new alphabet\n";
              sigma_map.emplace(std::piecewise_construct,
                                std::forward_as_tuple(sigma_hash),
                                std::forward_as_tuple(groupidx,
                                                      std::move(all_bdd)));
            }
        }

        // Result
        red.share_sigma_with.push_back(groupidx);
        red.all_letters.emplace_back();
        auto& group_letters = red.all_letters.back();

        // Compute it
        auto this_part = try_partition_me(all_bdd_v, -1u);
        assert(this_part.relabel_succ);

        // Transform it
        // group_letters is pair<new_letter, set of implied orig letters as id>
        // There are as many new_letters as treated bdds in the partition
        group_letters.clear();
        group_letters.reserve(this_part.treated.size());
        node2idx.clear();
        node2idx.reserve(this_part.treated.size());

        for (const auto& [label, node] : this_part.treated)
          {
            node2idx[node] = group_letters.size();
            group_letters.emplace_back(std::piecewise_construct,
                                       std::forward_as_tuple(label),
                                       std::forward_as_tuple());
          }

        // Go through the graph for each original letter
        auto search_leaves
            = [&ig = *this_part.ig, &group_letters, &node2idx]
                (int orig_letter_id, unsigned s, auto&& search_leaves_) -> void
          {
            if (ig.state_storage(s).succ == 0)
              {
                // Leaf
                unsigned idx = node2idx[s];
                auto& setidx = group_letters[idx].second;
                setidx.emplace_hint(setidx.end(), orig_letter_id);
              }
            else
              {
                // Traverse
                for (const auto& e : ig.out(s))
                  search_leaves_(orig_letter_id, e.dst, search_leaves_);
              }
          };

        const unsigned Norig = all_bdd_v.size();
        for (unsigned s = 0; s < Norig; ++s)
          search_leaves(all_bdd_v[s].id(), s, search_leaves);

        // Verify that all letters imply at least one original letter
        assert(std::all_of(group_letters.begin(), group_letters.end(),
                           [](const auto& l){return !l.second.empty(); }));


#ifdef TRACE
        trace << "this group letters" << std::endl;
        auto sp = [&](const auto& c)
            {std::for_each(c.begin(), c.end(),
                           [&](auto& e){trace << e << ' '; }); };
        for (const auto& p : group_letters)
          {
            trace << p.first << " - ";
            sp(p.second);
            trace << std::endl;
          }
#endif
      }
  }

  // compute bisimilar minimal letters
  // We say two letters are bisimilar if they have the same destination
  // for all src states
  void compute_bisim_letter(reduced_alphabet_t& red,
                            const_twa_graph_ptr& mmw,
                            const unsigned n_env,
                            const unsigned i)

  {
    // Do not use -1u to mark no succ, as this is "bad" for the
    // hashing function -> Use first unused state
    const unsigned no_succ_mark = mmw->num_states();
    const auto& mlv = red.minimal_letters_vec.at(i);
    const unsigned n_ml = mlv.size();
    const unsigned nsg = std::count(red.which_group.begin(),
                                    red.which_group.end(), i);
    assert(nsg != 0 && nsg <= n_env);

    std::vector<unsigned> dest_vec(nsg);

    // hashed id -> <dest vector, list of sim indices vec>
    std::unordered_multimap<size_t,
        std::pair<std::vector<unsigned>, std::vector<unsigned>>> sim_map;

    auto get_e_dst = [&](const auto& e_env)->unsigned
      {
        return mmw->out(e_env.dst).begin()->dst;
      };

    for (unsigned idx = 0; idx < n_ml; ++idx)
      {
        const bdd& ml = mlv[idx];
        const std::set<int>& ml_impl = red.minimal_letters[i][ml].first;

        dest_vec.clear();
        for (unsigned s = 0; s < n_env; ++s)
          {
            if (red.which_group[s] != i)
              continue;
            // Check which outgoing edge is implied by ml
            // Note there can be only one due to input determinism
            // Note if there is no such edge we mark it
            unsigned this_dst = no_succ_mark;
            for (const auto& e : mmw->out(s))
              if (ml_impl.count(e.cond.id()))
                {
                  this_dst = get_e_dst(e);
                  break;
                }
            dest_vec.push_back(this_dst);
          }
        assert(dest_vec.size() == nsg);

        // We constructed the dst vector, check if it already exists
        size_t id = id_cont_hasher()(dest_vec);
        auto [eq, eq_end] = sim_map.equal_range(id);
        bool is_sim = false;
        for (; eq != eq_end; ++eq)
          if (dest_vec == eq->second.first)
            {
              eq->second.second.push_back(idx);
              is_sim = true;
              break;
            }
        if (!is_sim)
          sim_map.emplace(id,
                          std::make_pair(dest_vec,
                                         std::vector<unsigned>{idx}));
      }
    // save results
    red.bisim_letters.emplace_back();
    auto& bs = red.bisim_letters.back();
    // We need to sort the results because the
    // construction of the sat-problem latter on depends on it
    for (auto&& [id, pv] : sim_map)
      {
        (void)id;
        // We want front (the representative) to be the smallest
        std::sort(pv.second.begin(), pv.second.end());
        bs.emplace_back(std::move(pv.second));
      }
    // Sort the bisimilar classes as well for the same reason
    std::sort(bs.begin(), bs.end(),
              [](const auto& v1, const auto& v2)
                {return v1.front() < v2.front(); });
    //Done
  }

  // If two letters take the same original edge / go to the same destination
  // for all states, then one can represent the other
  // Here we search a minimal subset of letters that can represent
  // all the others
  void compute_minimal_letters(reduced_alphabet_t& red,
                               const_twa_graph_ptr& mmw,
                               const unsigned n_env)
  {
    // mmw is deterministic with respect to inputs
    // So if two letters imply the same conditions
    // they take the same edges and can therefore be represented
    // by just one of them

    std::unordered_multimap<size_t, bdd> tgt_map;

    const unsigned n_groups = red.n_groups;
    red.minimal_letters.clear();
    red.minimal_letters.reserve(n_groups);
    red.n_red_sigma = 0;

    for (unsigned i = 0; i < n_groups; ++i)
      {
        // If a group shares an alphabet with another group,
        // then they also share the minimal letters
        // Again, copied to avoid overhead
        if (red.share_sigma_with[i] != i)
          {
            assert(red.share_sigma_with[i] < i);
            red.minimal_letters
                .push_back(red.minimal_letters[red.share_sigma_with[i]]);
            red.minimal_letters_vec
                .push_back(red.minimal_letters_vec[red.share_sigma_with[i]]);
            continue;
          }

        tgt_map.clear();

        red.minimal_letters.emplace_back();
        auto& group_min_letters = red.minimal_letters.back();

        // Check all letters
        for (const auto& [letter, impl_cond] : red.all_letters[i])
          {
            // Check if this set exists
            size_t hv = id_cont_hasher()(impl_cond);
            auto eq_r = tgt_map.equal_range(hv);
            bool covered = false;
            for (auto min_letter_it = eq_r.first; min_letter_it != eq_r.second;
                 ++min_letter_it)
              {
                auto& min_letter_struct =
                  group_min_letters.at(min_letter_it->second);
                // Check if truly compatible
                if (impl_cond == min_letter_struct.first)
                  {
                    // letter can be represented by min_letter
                    min_letter_struct.second.insert(letter.id());
                    covered = true;
                    break;
                  }
              }
            if (!covered)
              {
                // We have found a new minimal letter
                // Update tgt_map and minimal_letters
                tgt_map.emplace(hv, letter);
                group_min_letters.emplace(letter,
                                          std::make_pair(impl_cond,
                                             std::set<int>{letter.id()}));
              }
          }
        red.minimal_letters_vec.emplace_back();
        auto& gmlv = red.minimal_letters_vec.back();
        gmlv.reserve(red.minimal_letters.back().size());
        const auto& mlg = red.minimal_letters.back();
        trace << "Group min letters\n";
        for (const auto& mlit : mlg)
          {
            trace << mlit.first << '\n';
            gmlv.push_back(mlit.first);
          }
        // Sort it
        // todo: stable sort?
        std::sort(gmlv.begin(), gmlv.end(),
                  [](const bdd& l, const bdd& r)
                  {return l.id() < r.id(); });
      }


    for (unsigned i = 0; i < n_groups; i++)
      {
        // Compute the bisimilar minimal letters
        compute_bisim_letter(red, mmw, n_env, i);

        red.n_red_sigma = std::max(red.n_red_sigma,
                                   (unsigned) red.bisim_letters.back().size());
      }

    // Save if groups share not only the alphabet,
    // but also which letters are bisimilar
    red.share_bisim_with = std::vector<unsigned>(n_groups, -1u);
    for (unsigned i = 0; i < n_groups; i++)
      {
        if (red.share_sigma_with[i] == i)
          red.share_bisim_with[i] = i; // Its own class
        for (unsigned j = 0; j < i; ++j)
          if (red.share_sigma_with[j] == red.share_sigma_with[i]
              && red.bisim_letters[j] == red.bisim_letters[i])
            {
              red.share_bisim_with[i] = j;
              break;
            }
        if (red.share_bisim_with[i] == -1u)
          red.share_bisim_with[i] = i;
      }
    trace << "All min letters " << red.n_red_sigma << '\n';
  }

  // We construct a new graph with edges labeled by the minimal letters
  // and only holding the env states
  twa_graph_ptr split_on_minimal(const reduced_alphabet_t& red,
                                 const_twa_graph_ptr mmw,
                                 const unsigned n_env)
  {
    const unsigned n_groups = red.n_groups;
    auto split_mmw = make_twa_graph(mmw->get_dict());
    split_mmw->copy_ap_of(mmw);
    split_mmw->new_states(n_env);

    // We only need env states
    auto get_e_dst = [&](const auto& e_env)
      {
        return mmw->out(e_env.dst).begin()->dst;
      };

    // We only need the transitions implied
    // by minimal and representative letters
    // Build a map from bdd-ids in the graph
    // to the set of implied minimal letters
    // Note we can do this group by group
    std::vector<std::unordered_map<int, std::set<int>>>
        l_map_glob(red.n_groups,
                   std::unordered_map<int, std::set<int>>{});

    // todo Check if this is bottleneck
    // Note: if two groups share the alphabet AND the
    //       which letters are bisimilar -> They also share the map
    for (unsigned i = 0; i < n_groups; ++i)
      {
        auto& l_map = l_map_glob.at(red.share_bisim_with[i]);
        if (l_map.empty())
          {
            assert(red.share_bisim_with[i] == i);
            const auto& bisim_idx_vec = red.bisim_letters[i];
            for (const auto& a_bisim : bisim_idx_vec)
              {
                assert(a_bisim.front() < red.minimal_letters_vec[i].size());
                const bdd& repr_bdd =
                  red.minimal_letters_vec[i].at(a_bisim.front());
                const auto& it_mlb =
                    red.minimal_letters[i].at(repr_bdd);
                const int this_id = repr_bdd.id();
                for (int implied_by : it_mlb.first)
                  l_map[implied_by].insert(this_id);
              }
          }
        else
          assert(red.share_bisim_with[i] < i);

        const auto lmap_end = l_map.end();
        for (unsigned s = 0; s < n_env; ++s)
          {
            if (red.which_group[s] != i)
              continue;
            for (const auto &e : mmw->out(s))
              {
                // Edge might be simulated by another
                auto it_e = l_map.find(e.cond.id());
                if (it_e != lmap_end)
                  {
                    const auto& ml_l = it_e->second;
                    unsigned dst_e = get_e_dst(e);
                    for (int bdd_id : ml_l)
                      split_mmw->new_edge(s, dst_e, bdd_from_int(bdd_id));
                  }
                else
                  trace << e.src << " - " << e.cond.id() << " > " << e.dst
                        << " is simulated\n";
              }
          }
      }

    // todo sort edges inplace? bdd_less_than vs bdd_less_than_stable
    split_mmw->
        get_graph().sort_edges_([](const auto& e1, const auto& e2)
                                  {
                                    return std::make_pair(e1.src,
                                                          e1.cond.id())
                                           < std::make_pair(e2.src,
                                                            e2.cond.id());
                                  });
    split_mmw->get_graph().chain_edges_();
#ifdef TRACE
    trace << "Orig split aut\n";
    print_hoa(std::cerr, mmw) << '\n';
    {
      auto ss = make_twa_graph(split_mmw, twa::prop_set::all());
      for (unsigned group = 0; group < red.n_groups; ++group)
        {
          std::vector<bdd> edge_num;
          for (unsigned i = 0;  i < red.minimal_letters_vec[group].size(); ++i)
            {
              edge_num.push_back(
                bdd_ithvar(ss->register_ap("g"+std::to_string(group)
                                           +"e"+std::to_string(i))));
            }
          for (unsigned s = 0; s < n_env; ++s)
            {
              if (red.which_group.at(s) != group)
                continue;
              for (auto& e : ss->out(s))
                e.cond =
                    edge_num.at(
                        find_first_index_of(red.bisim_letters[group],
                            [&, cc = e.cond](const auto& bs_idx_vec)
                              {return cc
                                  == red.minimal_letters_vec[group]
                                       [bs_idx_vec.front()]; }));
            }
        }
      trace << "Relabeled split aut\n";
      print_hoa(std::cerr, ss) << '\n';
      trace << "Bisim ids\n";
      for (unsigned i = 0; i < n_groups; ++i)
        {
          trace << "group " << i << '\n';
          for (const auto& idx_vec : red.bisim_letters[i])
            trace << red.minimal_letters_vec[i][idx_vec.front()].id() << '\n';
          trace << "ids in the edge of the group\n";
          for (unsigned s = 0; s < n_env; ++s)
            if (red.which_group.at(s) == i)
              for (const auto& e : split_mmw->out(s))
                trace << e.src << "->" << e.dst << ':' << e.cond.id() << '\n';
        }
    }
#endif
    return split_mmw;
  }

  std::pair<twa_graph_ptr, reduced_alphabet_t>
  reduce_and_split(const_twa_graph_ptr mmw, const unsigned n_env,
                   const square_matrix<bool, true>& incompmat,
                   satprob_info& si)
  {
    reduced_alphabet_t red;
    si.start();

    std::tie(red.n_groups, red.which_group) = trans_comp_classes(incompmat);

    compute_all_letters(red, mmw, n_env);
    si.split_all_let_time = si.restart();

    compute_minimal_letters(red, mmw, n_env);
    si.split_min_let_time = si.restart();
    si.n_bisim_let = red.n_red_sigma;

    twa_graph_ptr split_mmw = split_on_minimal(red, mmw, n_env);
    si.split_cstr_time = si.restart();
    trace << std::endl;

    return std::make_pair(split_mmw, red);
  }

  // Things for lit mapping
  // mapping (states, classes)
  struct xi_t : public std::pair<unsigned, unsigned>
  {
    unsigned& x;
    unsigned& i;

    constexpr xi_t(unsigned x_in, unsigned i_in)
      :  std::pair<unsigned, unsigned>{x_in, i_in}
      , x{this->first}
      , i{this->second}
    {
    }

    constexpr xi_t(const xi_t& xi)
      : xi_t{xi.x, xi.i}
    {
    }

    xi_t& operator=(const xi_t& xi)
    {
      x = xi.x;
      i = xi.i;
      return *this;
    }

    xi_t(xi_t&& xi)
      : xi_t{xi.x, xi.i}
    {
    }
  };

  // mapping (classes, letters, classes)
  struct iaj_t
  {
    size_t hash() const noexcept
    {
      size_t h = i;
      h <<= 21;
      h += a;
      h <<= 20;
      h += j;
      return std::hash<size_t>()(h);
    }
    bool operator==(const iaj_t& rhs) const
    {
      return std::tie(i, a, j) == std::tie(rhs.i, rhs.a, rhs.j);
    }
    bool operator<(const iaj_t& rhs) const
    {
      return std::tie(i, a, j) < std::tie(rhs.i, rhs.a, rhs.j);
    }

    unsigned i, a, j;
  };

  auto iaj_hash =
      [](const iaj_t& iaj) noexcept {return iaj.hash(); };
  auto iaj_eq =
      [](const iaj_t& l, const iaj_t& r){return l == r; };
  auto iaj_less = [](const iaj_t& l, const iaj_t& r){return l < r; };

  template<bool USE_PICO>
  struct lit_mapper;

  template<>
  struct lit_mapper<true>
  {
    // x and y in same class?
    //x <-> x, i <-> y
    using xy_t = xi_t;
    // using k-th product of out-cond of state x for minimal letter u
    // u <-> i, x <-> a, k <-> k
    using uxk_t = iaj_t;

    PicoSAT* psat_;
    unsigned n_classes_;
    const unsigned n_env_, n_sigma_red_;
    int next_var_;
    bool frozen_xi_, frozen_iaj_, frozen_si_;
    //xi -> lit
    std::unordered_map<xi_t, int, pair_hash> sxi_map_;
    //xy -> lit
    std::unordered_map<xi_t, int, pair_hash> ixy_map_;
    //iaj -> lit
    std::unordered_map<iaj_t, int,
                       decltype(iaj_hash),
                       decltype(iaj_eq)> ziaj_map_{1, iaj_hash, iaj_eq};
    //iaj -> lit
    std::unordered_map<uxk_t, int,
                       decltype(iaj_hash),
                       decltype(iaj_eq)> cuxk_map_{1, iaj_hash, iaj_eq};
    // all lits
    std::vector<int> all_lits;

    lit_mapper(unsigned n_classes, unsigned n_env,
               unsigned n_sigma_red)
      : psat_{picosat_init()}
      , n_classes_{n_classes}
      , n_env_{n_env}
      , n_sigma_red_{n_sigma_red}
      , next_var_{std::numeric_limits<int>::min()}
      , frozen_xi_{false}
      , frozen_iaj_{false}
    {
      next_var_ = get_var_();
      // There are at most n_classes*n_env literals in the sxi_map
      // Usually they all appear
      sxi_map_.reserve(n_classes_*n_env_);
      // There are at most n_classes*n_classes*n_sigma_red in ziaj_map
      // However they are usually more scarce
      ziaj_map_.reserve((n_classes_*n_classes_*n_sigma_red_)/3);
    }

    ~lit_mapper()
    {
      picosat_reset(psat_);
    }

    int get_var_()
    {
      return picosat_inc_max_var(psat_);
    }

    void inc_var()
    {
      all_lits.push_back(next_var_);
      next_var_ = get_var_();
    }

    int sxi2lit(xi_t xi)
    {
      assert(xi.x < n_env_ && "Exceeds max state number.");
      assert(xi.i < n_classes_ && "Exceeds max source class.");
      auto [it, inserted] = sxi_map_.try_emplace(xi, next_var_);
      if (inserted)
        inc_var();
      assert((!frozen_xi_ || !inserted) && "Created lit when frozen.");
      return it->second;
    }

    int sxi2lit(xi_t xi) const
    {
      assert(sxi_map_.count(xi) && "Cannot create lit when const.");
      return sxi_map_.at(xi);
    }

    int get_sxi(xi_t xi) const // Gets the literal or zero of not defined
    {
      auto it = sxi_map_.find(xi);
      if (it == sxi_map_.end())
        return 0;
      else
        return it->second;
    }

    void freeze_xi()
    {
      frozen_xi_ = true;
    }
    void unfreeze_xi()
    {
      frozen_xi_ = false;
    }

    int ziaj2lit(iaj_t iaj)
    {
      assert(iaj.i < n_classes_ && "Exceeds source class.");
      assert(iaj.a < n_sigma_red_ && "Exceeds max letter idx.");
      assert(iaj.j < n_classes_&& "Exceeds dest class.");
      auto [it, inserted] = ziaj_map_.try_emplace(iaj, next_var_);
      assert((!frozen_iaj_ || !inserted) && "Created lit when frozen.");
      if (inserted)
        inc_var();
      return it->second;
    }

    int ziaj2lit(iaj_t iaj) const
    {
      assert(ziaj_map_.count(iaj) && "Cannot create lit when const.");
      return ziaj_map_.at(iaj);
    }
    int get_iaj(iaj_t iai) const // Gets the literal or zero of not defined
    {
      auto it = ziaj_map_.find(iai);
      if (it == ziaj_map_.end())
        return 0;
      else
        return it->second;
    }

    void freeze_iaj()
    {
      frozen_iaj_ = true;
    }
    void unfreeze_iaj()
    {
      frozen_iaj_ = false;
    }

    int ixy2lit(xy_t xy)
    {
      assert(xy.x < n_env_ && "Exceeds max state number.");
      assert(xy.i < n_env_ && "Exceeds max state number.");
      auto [it, inserted] = ixy_map_.try_emplace(xy, next_var_);
      if (inserted)
        inc_var();
      return it->second;
    }

    int ixy2lit(xy_t xy) const
    {
      return ixy_map_.at(xy);
    }

    int cuxk2lit(uxk_t uxk)
    {
      assert(uxk.a < n_env_ && "Exceeds max state number.");
      auto [it, inserted] = cuxk_map_.try_emplace(uxk, next_var_);
      if (inserted)
        inc_var();
      return it->second;
    }

    int cuxk2lit(uxk_t uxk) const
    {
      return cuxk_map_.at(uxk);
    }

    std::ostream& print(std::ostream& os = std::cout,
                        std::vector<int>* sol = nullptr)
    {
      bool hs = sol != nullptr;
      auto ts = [&](int i){return std::to_string(i); };

      {
        std::map<xi_t, int> xi_tmp(sxi_map_.begin(),
                                   sxi_map_.end());
        os << "x - i -> lit" << (hs ? " - sol\n" : "\n");
        for (auto& it : xi_tmp)
          os << it.first.x << " - " << it.first.i << " -> " << it.second
             << (hs ? " - " + ts(sol->at(sxi_map_.at(it.first))) : " ")
             << '\n';
      }
      {
        std::map<iaj_t, int, decltype(iaj_less)>
            iaj_tmp(ziaj_map_.begin(), ziaj_map_.end(), iaj_less);
        os << "i - a - j -> lit\n";
        for (auto& it : iaj_tmp)
            os << it.first.i << " - " << it.first.a << " - " << it.first.j
             << " -> " << it.second
             << (hs ? " - " + ts(sol->at(ziaj_map_.at(it.first))) : " ")
             << '\n';
      }
      {
        std::map<xy_t, int> xy_tmp(ixy_map_.begin(),
                                   ixy_map_.end());
        os << "x - y -> lit" << (hs ? " - sol\n" : "\n");
        for (auto& it : xy_tmp)
          os << it.first.x << " - " << it.first.i << " -> " << it.second
             << (hs ? " - " + ts(sol->at(ixy_map_.at(it.first))) : " ")
             << '\n';
      }
      {
        std::map<uxk_t, int, decltype(iaj_less)>
            uxk_tmp(cuxk_map_.begin(), cuxk_map_.end(), iaj_less);
        os << "u - x - k -> lit\n";
        for (auto& it : uxk_tmp)
            os << it.first.i << " - " << it.first.a << " - " << it.first.j
             << " -> " << it.second
             << (hs ? " - " + ts(sol->at(cuxk_map_.at(it.first))) : " ")
             << '\n';
      }
      return os;
    }
  };

  using ia_t = xi_t;


  using pso_pair = std::pair<unsigned, bdd>;
  struct pso_order
  {
    bool operator()(const pso_pair& p1,
                    const pso_pair& p2) const noexcept
    {
      return p1.first < p2.first;
    }
  };

  template<bool USE_PICO>
  struct mm_sat_prob_t;

  template<>
  struct mm_sat_prob_t<true>
  {
    mm_sat_prob_t(unsigned n_classes, unsigned n_env,
                  unsigned n_sigma_red, satprob_info& si)
      : lm(n_classes, n_env, n_sigma_red)
      , n_classes{lm.n_classes_}
      , si{si}
    {
      state_cover_clauses.reserve(n_classes);
      trans_cover_clauses.reserve(n_classes*n_sigma_red);
    }

    void add_static(int lit)
    {
      picosat_add(lm.psat_, lit);
    }
    template<class CONT>
    void add_static(CONT& lit_cont)
    {
      for (int lit : lit_cont)
        picosat_add(lm.psat_, lit);
    }


    void set_variable_clauses()
    {
      trace << "c Number of local clauses "
            << state_cover_clauses.size() + trans_cover_clauses.size() << '\n';
      trace << "c Cover clauses\n";
      picosat_push(lm.psat_);
      for (auto& [_, clause] : state_cover_clauses)
        {
          (void)_;
          // Clause is not nullterminated!
          clause.push_back(0);
          picosat_add_lits(lm.psat_, clause.data());
          trace_clause(clause);
          clause.pop_back();
        }
      trace << "c Transition cover clauses\n";
      for (auto& elem : trans_cover_clauses)
        {
          // Clause is not nullterminated!
          auto& clause = elem.second;
          clause.push_back(0);
          picosat_add_lits(lm.psat_, clause.data());
          trace_clause(clause);
          clause.pop_back();
        }
    }

    std::vector<int>
    get_sol()
    {
      // Returns a vector of assignments
      // The vector is empty iff the prob is unsat
      // res[i] == -1 : i not used in lit mapper
      // res[i] == 0 : i is assigned false
      // res[i] == 1 : i is assigned true
      if (sat_dimacs_file)
        {
          fprintf(sat_dimacs_file->f,
                  "c ### Next Instance %lld %lld ###\n",
                  this->si.n_classes, this->si.n_refinement);
          picosat_print(lm.psat_, sat_dimacs_file->f);
        }
      switch (picosat_sat(lm.psat_, -1))
      {
        case PICOSAT_UNSATISFIABLE:
          return {};
        case PICOSAT_SATISFIABLE:
          {
            std::vector<int>
                res(1 + (unsigned) picosat_variables(lm.psat_), -1);
            SPOT_ASSUME(res.data()); // g++ 11 believes data might be nullptr
            res[0] = 0; // Convention
            for (int lit : lm.all_lits)
              res[lit] = picosat_deref(lm.psat_, lit);
#ifdef TRACE
            trace << "Sol is\n";
            for (unsigned i = 0; i < res.size(); ++i)
              trace << i << ": " << res[i] << '\n';
#endif
            return res;
          }
      default:
          throw std::runtime_error("Unknown error in picosat.");
      }
    }

    void unset_variable_clauses()
    {
      picosat_pop(lm.psat_);
    }

    unsigned n_lits() const
    {
      return lm.next_var_ - 1;
    }

    unsigned n_clauses() const
    {
      return (unsigned) picosat_added_original_clauses(lm.psat_);
    }

    // The mapper
    lit_mapper<true> lm;

    // The current number of classes
    unsigned& n_classes;

    // partial solution, incompatibility
    // Clauses have to be added, but existing ones
    // remain unchanged

    // Add the new lit each time n_classes is increased
    std::vector<std::pair<unsigned, std::vector<int>>> state_cover_clauses;
    std::unordered_map<ia_t, std::vector<int>, pair_hash> trans_cover_clauses;

    // A vector of maps group -> minimal letter -> set of pairs(state, ocond)
    // The set is only ordered with respect to the state
    using pso_set = std::set<pso_pair, pso_order>;
    std::vector<std::vector<pso_set>> ocond_maps;
    // A matrix tracking if two states
    // are already "tracked" for extended incompatibility
    square_matrix<bool, true> tracked_s_pair;
    // A map relating a bdd to a list of its cubes using its
    // id as key
    std::unordered_map<int, std::vector<bdd>> cube_map;
    // A map that indicates if two cubes are compatible or not via their id
    std::unordered_map<std::pair<int, int>, bool, pair_hash> cube_incomp_map;
    // Piggy-back a struct for performance measure
    satprob_info& si;
  };

  template<>
  void mm_sat_prob_t<true>::add_static(std::vector<int>& lit_cont)
  {
    picosat_add_lits(lm.psat_, lit_cont.data());
  }

  template <bool USE_PICO>
  void add_trans_cstr_f(mm_sat_prob_t<USE_PICO>&,
                        const square_matrix<bool, true>&,
                        const iaj_t, const unsigned, const int,
                        const unsigned,
                        const std::vector<unsigned>&,
                        const std::vector<unsigned>&);

  // Add the constraints on transitions if the src class and possibly
  // the dst class is a partial solution
  template <>
  void
  add_trans_cstr_f<true>(mm_sat_prob_t<true>& mm_pb,
                         const square_matrix<bool, true>& incompmat,
                         const iaj_t iaj, const unsigned fdj, const int iajlit,
                         const unsigned fdx_idx,
                         const std::vector<unsigned>& group_states_,
                         const std::vector<unsigned>& has_a_edge_)
  {
    const auto& lm = mm_pb.lm;
    const unsigned& n_sg = group_states_.size();
    const unsigned fdx = group_states_[fdx_idx];
    const unsigned fdx_succ = has_a_edge_[fdx_idx];
    assert(fdj < incompmat.dim());

    static std::vector<int> clause(4, 0);
    for (unsigned xidx = 0; xidx < n_sg; ++xidx)
      {
        if (fdj == fdx_succ && xidx == fdx_idx)
          continue; // founding state source -> founding state dst

        const unsigned x = group_states_[xidx];
        // x incompatible with founding state
        // (Only here due to transitive closure)
        // -> Nothing to do, taken care on in incomp section
        if (incompmat.get(fdx, x))
          continue;
        // Successor
        const unsigned xprime = has_a_edge_[xidx];
        // -1u -> No edge, always ok
        // xprime == fdj -> Edge to founding state, always ok
        if (xprime == -1u || xprime == fdj)
          continue;

        auto clause_it = clause.begin();
        *clause_it = -iajlit;
        // If xprime and the dest class are incompatible
        // -> source state can not be in i if iajlit is active
        if (xidx != fdx_idx)
          // Must be in src_class as it is founding
          *(++clause_it) = -lm.sxi2lit({x, iaj.i});
        // No need to add xprime if it is not compatible
        // with the successor of the founding state of src (if existent)
        // or the dst class
        if ((fdx_succ == -1u || !incompmat.get(fdx_succ, xprime))
            && !incompmat.get(fdj, xprime))
          *(++clause_it) = lm.sxi2lit({xprime, iaj.j});

        *(++clause_it) = 0;
        mm_pb.add_static(clause);
        trace_clause(clause);
      }
  }

  template <bool USE_PICO>
  mm_sat_prob_t<USE_PICO>
  build_init_prob(const_twa_graph_ptr split_mmw,
                  const square_matrix<bool, true>& incompmat,
                  const reduced_alphabet_t& red,
                  const part_sol_t& psol,
                  const unsigned n_env,
                  satprob_info& si)
  {
    const auto& psolv = psol.psol;
    const unsigned n_classes = psolv.size();
    const unsigned n_red = red.n_red_sigma;
    const unsigned n_groups = red.n_groups;

    mm_sat_prob_t<USE_PICO> mm_pb(n_classes, n_env, n_red, si);

    auto& lm = mm_pb.lm;

    // Creating sxi
    lm.unfreeze_xi();

    // Modif: literals sxi that correspond to the founding state of
    // a partial solution class are always true -> we therefore skip them

    // 1 Set the partial solution
    // Each partial solution state gets its own class
    // This is a simple "true" variable, so omitted

    // 2 State cover
    // Each state that is not a partial solution
    // must be in some class
    // Partial solution classes are skipped if incompatible
    const auto& is_psol = psol.is_psol;

    for (unsigned s = 0; s < n_env; ++s)
      {
        if (is_psol[s] != -1u)
          continue;
        // new clause
        mm_pb.state_cover_clauses.emplace_back(std::piecewise_construct,
            std::forward_as_tuple(s),
            std::forward_as_tuple(std::vector<int>{}));
        auto& clause = mm_pb.state_cover_clauses.back().second;
        // All possible classes
        // Note, here they are all partial solutions
        for (unsigned i = 0; i < n_classes; ++i)
          if (!incompmat.get(psolv[i], s))
            clause.push_back(lm.sxi2lit({s, i}));
      }
    // All sxi of importance, i.e. that can be true or false,
    // have been created
    lm.freeze_xi();

    // 3 Incomp
    // Modif: do not add partial solution
    // as the corresponding sxi does not appear in the cover
    // Note: special care is taken later on for closure
    trace << "c Incompatibility" << std::endl;
    {
      std::vector<int> inc_clause_(3);
      inc_clause_[2] = 0;
      for (unsigned x = 0; x < n_env; ++x)
        {
          for (unsigned i = 0; i < n_classes; ++i)
            {
              // Check if compatible with partial solution
              if (psolv[i] == x || incompmat.get(psolv[i], x))
                continue;
              // Get all the incompatible states
              for (unsigned y = x + 1; y < n_env; ++y)
                {
                  // Check if compatible with partial solution
                  if (psolv[i] == y || incompmat.get(psolv[i], y))
                    continue;
                  if (incompmat.get(x, y))
                    {
                      inc_clause_[0] = -lm.sxi2lit({x, i});
                      inc_clause_[1] = -lm.sxi2lit({y, i});
                      mm_pb.add_static(inc_clause_);
                      trace_clause(inc_clause_);
                    }
                }
            }
        }
    }


    // 4 Cover for transitions
    // Modif if target or source are not compatible with
    // the partial solution class -> simplify condition
    // List of possible successor classes
    std::vector<bool> succ_classes(n_classes);

    // Loop over all minimal letters
    // We need a vector of iterators to make it efficient
    // Attention, all letters in the split_mmw are minimal letter,
    // not all states have necessarily edges for all letters
    // (In the case of ltlsynt they do but not in general)
    using edge_it_type = decltype(split_mmw->out(0).begin());
    std::vector<std::pair<edge_it_type, edge_it_type>> edge_it;
    edge_it.reserve((unsigned) n_env/n_groups + 1);

    // has_a_edge[i] stores if i has an edge under a
    // No? -> -1u
    // Yes? -> dst state
    // This has to be done group by group now

    std::vector<unsigned> group_states_;
    // global class number + local founding state index
    std::vector<std::pair<unsigned, unsigned>> group_classes_;
    std::vector<unsigned> has_a_edge_;

    for (unsigned group = 0; group < n_groups; ++group)
      {
        trace << "c Group " << group << " trans cond\n";
        const unsigned n_letters_g = red.bisim_letters[group].size();

        group_states_.clear();
        for (unsigned s = 0; s < n_env; ++s)
          if (red.which_group[s] == group)
            group_states_.push_back(s);
        const unsigned n_states_g_ = group_states_.size();
        // All classes that have their founding state in
        // the current group
        group_classes_.clear();
        for (unsigned src_class = 0; src_class < n_classes; ++src_class)
          if (red.which_group[psolv[src_class]] == group)
            {
              unsigned idx =
                find_first_index_of(group_states_,
                                    [&](unsigned s)
                                      {return s == psolv[src_class]; });
              assert(idx != n_states_g_);
              group_classes_.emplace_back(src_class, idx);
            }
        edge_it.clear();
        for (unsigned s : group_states_)
          edge_it.emplace_back(split_mmw->out(s).begin(),
                               split_mmw->out(s).end());

        has_a_edge_.resize(n_states_g_);

        for (unsigned abddidu = 0; abddidu < n_letters_g; ++abddidu)
          {
            const unsigned abdd_idx =
                red.bisim_letters[group][abddidu].front();
            const bdd& abdd = red.minimal_letters_vec[group][abdd_idx];
            const int abddid = abdd.id();
            // Advance all iterators if necessary
            // also check if finished.
            // if all edges are treated we can stop
            // Drive by check if a exists in outs
            auto h_a_it = has_a_edge_.begin();
            std::for_each(edge_it.begin(), edge_it.end(),
                          [&abddid, &h_a_it](auto& eit)
                            {
                              *h_a_it = -1u;
                              if ((eit.first != eit.second)
                                   && (eit.first->cond.id() < abddid))
                                  ++eit.first;
                              if ((eit.first != eit.second)
                                  && (eit.first->cond.id() == abddid))
                                *h_a_it = eit.first->dst;
                              ++h_a_it;
                            });
            assert(h_a_it == has_a_edge_.end());

            // Loop over src classes, note all classes are partial solution
            // classes
            for (auto [src_class, fdx_idx] : group_classes_)
              {
                // Successor of the founding state under the current letter
                const unsigned fdx = group_states_[fdx_idx];
                const unsigned fdx_succ = has_a_edge_[fdx_idx];
                // -1u if not partial solution else number of class
                const unsigned fdx_succ_class =
                    (fdx_succ == -1u) ? -1u : is_psol[fdx_succ];

                assert(!mm_pb.trans_cover_clauses.count({src_class,
                                                         abddidu}));
                if (fdx_succ_class != -1u)
                  {
                    // The target is also partial solution state
                    // -> fdx_succ_class is the only possible successor class
                    assert(!lm.get_iaj({src_class, abddidu, fdx_succ_class}));
                    lm.unfreeze_iaj();
                    int iajlit = lm.ziaj2lit({src_class, abddidu,
                                              fdx_succ_class});
                    lm.freeze_iaj();
                    mm_pb.trans_cover_clauses[{src_class, abddidu}]
                        .push_back(iajlit);
                    add_trans_cstr_f<USE_PICO>(mm_pb,
                                               incompmat,
                                               {src_class, abddidu,
                                                fdx_succ_class},
                                               fdx_succ, iajlit, fdx_idx,
                                               group_states_, has_a_edge_);
                  }
                else
                  {
                    // We need to determine all possible
                    // successor classes
                    // Note that fdx_succ (if existent) always has to be
                    // in the target class, so only classes compatible
                    // to this state are viable
                    std::fill(succ_classes.begin(), succ_classes.end(),
                              false);
                    for (unsigned xidx = 0; xidx < n_states_g_; ++ xidx)
                      {
                        const unsigned x = group_states_[xidx];
                        // x comp src class
                        if (incompmat.get(fdx, x))
                          continue;
                        const unsigned xprime = has_a_edge_[xidx];
                        // xprime comp dst class
                        if (xprime == -1u
                            || (fdx_succ != -1u
                                && incompmat.get(fdx_succ, xprime)))
                          continue;
                        bool added_dst = false;
                        for (unsigned dst_class = 0; dst_class < n_classes;
                             ++dst_class)
                          {
                            if (succ_classes[dst_class])
                              continue;
                            if ((fdx_succ == -1u
                                 || !incompmat.get(psolv[dst_class],
                                                         fdx_succ))
                                && !incompmat.get(psolv[dst_class], xprime))
                              {
                                // Possible dst
                                succ_classes[dst_class] = true;
                                lm.unfreeze_iaj();
                                int iajlit = lm.ziaj2lit({src_class, abddidu,
                                                          dst_class});
                                lm.freeze_iaj();
                                mm_pb.trans_cover_clauses[{src_class,
                                                           abddidu}]
                                    .push_back(iajlit);
                                add_trans_cstr_f(mm_pb,
                                                 incompmat,
                                                 {src_class, abddidu,
                                                  dst_class},
                                                 psolv[dst_class], iajlit,
                                                 fdx_idx, group_states_,
                                                 has_a_edge_);
                                added_dst = true;
                              }
                          }
                        if (added_dst && all_of(succ_classes))
                          break;
                      } // All source states -> possible dst
                  } // founding state -> founding state?
              }//src_class
          } // letter
        // check if all have been used
        assert(std::all_of(edge_it.begin(), edge_it.end(),
                           [](auto& eit)
                           {
                             return ((eit.first == eit.second)
                                     || ((++eit.first) == eit.second));
                           }));
      } // group
    // Done building the initial problem
    trace << std::endl;

    // we also have to init the helper struct
    mm_pb.ocond_maps.resize(red.n_groups);
    for (unsigned i = 0; i < red.n_groups; ++i)
      mm_pb.ocond_maps[i].resize(red.minimal_letters_vec[i].size());
    mm_pb.tracked_s_pair = square_matrix<bool, true>(n_env, false);

    return mm_pb;
  }

  // This is called when we increase the number of available classes
  // We know that the new class is not associated to a partial solution
  // or founding state
  template <bool USE_PICO>
  void increment_classes(const_twa_graph_ptr split_mmw,
                         const square_matrix<bool, true>& incompmat,
                         const reduced_alphabet_t& red,
                         const part_sol_t& psol,
                         mm_sat_prob_t<USE_PICO>& mm_pb)
  {
    const unsigned new_class = mm_pb.n_classes++;
    const unsigned n_env = mm_pb.lm.n_env_;
    auto& lm = mm_pb.lm;
    const auto& psolv = psol.psol;
    const unsigned n_psol = psolv.size();
    const unsigned n_groups = red.n_groups;


    // 1 Add the new class to all states that are not founding states
    // Note that in the other case, we still have to create the
    // literal eventually, as it might be needed for the transition conditions
    auto it_cc = mm_pb.state_cover_clauses.begin();
    lm.unfreeze_xi();
    for (unsigned x = 0; x < n_env; ++x)
      {
        if (psol.is_psol[x] != -1u)
          // Partial solution state
          continue;
        assert(it_cc != mm_pb.state_cover_clauses.end());
        it_cc->second.push_back(lm.sxi2lit({x, new_class}));
        ++it_cc;
      }
    assert(it_cc == mm_pb.state_cover_clauses.end());


    // 2 Set incompatibilities
    // All states can be in the new class, so we have to set all
    // incompatibilities
    {
      trace << "c Incomp class " << new_class << '\n';
      std::vector<int> inc_clause_(3); // Vector call to pico faster
      inc_clause_[2] = 0;
      for (unsigned x = 0; x < n_env; ++x)
        {
          assert(lm.get_sxi({x, new_class}) || psol.is_psol[x] != -1u);
          for (unsigned y = x + 1; y < n_env; ++y)
            if (incompmat.get(x, y))
              {
                assert(lm.get_sxi({y, new_class}) || psol.is_psol[y] != -1u);
                inc_clause_[0] = -lm.sxi2lit({x, new_class});
                inc_clause_[1] = -lm.sxi2lit({y, new_class});
                mm_pb.add_static(inc_clause_);
                trace_clause(inc_clause_);
              }
        }
    }

    // 3 Transition cover
    // Add the new class to the cover condition of
    // transitions
    // Note that all classes are possible targets of the new
    // class and all classes can target the new class
    // Note all classes means also all groups
    // so we need all minimal letters
    lm.unfreeze_iaj();
    // New_class as dst
    for (auto& elem : mm_pb.trans_cover_clauses)
      elem.second.push_back(lm.ziaj2lit({elem.first.first, elem.first.second,
                                         new_class}));
    // New_class as src
    for (unsigned abddidu = 0; abddidu < red.n_red_sigma; ++abddidu)
      {
        auto& na_cover =
            mm_pb.trans_cover_clauses[{new_class, abddidu}];
        na_cover.reserve(new_class + 1);
        for (unsigned dst_class = 0; dst_class <= new_class; ++dst_class)
          na_cover.push_back(lm.ziaj2lit({new_class, abddidu, dst_class}));
      }
    lm.freeze_iaj();


    // 4 Transition
    // As before, simplify conditions
    // we need to loop through all letters again
    using edge_it_type = decltype(split_mmw->out(0).begin());
    static std::vector<std::pair<edge_it_type, edge_it_type>> edge_it;
    edge_it.reserve((unsigned) n_env/n_groups + 1);

    // has_a_edge[i] stores if i has an edge under a
    // No? -> -1u
    // Yes? -> dst state
    // This has to be done group by group now

    static std::vector<unsigned> group_states_;
    static std::vector<unsigned> has_a_edge_;

    for (unsigned group = 0; group < n_groups; ++group)
      {
        trace << "c Trans conditions group " << group << '\n';
        const unsigned n_letters_g = red.bisim_letters[group].size();

        group_states_.clear();
        for (unsigned s = 0; s < n_env; ++s)
          if (red.which_group[s] == group)
            group_states_.push_back(s);
        const unsigned n_states_g_ = group_states_.size();
        edge_it.clear();
        for (unsigned s : group_states_)
          edge_it.emplace_back(split_mmw->out(s).begin(),
                               split_mmw->out(s).end());

        has_a_edge_.resize(n_states_g_);

        for (unsigned abddidu = 0; abddidu < n_letters_g; ++abddidu)
          {
            const unsigned abdd_idx =
              red.bisim_letters[group][abddidu].front();
            const bdd& abdd = red.minimal_letters_vec[group][abdd_idx];
            const int abddid = abdd.id();
            // Advance all iterators if necessary
            // also check if finished.
            // if all edges are treated we can stop
            // Drive by check if a exists in outs
            auto h_a_it = has_a_edge_.begin();
            std::for_each(edge_it.begin(), edge_it.end(),
                          [&abddid, &h_a_it](auto& eit)
                            {
                              *h_a_it = -1u;
                              if ((eit.first != eit.second)
                                  && (eit.first->cond.id() < abddid))
                                ++eit.first;
                              if ((eit.first != eit.second)
                                  && (eit.first->cond.id() == abddid))
                                *h_a_it = eit.first->dst;
                              ++h_a_it;
                            });
            assert(h_a_it == has_a_edge_.end());

            // All other classes
            // Loop over all states of the group
            std::vector<int> inc_clause(4, 0);
            for (unsigned xidx = 0; xidx < n_states_g_; ++xidx)
              {
                const unsigned x = group_states_[xidx];
                const unsigned xprime = has_a_edge_[xidx];
                if (xprime == -1u)
                  continue; // No edge here

                // New class as dst
                // All transitions can go
                // but not all transitions can start in any class
                // Note that it can also go to it self hence the <=
                // Important the literal sxi is simply true for x <-> i
                // a) x is incompatible with partial solution -> skip
                // b) x is the founding state of the src class
                //    -> then it is always in the class
                // c) x can possibly be in the src class
                for (unsigned src_class = 0; src_class <= new_class;
                     ++src_class)
                  {
                    // a)
                    if ((src_class < n_psol)
                        && incompmat.get(x, psolv[src_class]))
                      // x can not be in source class
                      // No additional constraints necessary
                      continue;
                    // the iaj
                    inc_clause[0] = -lm.ziaj2lit({src_class, abddidu,
                                                  new_class});

                    // The next two sxi2lit can introduce new literals
                    // all states can possibly be in the new class
                    inc_clause[1] = lm.sxi2lit({xprime, new_class});

                    // x is not in src class.
                    // This is not possible if x is founding state
                    if (src_class < n_psol && psolv[src_class] == x)
                      // b) Founding state
                      inc_clause[2] = 0;
                    else
                      // c) Full condition
                      inc_clause[2] = -lm.sxi2lit({x, src_class});

                    trace_clause(inc_clause);
                    mm_pb.add_static(inc_clause);
                  }//src classes
                // New class as src
                // all states can be in there, but not all targets must
                // be compatible
                // "self-loop" already covered
                // Note: If the dst is a partial solution class then
                // a) xprime is a founding state
                // b) The xprime is incompatible with the founding state
                //    -> -sxi || -ziaj
                // the clause -sxi || -ziaj || sxprimej is trivially fulfilled
                // c) All other cases -> full clause

                for (unsigned dst_class = 0; dst_class < new_class;
                     ++dst_class)
                  {
                    if (dst_class < n_psol && psolv[dst_class] == xprime)
                      continue; // case a)

                    // case b)
                    inc_clause[0] = -lm.sxi2lit({x, new_class});
                    inc_clause[1] = -lm.ziaj2lit({new_class, abddidu,
                                                  dst_class});
                    // Adding lit to go from b) to c)
                    if (dst_class < n_psol
                        && incompmat.get(xprime, psolv[dst_class]))
                      inc_clause[2] = 0;
                    else
                      {
                        int sxi = lm.sxi2lit({xprime, dst_class});
                        assert(sxi);
                        inc_clause[2] = sxi;
                      }

                    trace_clause(inc_clause);
                    mm_pb.add_static(inc_clause);
                  }// dst calsses
              } // states
          } // letters
      } // groups
    lm.freeze_xi();

    // "Propagate" the knowledge about cases
    // where the usual constraints are insufficient to cope with
    // the expressiveness of bdds
    // All constraints on the cube(s) are conditioned by
    // whether or not the states share some class ixy
    // So here we only need to add the new class to all
    // tracked states.
    std::vector<int> c_clause(4, 0);
    const auto& lm_c = lm;
    // The state literals must exist
    for (unsigned s1 = 0; s1 < n_env; ++s1)
      for (unsigned s2 = s1 + 1; s2 < n_env; ++s2)
        if (mm_pb.tracked_s_pair.get(s1, s2))
          {
            c_clause[0] = -lm.sxi2lit({s1, new_class});
            c_clause[1] = -lm_c.sxi2lit({s2, new_class});
            c_clause[2] = lm_c.ixy2lit({s1, s2});
          }
  } // done increment_classes

  std::vector<std::vector<bdd>>
  comp_represented_cond(const reduced_alphabet_t& red)
  {
    const unsigned n_groups = red.n_groups;
    // For each minimal letter, create the (input) condition that it represents
    // This has to be created for each minimal letters
    // and might be shared between alphabets
    std::vector<std::vector<bdd>> gmm2cond;
    gmm2cond.reserve(n_groups);
    for (unsigned group = 0; group < n_groups; ++group)
      {
        const unsigned n_sigma = red.share_sigma_with[group];
        if (n_sigma == group)
          {
            // New alphabet, construct all the conditions
            const unsigned size_sigma = red.minimal_letters_vec[group].size();
            gmm2cond.push_back(std::vector<bdd>(size_sigma, bddfalse));
            for (unsigned idx = 0; idx < size_sigma; ++idx)
              {
                // We need to actually construct it
                bdd repr = red.minimal_letters_vec[group][idx];
                const auto& [_, repr_letters] =
                  red.minimal_letters[group].at(repr);
                (void)_;
                // The class of letters is the first set
                for (int id : repr_letters)
                  {
                    bdd idbdd = bdd_from_int(id);
                    gmm2cond.back()[idx] |= idbdd;
                  }
              }
          }
        else
          {
            // Copy
            assert(n_sigma < group);
            gmm2cond.push_back(gmm2cond.at(n_sigma));
          }
      }
    return gmm2cond;
  }

  void cstr_split_mealy(twa_graph_ptr& minmach,
                        const reduced_alphabet_t& red,
                        const std::vector<std::pair<unsigned,
                                                    std::vector<bool>>>&
                          x_in_class,
                        std::unordered_map<iaj_t, bdd,
                            decltype(iaj_hash),
                            decltype(iaj_eq)>& used_ziaj_map)
  {
    const unsigned n_env_states = minmach->num_states();
    // Looping over unordered is different on arm vs intel
    // todo better way to do this?
    // We could move it I guess, but the elements are fairly cheap to copy
    auto used_ziaj_ordered = std::vector<std::pair<iaj_t, bdd>>();
    used_ziaj_ordered.reserve(used_ziaj_map.size());

    for (const auto& e : used_ziaj_map)
      used_ziaj_ordered.push_back(e);

    std::sort(used_ziaj_ordered.begin(), used_ziaj_ordered.end(),
              [](const auto& pl, const auto& pr)
                {return pl.first < pr.first; });

    // For each minimal letter, create the (input) condition that it represents
    // This has to be created for each minimal letters
    // and might be shared between alphabets
    std::vector<std::vector<bdd>> gmm2cond
        = comp_represented_cond(red);

#ifdef TRACE
    for (const auto& el : used_ziaj_ordered)
      {
        const unsigned group = x_in_class[el.first.i].first;
        const bdd& incond = gmm2cond[group][el.first.a];
        std::cerr << el.first.i << " - " << incond << " / "
                  << el.second << " > " << el.first.j << std::endl;
      }
#endif

    // player_state_map
    // xi.x -> dst class
    // xi.i -> id of outcond
    // value -> player state
    std::unordered_map<xi_t, unsigned, pair_hash> player_state_map;
    player_state_map.reserve(minmach->num_states());
    auto get_player = [&](unsigned dst_class, const bdd& outcond)
      {
        auto [it, inserted] =
        player_state_map.try_emplace({dst_class, (unsigned) outcond.id()},
                                     -1u);
        if (inserted)
          {
            it->second = minmach->new_state();
            minmach->new_edge(it->second, dst_class, outcond);
            trace << "Added p " << it->second << " - " << outcond << " > "
                  << dst_class << std::endl;
          }
        assert(it->second < minmach->num_states());
        return it->second;
      };

    // env_edge_map
    // xi.x -> src_class
    // xi.i -> player state
    // value -> edge_number
    std::unordered_map<xi_t, unsigned, pair_hash> env_edge_map;
    env_edge_map.reserve(minmach->num_states());

    auto add_edge = [&](unsigned src_class, unsigned dst_class,
                        const bdd& incond, const bdd& outcond)
      {
        unsigned p_state = get_player(dst_class, outcond);
        auto it = env_edge_map.find({src_class, p_state});
        if (it == env_edge_map.end())
          {
            // Construct the edge
            env_edge_map[{src_class, p_state}] =
              minmach->new_edge(src_class, p_state, incond);
            trace << "Added e " << src_class << " - " << incond << " > "
                  << p_state << std::endl;
          }
        else
          // There is already an edge from src to pstate -> or the condition
          minmach->edge_storage(it->second).cond |= incond;
      };

    for (const auto& [iaj, outcond] : used_ziaj_ordered)
      {
        // The group determines the incond
        const unsigned group = x_in_class[iaj.i].first;
        const bdd& incond = gmm2cond[group][iaj.a];
        add_edge(iaj.i, iaj.j, incond, outcond);
      }

    // Set the state-player
    auto* sp = new std::vector<bool>(minmach->num_states(), true);
    for (unsigned i = 0; i < n_env_states; ++i)
      (*sp)[i] = false;
    minmach->set_named_prop("state-player", sp);
  }

  // This function refines the sat constraints in case the
  // incompatibility computation relying on bdds is too optimistic
  // It add constraints for each violating class and letter
  template <bool USE_PICO>
  void add_bdd_cond_constr(mm_sat_prob_t<USE_PICO>& mm_pb,
                           const_twa_graph_ptr mmw,
                           const reduced_alphabet_t& red,
                           const unsigned n_env,
                           const std::deque<std::pair<unsigned, unsigned>>&
                             infeasible_classes,
                           const std::vector<std::pair<unsigned,
                                                       std::vector<bool>>>&
                            x_in_class)
  {
    //infeasible_classes : Class, Letter index
    const unsigned n_groups = red.n_groups;

    // Step one: Decompose all concerned conditions
    // that have not yet been decomposed
    decltype(mm_pb.ocond_maps) ocond_maps(n_groups);
    for (unsigned i = 0; i < n_groups; ++i)
      ocond_maps[i].resize(red.minimal_letters_vec[i].size());

    // Helper
    auto get_o_cond = [&](const auto& e)
      {
        return mmw->out(e.dst).begin()->cond;
      };
    for (auto [n_class, letter_idx] : infeasible_classes)
      {
        trace << "c Adding additional constraints for class "
              << n_class << " and letter id " << letter_idx << '\n';
        const unsigned group = red.which_group[x_in_class[n_class].first];
        // Check all states in this class if already decomposed
        for (unsigned s = 0; s < n_env; ++s)
          {
            // In class?
            if (!x_in_class[group].second[s])
              continue;
            // Decomposed?
            // Note the set is only ordered with respect to the state,
            // so it does not matter which bdd we use to check if the
            // outcond of the minimal letter and is already decomposed
            if (mm_pb.ocond_maps.at(group).at(letter_idx).count({s, bddfalse}))
              continue;

            // Search for the actual edge implied by this minimal letter
            // there is only one
            const auto& impl_edges =
                red.minimal_letters.at(group).
                  at(red.minimal_letters_vec.at(group).at(letter_idx)).first;
            bdd econd = bddfalse;
            for (const auto& e : mmw->out(s))
              if (impl_edges.count(e.cond.id()))
                {
                  assert(econd == bddfalse);
                  econd = get_o_cond(e);
#ifdef NDEBUG
                  break;
#endif
                }
            // Safe it
            assert(econd != bddfalse);
            ocond_maps[group][letter_idx].emplace(s, econd);
            // Check if this bdd (econd) was already decomposed earlier on
            // If not, do so
            // Decompose it into cubes
            auto [it, inserted] =
                mm_pb.cube_map.try_emplace(econd.id(),
                                           std::vector<bdd>());

            if (inserted)
              {
                minato_isop econd_isop(econd);
                bdd prod;
                while ((prod = econd_isop.next()) != bddfalse)
                  it->second.push_back(prod);
              }
          }
      }
    // Decomposed all newly discovered conflicts

    // Compute the literals and clauses
    // of the new cases
    // 1) Covering condition for conditions that have more than one cube
    auto& lm = mm_pb.lm;
    std::vector<int> c_clause;
    for (unsigned group = 0; group < n_groups; ++group)
      {
        const auto& group_map = ocond_maps[group];
        const unsigned n_ml = red.minimal_letters_vec[group].size();
        for (unsigned letter_idx = 0; letter_idx < n_ml; ++letter_idx)
          {
            const auto& ml_list = group_map[letter_idx];
            for (const auto& [s, econd] : ml_list)
              {
                const unsigned n_cubes = mm_pb.cube_map.at(econd.id()).size();
                assert(n_cubes);
                c_clause.clear();
                for (unsigned idx = 0; idx < n_cubes; ++idx)
                  c_clause.push_back(lm.cuxk2lit({letter_idx, s, idx}));
                c_clause.push_back(0);
                mm_pb.add_static(c_clause);
                trace_clause(c_clause);
              }
          }
      }

    // 2) Incompatibility condition between cubes
    //    If a condition has only one cube -> use the state
    //    Attention lm.get(sxi) returns zero if x is the founding state of i
    //    Incompatibilities can arise between new/new and new/old conditions
    //    Avoid redundant clauses for new/new constraints new/new
    auto create_cstr = [&](unsigned s1, unsigned s2,
                           const std::vector<std::pair<int, int>>&
                                 incomp_cubes_list)
      {
        // No simplification as this can backfire

        // Helper literal that determines if s1 and s2 are
        // at least in one common class
        // either s1 is not in class or s2 is not in class
        // ot is1s2
        const int is1s2 = lm.ixy2lit({s1, s2});
        // The constraint below is might have already been
        // constructed if so, the states are marked as tracked
        if (!mm_pb.tracked_s_pair.get(s1, s2))
          {
            mm_pb.tracked_s_pair.set(s1, s2, true);
            for (unsigned iclass = 0; iclass < mm_pb.n_classes; ++iclass)
              {
                c_clause.clear();
                int c_lit;
                c_lit = lm.get_sxi({s1, iclass});
                if (c_lit)
                  c_clause.push_back(-c_lit);
                c_lit = lm.get_sxi({s2, iclass});
                if (c_lit)
                  c_clause.push_back(-c_lit);
                c_clause.push_back(is1s2);
                c_clause.push_back(0);
                mm_pb.add_static(c_clause);
                trace_clause(c_clause);
              }
          }
        // Now all the additional clauses have the form
        // not same class or not cube1 or not cube2
        c_clause.resize(4);
        std::fill(c_clause.begin(), c_clause.end(), 0);
        c_clause[0] = -is1s2;
        for (auto [c1_lit, c2_lit] : incomp_cubes_list)
          {
            c_clause[1] = -c1_lit;
            c_clause[2] = -c2_lit;
            mm_pb.add_static(c_clause);
            trace_clause(c_clause);
          }
      };

    auto fill_incomp_list = [&](std::vector<std::pair<int, int>>&
                                  incomp_cubes_list,
                                unsigned letter_idx,
                                unsigned s1, const std::vector<bdd>& c_list1,
                                unsigned s2, const std::vector<bdd>& c_list2)
      {
        const unsigned n_c1 = c_list1.size();
        const unsigned n_c2 = c_list2.size();
        incomp_cubes_list.clear();
        for (unsigned c1_idx = 0; c1_idx < n_c1; ++c1_idx)
          for (unsigned c2_idx = 0; c2_idx < n_c2; ++c2_idx)
            {
              auto [it, inserted] =
                mm_pb.cube_incomp_map.try_emplace({c_list1[c1_idx].id(),
                                                   c_list2[c2_idx].id()},
                                                   false);
              if (inserted)
                it->second =
                    bdd_have_common_assignment(c_list1[c1_idx],
                                               c_list2[c2_idx]);
              if (!it->second)
                incomp_cubes_list.emplace_back((int) c1_idx,
                                               (int) c2_idx);
            }

        // Replace the indices in the incomp_cubes_list
        // with the literals if there is more than one cube the
        // reduce the number of look-ups
        auto repl1 = [&, ntrans = n_c1 == 1](int idx)
          {
            return ntrans ? idx : lm.cuxk2lit({letter_idx,
                                              s1,
                                              (unsigned) idx});
          };
        auto repl2 = [&, ntrans = n_c2 == 1](int idx)
          {
            return  ntrans ? idx : lm.cuxk2lit({letter_idx,
                                                s2,
                                                (unsigned) idx});
          };
        std::transform(incomp_cubes_list.begin(),
                       incomp_cubes_list.end(),
                       incomp_cubes_list.begin(),
                       [&](auto& el) -> std::pair<int, int>
                       {
                         auto r1 = repl1(el.first);
                         auto r2 = repl2(el.second);
                         return {r1, r2}; });
      };

    std::vector<std::pair<int, int>> incomp_cubes_list;
    for (unsigned group = 0; group < n_groups; ++group)
      {
        const auto& group_map = ocond_maps[group];
        const unsigned n_ml = red.minimal_letters_vec[group].size();
        for (unsigned letter_idx = 0; letter_idx < n_ml; ++letter_idx)
          {
            const auto& ml_list = group_map[letter_idx];
            assert(ml_list.begin() != ml_list.end());
            // Incompatibility is commutative
            // new / new constraints
            const auto it_end = ml_list.end();
            const auto it_endm1 = --ml_list.end();
            for (auto it1 = ml_list.begin(); it1 != it_endm1; ++it1)
              {
                auto it2 = it1;
                ++it2;
                for (; it2 != it_end; ++it2)
                  {
                    const auto& [s1, oc1] = *it1;
                    const auto& [s2, oc2] = *it2;
                    const auto& c_list1 = mm_pb.cube_map.at(oc1.id());
                    const unsigned n_c1 = c_list1.size();
                    const auto& c_list2 = mm_pb.cube_map.at(oc2.id());
                    const unsigned n_c2 = c_list2.size();

                    // If both of them only have one cube the base constraints
                    // are sufficient
                    if (n_c1 == 1 && n_c2 == 1)
                      continue;
                    // The simplified condition is also correct if
                    // every cube of s1 intersects with every cube of s2
                    // Build a list of incompatible cubes
                    fill_incomp_list(incomp_cubes_list, letter_idx,
                                     s1, c_list1, s2, c_list2);

                    if (incomp_cubes_list.empty())
                      continue;

                    create_cstr(s1, s2, incomp_cubes_list);
                  }
              }
            // New / old constraints
            // We need to add constraints between newly decomposed
            // letter/states combinations with the ones already found
            const auto& ml_list_old = mm_pb.ocond_maps[group][letter_idx];
            for (const auto& [s1, oc1] : ml_list)
              for (const auto& [s2, oc2] : ml_list_old)
                {
                  const auto& c_list1 = mm_pb.cube_map.at(oc1.id());
                  const unsigned n_c1 = c_list1.size();
                  const auto& c_list2 = mm_pb.cube_map.at(oc2.id());
                  const unsigned n_c2 = c_list2.size();

                  if (n_c1 == 1 && n_c2 == 1)
                    continue;

                  fill_incomp_list(incomp_cubes_list, letter_idx,
                                   s1, c_list1, s2, c_list2);

                  if (incomp_cubes_list.empty())
                    continue;

                  create_cstr(s1, s2, incomp_cubes_list);
                }
          }
      }
      // Done creating the additional constraints
  }

  template<bool USE_PICO>
  twa_graph_ptr
  try_build_min_machine(mm_sat_prob_t<USE_PICO>& mm_pb,
                        const_twa_graph_ptr mmw,
                        const reduced_alphabet_t& red,
                        const part_sol_t& psol,
                        const unsigned n_env,
                        satprob_info& si)
  {
    const auto& psolv = psol.psol;
    const unsigned n_psol = psolv.size();
    const unsigned n_groups = red.n_groups;

    // List of incompatible/infeasible classes and letters
    // first class, second minimal letter
    std::deque<std::pair<unsigned, unsigned>> infeasible_classes;

    // Check if a solution exists for the current prob
    // Add the variable clauses
    // looks daring but we either find a solution or it is infeasible
    // in both cases we return directly
    while (true)
      {
        infeasible_classes.clear();
#ifdef TRACE
        mm_pb.lm.print(std::cerr);
#endif
        mm_pb.set_variable_clauses();
        si.n_lit = mm_pb.n_lits();
        si.n_clauses = mm_pb.n_clauses();
        si.start();
        auto sol = mm_pb.get_sol();
        si.sat_time = si.restart();

        if (sol.empty())
          {
            mm_pb.unset_variable_clauses();
            si.write();
            return nullptr;
          }
#ifdef TRACE
        mm_pb.lm.print(std::cerr, &sol);
#endif

        const unsigned n_classes = mm_pb.n_classes;
        const auto& lm = mm_pb.lm;

        auto minmach = make_twa_graph(mmw->get_dict());
        minmach->copy_ap_of(mmw);

        // We have as many states as classes
        // Plus the state necessary for player states if
        // we do not unsplit
        minmach->new_states(n_classes);

        // Get the init state
        unsigned x_init = mmw->get_init_state_number();
        trace << "orig init " << x_init << '\n';
        std::vector<unsigned> init_class_v;
        assert(x_init < n_env);
        {
          trace << "List of init classes ";
          if (psol.is_psol[x_init] != -1u)
            init_class_v.push_back(psol.is_psol[x_init]);
          for (unsigned i = 0; i < n_classes; ++i)
            {
              int litsxi = lm.get_sxi({x_init, i});
              if (litsxi && (sol.at(litsxi) == 1))
                {
                  init_class_v.push_back(i);
                  trace << i << " -> "<< litsxi << " : " << sol.at(litsxi)
                        << std::endl;
                }
            }
          assert(!init_class_v.empty());
        }

        // Save which class has which states
        // and to which group this class belongs
        std::vector<std::pair<unsigned, std::vector<bool>>>
            x_in_class(n_classes,
                       std::make_pair(-1u, std::vector<bool>(n_env, false)));

        // Todo : Check if we can "reduce" the solution
        // that is to remove states from classes without
        // violating the constraints.
        // Idea: Less constraints to encode -> smaller AIGER for syntcomp

        // Add partial solution state
        for (unsigned i = 0; i < n_classes; ++i)
          {
            for (unsigned x = 0; x < n_env; ++x)
              // By convention 0-lit is false
              x_in_class[i].second[x] =
                  (sol.at(lm.get_sxi({x, i})) == 1);
            if (i < n_psol)
              // partial solution
              x_in_class[i].second[psolv[i]] = true;
            unsigned first_x = find_first_index_of(x_in_class[i].second);
            assert(first_x != n_env && "No state in class.");
            x_in_class[i].first = red.which_group[first_x];
          }
    #ifdef TRACE
        for (unsigned i = 0; i < n_classes; ++i)
          {
            trace << "Class " << i << " group " << x_in_class[i].first << '\n';
            for (unsigned x = 0; x < n_env; ++x)
              if (x_in_class[i].second[x])
                trace << x << ' ';
            trace << std::endl;
          }
    #endif

        // Check that each class has only states of the same group
        assert(std::all_of(x_in_class.begin(), x_in_class.end(),
                           [&red, n_env](const auto& p)
                             {
                               for (unsigned i = 0; i < n_env; ++i)
                                 if (p.second[i]
                                     && (red.which_group[i] != p.first))
                                   {
                                     trace << "state "
                                           << i << " is in a class "
                                                   "associated to group "
                                           << p.first << std::endl;
                                     return false;
                                   }
                               return true;
                             }));

        // Build the conditions
        // Decide on which ziaj to use
        std::unordered_map<iaj_t, bdd, decltype(iaj_hash), decltype(iaj_eq)>
            used_ziaj_map(0, iaj_hash, iaj_eq);
        used_ziaj_map.reserve(1 + lm.ziaj_map_.size()/2);

        // todo : Here we use the first successor found.
        //        Better ideas?
        // Note ziaj : class i goes to j for minimal bisimilar letter a.
        // a represents a number of minimal letters -> all of them
        // make i go to j
        for (unsigned src_class = 0; src_class < n_classes; ++src_class)
          {
            const unsigned group = x_in_class[src_class].first;
            const unsigned n_mlb = red.bisim_letters[group].size();
            for (unsigned amlbidu = 0; amlbidu < n_mlb; ++amlbidu)
              {
                for (unsigned dst_class = 0; dst_class < n_classes; ++dst_class)
                  if (sol.at(lm.get_iaj({src_class, amlbidu, dst_class})) == 1)
                    {
                      trace << "using mlb " << src_class << ' ' << amlbidu
                            << ' ' << dst_class << std::endl;
                      //Accept this for all bisimilar
                      for (unsigned abddidu : red.bisim_letters[group][amlbidu])
                        {
                          used_ziaj_map[{src_class, abddidu, dst_class}]
                              = bddtrue;
                          trace << "using " << src_class << ' ' << abddidu
                                << ' ' << dst_class << std::endl;
                        }
                      break;
                    }
              }
          }
        // todo check for no trans?
        // Attention, from here we treat the minimal letters
        // not the bisimilar ones, as minimal letters share outconds, which is
        // not the case for bisimilar ones

        // Loop over edges to construct the out conds
        auto get_o_cond = [&](const auto& e)
          {
            return mmw->out(e.dst).begin()->cond;
          };
#ifndef NDEBUG
        auto get_env_dst = [&](const auto& e)
          {
            return mmw->out(e.dst).begin()->dst;
          };
#endif

        // The first two loops cover all env-edges
        for (unsigned group = 0; group < n_groups; ++group)
          {
            const auto& min_let_g = red.minimal_letters[group];
            const auto& min_let_v_g = red.minimal_letters_vec[group];
            const unsigned nmlg = min_let_v_g.size();

            for (unsigned src = 0; src < n_env; ++src)
              {
                if (red.which_group[src] != group)
                  continue;
                for (const auto& e : mmw->out(src))
                  // Check which minimal letters implies this edge
                  {
                    // All minimal words of this group
                    for (unsigned abddidu = 0; abddidu < nmlg; ++abddidu)
                      {
                        const auto& impl_sets =
                          min_let_g.at(min_let_v_g[abddidu]);
                        if (impl_sets.first.count(e.cond.id()))
                          {
                            bdd outcond = get_o_cond(e);
                            // This edge has the "letter" abddidu
                            // abddidu -> e.cond
                            // Loop over all possible src- and dst-classes
                            // AND the out conditions
                            for (unsigned src_class = 0;
                                 src_class < n_classes;
                                 ++src_class)
                              {
                                if (!x_in_class[src_class].second[e.src])
                                  continue;
                                for (unsigned dst_class = 0;
                                     dst_class < n_classes;
                                     ++dst_class)
                                  {
                                    // Currently we only use the first one
                                    // but we still loop over
                                    // all if changed later
                                    auto it = used_ziaj_map.find({src_class,
                                                                  abddidu,
                                                                  dst_class});
                                    if (it != used_ziaj_map.end())
                                      {
                                        assert(
                                          x_in_class[dst_class]
                                              .second[get_env_dst(e)]);
                                        it->second &= outcond;
                                      }
                                  } // dst_class
                              } // src_class
                          }
                      } // min_let
                  } // e
              } // s
          } // groups
        // Check if all the outconds are actually feasible,
        // that is all outconds are not false
        for (const auto& el : used_ziaj_map)
          if (el.second == bddfalse)
            infeasible_classes.emplace_back(el.first.i, el.first.a);
        si.build_time = si.restart();

        if (!infeasible_classes.empty())
          {
            // Remove the variable clauses
            // This is suboptimal but the contexts form a stack so...
            auto oldrefine = si.n_refinement;
            si.write();
            si.task = "refinement";
            si.n_classes = n_classes;
            si.n_refinement = oldrefine + infeasible_classes.size();
            mm_pb.unset_variable_clauses();
            add_bdd_cond_constr(mm_pb, mmw, red, n_env,
                                infeasible_classes, x_in_class);
            si.refine_time = si.restart();
            continue; //retry
          }

        cstr_split_mealy(minmach, red, x_in_class, used_ziaj_map);
        // todo: What is the impact of chosing one of the possibilities
        minmach->set_init_state(init_class_v.front());
        return minmach;
      } // while loop
  } // try_build_machine

  twa_graph_ptr minimize_mealy_(const const_twa_graph_ptr& mm,
                               int premin, int max_letter_mult)
  {
    bdd outputs = ensure_mealy("minimize_mealy", mm);

    satprob_info si(sat_instance_name);
    si.task = "presat";
    stopwatch sglob;
    sglob.start();
    si.start();

    if ((premin < -1) || (premin > 1))
      throw std::runtime_error("premin has to be -1, 0 or 1");

    auto do_premin = [&]()->const_twa_graph_ptr
      {
        if (premin == -1)
          {
            if (!mm->get_named_prop<region_t>("state-player"))
              return split_2step(mm, false);
            else
              return mm;
          }
        else
          {
            bool is_split = mm->get_named_prop<region_t>("state-player");
            // We have a split machine -> unsplit then resplit,
            // as reduce mealy works on separated
            twa_graph_ptr mms;
            if (is_split)
              {
                auto mmi = unsplit_2step(mm);
                reduce_mealy_here(mmi, premin == 1);
                split_separated_mealy_here(mmi);
                return mmi;
              }
            else
              {
                auto mms = reduce_mealy(mm, premin == 1);
                return split_2step(mms, false);
              }
          }
      };

    const_twa_graph_ptr mmw = do_premin();
    assert(is_split_mealy(mmw));
    si.premin_time = si.restart();



    // 0 -> "Env" next is input props
    // 1 -> "Player" next is output prop
    const region_t& spref = get_state_players(mmw);
    assert((spref.size() == mmw->num_states())
           && "Inconsistent state players");

    // Reorganize the machine such that
    // first block: env-states
    // second black : player-states
    unsigned n_env = -1u;
    std::tie(mmw, n_env) = reorganize_mm(mmw, spref);
    assert(is_split_mealy(mmw));
#ifdef TRACE
    print_hoa(std::cerr, mmw);
#endif
    assert(n_env != -1u);
    si.reorg_time = si.restart();

    // Compute incompatibility based on bdd
    auto incompmat = compute_incomp(mmw, n_env, si, max_letter_mult);
#ifdef TRACE
    std::cerr << "Final incomp mat\n";
    incompmat.print(std::cerr);
#endif

    // Get a partial solution
    auto partsol = get_part_sol(incompmat);
    si.partsol_time = si.restart();


    auto early_exit = [&]()
      {
        si.done = 1;
        si.total_time = sglob.stop();
        si.write();
        // Always keep machines split
        if (mm->get_named_prop<region_t>("state-player"))
          assert(is_split_mealy_specialization(mm, mmw));
        else
          assert(is_split_mealy_specialization(split_2step(mm, false),
                                               mmw));
        return std::const_pointer_cast<twa_graph>(mmw);
      };

    // If the partial solution has the same number of
    // states as the original automaton -> we are done
    if (partsol.psol.size() == n_env)
      {
        return early_exit();
      }

    // Get the reduced alphabet
    auto [split_mmw, reduced_alphabet] =
        reduce_and_split(mmw, n_env, incompmat, si);

    auto mm_pb = build_init_prob<true>(split_mmw, incompmat,
                                       reduced_alphabet, partsol, n_env, si);
    si.prob_init_build_time = si.restart();
    si.write();

    twa_graph_ptr minmachine = nullptr;
    for (size_t n_classes = partsol.psol.size();
         n_classes < n_env; ++n_classes)
      {
        if (si.task.empty())
          si.task = "sat";
        si.n_iteration = (n_classes-partsol.psol.size());
        si.n_refinement = 0;
        si.n_classes = n_classes;

        minmachine = try_build_min_machine(mm_pb, mmw,
                                           reduced_alphabet,
                                           partsol, n_env,
                                           si);

        if (minmachine)
          break;
        increment_classes(split_mmw, incompmat, reduced_alphabet,
                          partsol, mm_pb);

      }
    // Is already minimal -> Return a copy
    // Set state players!
    if (!minmachine)
      return early_exit();
    set_synthesis_outputs(minmachine, outputs);

    si.done=1;
    si.n_min_states = minmachine->num_states();
    si.total_time = sglob.stop();
    si.write();

    assert(is_split_mealy_specialization(
      mm->get_named_prop<region_t>("state-player") ? mm
                                                   :split_2step(mm, false),
      minmachine));
    return minmachine;
  }
} // namespace

namespace spot
{
  twa_graph_ptr minimize_mealy(const const_twa_graph_ptr& mm,
                               int premin)
  {
    return minimize_mealy_(mm, premin, 10);
  }

  twa_graph_ptr
  minimize_mealy(const const_twa_graph_ptr& mm,
                 synthesis_info& si)
  {
    if ((si.minimize_lvl < 3) || (si.minimize_lvl > 5))
      throw std::runtime_error("minimize_mealy(): "
                               "minimize_lvl should be between 3 and 5.");

    std::string csvfile = si.opt.get_str("satlogcsv");
    std::string dimacsfile = si.opt.get_str("satlogdimacs");

    if (!csvfile.empty())
      {
        sat_csv_file = std::make_unique<std::ofstream>
          (csvfile, std::ios_base::ate | std::ios_base::app);
        if (!*sat_csv_file)
          throw std::runtime_error("could not open `" + csvfile
                                   + "' for writing");
        sat_csv_file->exceptions(std::ofstream::failbit
                                 | std::ofstream::badbit);
      }
    if (!dimacsfile.empty())
      sat_dimacs_file
        = std::make_unique<fwrapper>(dimacsfile);
    sat_instance_name = si.opt.get_str("satinstancename");
    auto res = minimize_mealy_(mm, si.minimize_lvl-4,
                               si.opt.get("max_letter_mult", 10));
    sat_csv_file.reset();
    sat_dimacs_file.reset();
    return res;
  }
}

namespace spot
{
  bool is_split_mealy_specialization(const_twa_graph_ptr left,
                                     const_twa_graph_ptr right,
                                     bool verbose)
  {
    assert(is_split_mealy(left));
    assert(is_split_mealy(right));

    const unsigned initl = left->get_init_state_number();
    const unsigned initr = right->get_init_state_number();

    const region_t& spr = get_state_players(right);
#ifndef NDEBUG
    const region_t& spl = get_state_players(left);
    // todo
    auto check_out = [](const const_twa_graph_ptr& aut,
                        const auto& sp)
      {
        for (unsigned s = 0; s < aut->num_states(); ++s)
          if (sp.at(s))
            if (((++aut->out(s).begin()) != aut->out(s).end())
                || (aut->out(s).begin() == aut->out(s).end()))
              {
                std::cerr << "Failed for " << s << '\n';
                return false;
              }

        return true;
      };
    assert(check_out(left, spl) &&
           "Left mealy machine has multiple or no player edges for a state");
    assert(check_out(right, spr) &&
           "Right mealy machine has multiple or no player edges for a state");
#endif
    // Get for each env state of right the uncovered input letters
    std::vector<bdd> ucr(right->num_states(), bddtrue);
    const unsigned nsr = right->num_states();
    for (unsigned s = 0; s < nsr; ++s)
      {
        if (spr[s])
          continue;
        for (const auto& e : right->out(s))
          ucr[s] -= e.cond;
      }

    using prod_state = std::pair<unsigned, unsigned>;

    std::unordered_set<prod_state, pair_hash> seen;
    std::deque<prod_state> todo;
    todo.emplace_back(initl, initr);
    seen.emplace(todo.back());

    auto get_p_edge_l = [&](const auto& e_env)
      {return *(left->out(e_env.dst).begin()); };
    auto get_p_edge_r = [&](const auto& e_env)
      {return *(right->out(e_env.dst).begin()); };

    while (!todo.empty())
      {
        const auto [sl, sr] = todo.front();
        todo.pop_front();
        for (const auto& el_env : left->out(sl))
          {
            // check if el_env.cond intersects with the unspecified of
            // sr. If so the sequence is not applicable -> false
            if (bdd_have_common_assignment(ucr[sr], el_env.cond))
              {
                if (verbose)
                  std::cerr << "State " << sl << " of left is not completely"
                            << " covered by " << sr << " of right.\n";
                return false;
              }


            const auto& el_p = get_p_edge_l(el_env);

            for (const auto& er_env : right->out(sr))
              {
                // if they can be taken at the same time, the output
                // of r must implies the one of left
                if (bdd_have_common_assignment(el_env.cond, er_env.cond))
                  {
                    const auto& er_p = get_p_edge_r(er_env);
                    if (!bdd_implies(er_p.cond, el_p.cond))
                      {
                        if (verbose)
                          std::cerr << "left " << el_env.src << " to "
                                    << el_env.dst << " and right "
                                    << er_env.src << " to " << er_env.dst
                                    << " have common letter "
                                    << (el_env.cond & er_env.cond) << " but "
                                    << er_p.cond << " does not imply "
                                    << el_p.cond << std::endl;
                        return false;
                      }


                    // Check if the new dst pair was already visited
                    assert(spl[el_p.src] && !spl[el_p.dst]);
                    assert(spr[er_p.src] && !spr[er_p.dst]);
                    auto [itdst, inserted] = seen.emplace(el_p.dst,
                                                          er_p.dst);
                    if (inserted)
                      todo.push_back(*itdst);
                  }
              }
          }
      }
    return true;
  }

  twa_graph_ptr
  mealy_product(const const_twa_graph_ptr& left,
                const const_twa_graph_ptr& right)
  {
    bdd outs[] = {get_synthesis_outputs(left),
                  get_synthesis_outputs(right)};

#ifndef NDEBUG
    for (const auto& [m, n, o] : {std::tuple{left, "left", outs[0]},
                                            {right, "right", outs[1]}})
      {
        if (!is_mealy(m))
          throw std::runtime_error(std::string("mealy_prod(): ") + n
                                  + " is not a mealy machine");
        if (!is_complete_(m, o))
          throw std::runtime_error(std::string("mealy_prod(): ") + n
                                   + " is not input complete");
      }
#endif

    auto p = product(left, right);
    bdd pouts = outs[0] & outs[1];
    set_synthesis_outputs(p, pouts);

#ifndef NDEBUG
    if (!is_mealy(p))
      throw std::runtime_error("mealy_prod(): Prooduct is not mealy");
    if (!is_complete_(p, pouts))
      throw std::runtime_error("mealy_prod(): Prooduct is not input complete. "
                               "Incompatible machines?");
#endif

    return p;
  }


  void
  simplify_mealy_here(twa_graph_ptr& m, int minimize_lvl,
                      bool split_out)
  {
    auto si = synthesis_info();
    si.minimize_lvl = minimize_lvl;
    return simplify_mealy_here(m, si, split_out);
  }

  void
  simplify_mealy_here(twa_graph_ptr& m, synthesis_info& si,
                      bool split_out)
  {
    const auto minimize_lvl = si.minimize_lvl;
    assert(is_mealy(m)
           && "simplify_mealy_here(): m is not a mealy machine!");
    if (minimize_lvl < 0 || 5 < minimize_lvl)
      throw std::runtime_error("simplify_mealy_here(): minimize_lvl "
                                "must be between 0 and 5.");

    stopwatch sw;
    if (si.bv)
      sw.start();

    bool is_separated = false;
    if (0 < minimize_lvl && minimize_lvl < 3)
      {
        // unsplit if necessary
        if (m->get_named_prop<region_t>("state-player"))
          {
            m = unsplit_mealy(m);
            is_separated = true;
          }
        reduce_mealy_here(m, minimize_lvl == 2);
      }
    else if (3 <= minimize_lvl)
      m = minimize_mealy(m, si);

    // Convert to demanded output format
    bool is_split = m->get_named_prop<region_t>("state-player");
    if (minimize_lvl == 0)
      {
        if (is_split && !split_out)
          m = unsplit_mealy(m);
        else if (!is_split && split_out)
          m = split_2step(m, false);
      }
    else if (0 < minimize_lvl && minimize_lvl < 3 && split_out)
      {
      if (is_separated)
        split_separated_mealy_here(m);
      else
        m = split_2step(m, false);
      }
    else if (3 <= minimize_lvl && !split_out)
      m = unsplit_mealy(m);

    if (si.bv)
      {
        if (si.verbose_stream)
          *si.verbose_stream << "simplification took " << sw.stop()
                             << " seconds\n";
        si.bv->simplify_strat_time += sw.stop();
        auto n_s_env = 0u;
        auto n_e_env = 0u;
        if (auto sp = m->get_named_prop<region_t>("state-player"))
          {
            n_s_env = sp->size() - std::accumulate(sp->begin(),
                                                   sp->end(),
                                                   0u);
            std::for_each(m->edges().begin(), m->edges().end(),
                          [&n_e_env, &sp](const auto& e)
                            {
                              n_e_env += (*sp)[e.src];
                        });
          }
        else
          {
            n_s_env = m->num_states();
            n_e_env = m->num_edges();
          }
        si.bv->nb_simpl_strat_states += n_s_env;
        si.bv->nb_simpl_strat_edges += n_e_env;
      }
  }
}
