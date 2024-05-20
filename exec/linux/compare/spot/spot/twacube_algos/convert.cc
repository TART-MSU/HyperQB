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
#include <spot/twacube_algos/convert.hh>
#include <spot/twaalgos/contains.hh>
#include <cassert>

namespace spot
{
  spot::cube satone_to_cube(bdd one, cubeset& cubeset,
                            std::unordered_map<int, int>& binder)
  {
    auto cube = cubeset.alloc();
    while (one != bddtrue)
      {
        if (bdd_high(one) == bddfalse)
          {
            assert(binder.find(bdd_var(one)) != binder.end());
            cubeset.set_false_var(cube, binder[bdd_var(one)]);
            one = bdd_low(one);
          }
        else
          {
            assert(binder.find(bdd_var(one)) != binder.end());
            cubeset.set_true_var(cube, binder[bdd_var(one)]);
            one = bdd_high(one);
          }
      }
    return cube;
  }

  bdd cube_to_bdd(spot::cube cube, const cubeset& cubeset,
                  std::unordered_map<int, int>& reverse_binder)
  {
    bdd result = bddtrue;
    for (unsigned int i = 0; i < cubeset.size(); ++i)
      {
        assert(reverse_binder.find(i) != reverse_binder.end());
        if (cubeset.is_false_var(cube, i))
          result &= bdd_nithvar(reverse_binder[i]);
        if (cubeset.is_true_var(cube, i))
          result &= bdd_ithvar(reverse_binder[i]);
      }
    return result;
  }

  spot::twacube_ptr twa_to_twacube(const spot::const_twa_graph_ptr aut)
  {
    if (aut == nullptr)
      return nullptr;

    // Compute the necessary binder and extract atomic propositions
    std::unordered_map<int, int> ap_binder;
    std::vector<std::string>* aps = extract_aps(aut, ap_binder);

    // Declare the twa cube
    auto tg =  spot::make_twacube(*aps);

    // Fix acceptance
    tg->acc() = aut->acc();

    // This binder maps twagraph indexes to twacube ones.
    std::unordered_map<int, int> st_binder;

    // Fill binder and create corresponding states into twacube
    for (unsigned n = 0; n < aut->num_states(); ++n)
      st_binder.insert({n, tg->new_state()});

    // Fix the initial state
    tg->set_initial(st_binder[aut->get_init_state_number()]);

    // Get the cubeset
    auto cs = tg->get_cubeset();

    // Now just build all transitions of this automaton
    // spot::cube cube(aps);
    for (unsigned n = 0; n < aut->num_states(); ++n)
      for (auto& t: aut->out(n))
        {
          bdd cond = t.cond;

          // Special case for bddfalse
          if (cond == bddfalse)
            continue;

          // Split the bdd into multiple transitions
          while (cond != bddfalse)
            {
              bdd one = bdd_satone(cond);
              cond -= one;
              spot::cube cube =spot::satone_to_cube(one, cs, ap_binder);
              tg->create_transition(st_binder[n], cube, t.acc,
                                    st_binder[t.dst]);
            }
        }
    // Must be contiguous to support swarming.
    assert(tg->succ_contiguous());
    delete aps;
    return tg;
  }

  std::vector<std::string>*
  extract_aps(const spot::const_twa_graph_ptr aut,
              std::unordered_map<int, int>& ap_binder)
  {
    std::vector<std::string>* aps = new std::vector<std::string>();
    for (auto f: aut->ap())
      {
        int size = aps->size();
        if (std::find(aps->begin(), aps->end(), f.ap_name()) == aps->end())
          {
            aps->push_back(f.ap_name());
            ap_binder.insert({aut->get_dict()->var_map[f], size});
          }
      }
    return aps;
  }

  spot::twa_graph_ptr
  twacube_to_twa(spot::twacube_ptr twacube, spot::bdd_dict_ptr d)
  {
    // Grab necessary variables
    auto& theg = twacube->get_graph();
    spot::cubeset cs = twacube->get_cubeset();

    // Build the resulting graph
    if (d == nullptr)
      d = spot::make_bdd_dict();
    auto res = make_twa_graph(d);

    // Fix the acceptance of the resulting automaton
    res->acc() = twacube->acc();

    // Grep bdd id for each atomic propositions
    std::vector<int> bdds_ref;
    for (auto& ap : twacube->ap())
      bdds_ref.push_back(res->register_ap(ap));

    // Build all resulting states
    for (unsigned int i = 0; i < theg.num_states(); ++i)
      {
        unsigned st = res->new_state();
        (void) st;
        assert(st == i);
      }

    // Build all resulting conditions.
    for (unsigned int i = 1; i <= theg.num_edges(); ++i)
      {
        bdd cond = bddtrue;
        for (unsigned j = 0; j < cs.size(); ++j)
          {
            if (cs.is_true_var(theg.edge_data(i).cube_, j))
              cond &= bdd_ithvar(bdds_ref[j]);
            else if (cs.is_false_var(theg.edge_data(i).cube_, j))
              cond &= bdd_nithvar(bdds_ref[j]);
            // otherwise it 's a free variable do nothing
          }

        res->new_edge(theg.edge_storage(i).src, theg.edge_storage(i).dst,
                      cond, theg.edge_data(i).acc_);
      }

    // Fix the initial state
    res->set_init_state(twacube->get_initial());

    return res;
  }

  bool are_equivalent(const spot::twacube_ptr twacube,
                      const spot::const_twa_graph_ptr twa)
  {
    // Compute the necessary binder and extract atomic propositions
    std::unordered_map<int, int> ap_binder;
    std::vector<std::string>* aps_twa = extract_aps(twa, ap_binder);
    std::vector<std::string> aps_twacube = twacube->ap();

    // Comparator to compare two strings in case insensitive manner
    auto comparator = [](const std::string& lhs, const std::string& rhs) {
      return lhs.compare(rhs) == 0;
    };

    // Error. Not working on the same set of aps.
    if (aps_twa->size() != aps_twacube.size() ||
        !std::equal(aps_twa->begin(), aps_twa->end(),
                   aps_twacube.begin(), comparator))
      throw std::runtime_error("are_equivalent: not working on the same "
                               "atomic propositions");

    auto res = twacube_to_twa(twacube, twa->get_dict());

    bool result =  are_equivalent(res, twa);
    delete aps_twa;
    return result;
  }
}
