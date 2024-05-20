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

#include <spot/twaalgos/dbranch.hh>
#include <spot/priv/robin_hood.hh>
#include <utility>
#include <stack>

namespace spot
{
  namespace
  {
    typedef std::pair<bdd, acc_cond::mark_t> bdd_color;

    struct bdd_color_hash
    {
      size_t
      operator()(const bdd_color& bc) const noexcept
      {
        return bc.first.id() ^ bc.second.hash();
      }
    };

    template<bool is_game>
    bool delay_branching_aux(const twa_graph_ptr& aut, std::vector<bool>* owner)
    {
      unsigned ns = aut->num_states();
      // number of predecessors of each state
      std::vector<unsigned> pred_count(ns, 0);
      unsigned init = aut->get_init_state_number();
      pred_count[init] = 2; // pretend the initial state has too many
                            // predecessors, so it does not get fused.
      // for each state, number of successors that have a single predecessors
      std::vector<unsigned> succ_cand(ns, 0);
      for (auto& e: aut->edges())
        for (unsigned d: aut->univ_dests(e))
          {
            // Note that e.dst might be a destination group in
            // alternating automata.
            unsigned pc = ++pred_count[d];
            succ_cand[e.src] += (pc == 1) - (pc == 2);
          }
      bool changed = false;
      typedef robin_hood::unordered_map<bdd_color, unsigned,
                                        bdd_color_hash> hashmap_t;
      hashmap_t first_dest[1 + is_game];
      auto& g = aut->get_graph();

      // Merging outgoing transitions may cause the automaton to need
      // transition-based acceptance.
      bool need_trans = !aut->prop_state_acc().is_true();

      // setup a DFS
      std::vector<bool> seen(ns);
      std::stack<unsigned> todo;
      auto push_state = [&](unsigned state)
      {
        todo.push(state);
        seen[state] = true;
      };
      push_state(init);

      while (!todo.empty())
        {
          unsigned src = todo.top();
          todo.pop();
          if (succ_cand[src] < 2) // nothing to merge
            {
              for (auto& e: aut->out(src))
                for (unsigned d: aut->univ_dests(e))
                  if (!seen[d])
                    push_state(d);
              continue;
            }
          first_dest[0].clear();
          if constexpr (is_game)
            first_dest[1].clear();
          auto it = g.out_iteraser(src);
          while (it)
            {
              unsigned canddst = it->dst;
              for (unsigned d: aut->univ_dests(canddst))
                if (!seen[d])
                  push_state(d);
              if (aut->is_univ_dest(canddst) || pred_count[canddst] != 1)
                {
                  ++it;
                  continue;
                }
              if (it->cond == bddfalse)
                {
                  it.erase();
                  continue;
                }
              unsigned mapidx = is_game ? (*owner)[canddst] : 0;
              auto [it2, inserted] =
                first_dest[mapidx].emplace(bdd_color{it->cond, it->acc},
                                           canddst);
              if (inserted)
                {
                  ++it;
                  continue;
                }
              unsigned mergedst = it2->second;
              // we have to merge canddst into mergedst.
              // This is as simple as:
              // 1) connecting their list of transitions
              unsigned& candlast = g.state_storage(canddst).succ_tail;
              if (candlast)
                {
                  unsigned& mergedfirst = g.state_storage(mergedst).succ;
                  unsigned& mergedlast = g.state_storage(mergedst).succ_tail;
                  unsigned& candfirst = g.state_storage(canddst).succ;
                  if (mergedlast)
                    {
                      aut->edge_storage(mergedlast).next_succ = candfirst;
                      // Do we need to require transition-based acceptance?
                      if (!need_trans)
                        need_trans =
                          (aut->edge_storage(candfirst).acc
                           != aut->edge_storage(mergedfirst).acc);
                    }
                  else              // mergedst had no successor
                    {
                      mergedfirst = candfirst;
                    }
                  mergedlast = candlast;
                  // 2) updating the source of the merged transitions
                  for (unsigned e2 = candfirst; e2 != 0;)
                    {
                      auto& edge = aut->edge_storage(e2);
                      edge.src = mergedst;
                      e2 = edge.next_succ;
                    }
                  // 3) deleting the edge to canddst.
                  candfirst = candlast = 0;
                }
              it.erase();
              // 4) updating succ_cand
              succ_cand[mergedst] += succ_cand[canddst];
              succ_cand[canddst] = 0;
              changed = true;
            }
        }
      if (need_trans)
        aut->prop_state_acc(false);
      return changed;
    }
  }

  bool delay_branching_here(const twa_graph_ptr& aut)
  {
    if (aut->prop_universal())
      return false;
    auto owner = aut->get_named_prop<std::vector<bool>>("state-player");
    if (SPOT_UNLIKELY(owner))
      return delay_branching_aux<true>(aut, owner);
    else
      return delay_branching_aux<false>(aut, nullptr);
  }
}
