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
#include <spot/twa/twagraph.hh>
#include <spot/gen/automata.hh>
#include <spot/misc/clz.hh>

namespace spot
{
  namespace gen
  {
    namespace
    {
      static twa_graph_ptr
      ks_nca(unsigned n, bdd_dict_ptr dict)
      {
        if (n == 0)
          throw std::runtime_error("ks-nca expects a positive argument");
        // the alphabet has four letters:
        // i, s (for sigma), p (for pi), h (for hash)
        // we encode this four letters alphabet thanks to two AP a and b
        // the exact encoding is not important
        // each letter is a permutation of the set {1..2n}
        // s = (1 2 .. 2n) the rotation
        // p = (1 2) the swap of the first two elements
        // i is the identity
        // d is the identity on {2..2n} but is undefined on 1

        // the automaton has 2n+1 states, numbered from 0 to 2n
        // 0 is the initial state and the only non-deterministic state

        auto aut = make_twa_graph(dict);

        // register aps
        bdd a = bdd_ithvar(aut->register_ap("a"));
        bdd b = bdd_ithvar(aut->register_ap("b"));

        // name the four letters
        bdd i = a & b;
        bdd s = a & (!b);
        bdd p = (!a) & b;
        bdd h = (!a) & (!b);

        // actually build the automaton
        aut->new_states(2*n+1);
        aut->set_init_state(0);
        aut->set_acceptance(1, acc_cond::acc_code::cobuchi());

        // from 0, we can non-deterministically jump to any state
        // (except 0) with any letter.
        for (unsigned q = 1; q <= 2*n; ++q)
          aut->new_edge(0, q, bddtrue, {0});
        // i is the identity
        for (unsigned q = 1; q <= 2*n; ++q)
          aut->new_edge(q, q, i);
        // p swaps 1 and 2, and leaves all other states invariant
        aut->new_edge(1, 2, p);
        aut->new_edge(2, 1, p);
        for (unsigned q = 3; q <= 2*n; ++q)
          aut->new_edge(q, q, p);
        // s goes to next state (mod 2*n, 0 excluded)
        aut->new_edge(2*n, 1, s);
        for (unsigned q = 1; q < 2*n; ++q)
          aut->new_edge(q, q+1, s);
        // h is the same as i, except on 1 where it goes back to the
        // initial state
        aut->new_edge(1, 0, h);
        for (unsigned q = 2; q <= 2*n; ++q)
          aut->new_edge(q, q, h);

        aut->merge_edges();
        aut->prop_state_acc(true);
        aut->prop_universal(false);
        aut->prop_complete(true);
        aut->prop_inherently_weak(false);
        aut->prop_stutter_invariant(false);
        aut->prop_semi_deterministic(false);
        return aut;
      }

      static twa_graph_ptr
      l_nba(unsigned n, bdd_dict_ptr dict)
      {
        if (n == 0)
          throw std::runtime_error("l-nba expects a positive argument");

        auto aut = make_twa_graph(dict);

        bdd p1 = bdd_ithvar(aut->register_ap("a"));
        bdd p2 = bdd_ithvar(aut->register_ap("b"));
        bdd a = p1 - p2;
        bdd b = p2 - p1;
        bdd s = p1 & p2;
        bdd all = p1 | p2;

        aut->new_states(3 * n + 1);
        aut->set_init_state(1);
        aut->set_buchi();

        aut->new_acc_edge(0, n + 1, a);
        aut->new_edge(2 * n + 1, 0, b);
        for (unsigned s = 1; s <= n; ++s)
          {
            aut->new_edge(s + n, std::min(s + n + 1, 2 * n), a);
            aut->new_edge(s + n, s, b);
            aut->new_edge(s, s, all);
            aut->new_edge(s, s + 2 * n, a);
            aut->new_edge(std::min(s + 2 * n + 1, 3 * n), s + 2 * n, a);
          }

        aut->prop_state_acc(true);
        aut->prop_universal(false);
        aut->prop_complete(false);
        aut->prop_inherently_weak(false);
        aut->prop_stutter_invariant(false);
        aut->prop_semi_deterministic(false);
        return aut;
      }

      static twa_graph_ptr
      l_dsa(unsigned n, bdd_dict_ptr dict)
      {
        if (n < 1 || n > 16)
          throw std::runtime_error("l-dsa expects 1 <= n <= 16");

        auto aut = make_twa_graph(dict);

        bdd a = bdd_ithvar(aut->register_ap("a"));
        bdd b = !a;

        aut->set_acceptance(2 * n, acc_cond::acc_code::streett(n));
        aut->new_states(4 * n);
        aut->set_init_state(0);

        for (unsigned s = 0; s < n; ++s)
          {
            unsigned col1 = 4 * s;
            unsigned col2 = 4 * s + 1;
            unsigned col3 = 4 * s + 2;
            unsigned col4 = 4 * s + 3;
            aut->new_edge(col1, 2, a, {2 * s});
            aut->new_edge(col2, col1, b);
            aut->new_edge(col2, std::min(col2 + 4, 4 * n - 3), a);
            aut->new_edge(col3, col4, b);
            aut->new_edge(col3, std::min(col3 + 4, 4 * n - 2), a);
            aut->new_edge(col4, 1, a, {2 * s + 1});
          }

        aut->prop_state_acc(true);
        aut->prop_universal(true);
        aut->prop_complete(false);
        aut->prop_inherently_weak(false);
        aut->prop_stutter_invariant(false);
        aut->prop_semi_deterministic(true);
        return aut;
      }
    }

    // Return the smallest integer k such that 2^k ≥ n > 1.
    static unsigned ulog2(unsigned n)
    {
      assert(n>1); // clz() is undefined for n==0
      --n;
      return CHAR_BIT*sizeof(unsigned) - clz(n);
    }

    static twa_graph_ptr
    m_nba(unsigned n, bdd_dict_ptr dict)
    {
      if (n == 0)
        throw std::runtime_error
          ("l-nba expects a positive argument");

      auto aut = make_twa_graph(dict);
      aut->set_buchi();
      aut->new_states(n + 1);
      aut->set_init_state(0);

      // How many AP to we need to represent n+1 letters
      unsigned nap = ulog2(n + 1);
      std::vector<int> apvars(nap);
      for (unsigned a = 0; a < nap; ++a)
        apvars[a] = aut->register_ap("p" + std::to_string(a));

      bdd all = bdd_ibuildcube(0, nap, apvars.data());
      for (unsigned letter = n; letter > 0; --letter)
        {
          bdd cond = bdd_ibuildcube(letter, nap, apvars.data());
          aut->new_acc_edge(0, letter, cond);
          aut->new_edge(letter, 0, cond);
          all |= cond;
        }
      for (unsigned letter = n; letter > 0; --letter)
        aut->new_edge(letter, letter, all);

      aut->prop_state_acc(true);
      aut->prop_universal(false);
      aut->prop_complete(false);
      aut->prop_inherently_weak(false);
      aut->prop_stutter_invariant(false);
      aut->prop_semi_deterministic(false);

      return aut;
    }

    static twa_graph_ptr
    cyclist_trace_or_proof(unsigned n, bool trace, bdd_dict_ptr dict)
    {
      auto aut = make_twa_graph(dict);
      acc_cond::mark_t m = aut->set_buchi();
      aut->new_states(n + 2);
      aut->set_init_state(0);
      if (trace)
          m = {};
      aut->prop_state_acc(true);

      // How many AP to we need to represent n+1 letters
      unsigned nap = ulog2(n + 1);
      std::vector<int> apvars(nap);
      for (unsigned a = 0; a < nap; ++a)
        apvars[a] = aut->register_ap("p" + std::to_string(a));

      if (trace)
        aut->new_edge(0, 0, bddtrue); // the only non-deterministic edge
      else
        aut->prop_universal(true);

      bdd zero = bdd_ibuildcube(0, nap, apvars.data());
      aut->new_edge(0, 1, zero, m);
      for (unsigned letter = 1; letter <= n; ++letter)
        {
          bdd cond = bdd_ibuildcube(letter, nap, apvars.data());
          aut->new_acc_edge(1, letter + 1, cond);
          aut->new_edge(letter + 1, 1, zero, m);
        }

      return aut;
    }

    static twa_graph_ptr
    cycle_nba(unsigned n, bool onehot, bdd_dict_ptr dict)
    {
      if (n == 0)
        throw std::runtime_error
          (onehot
           ? "cycle-onehot-nba expects a positive argument"
           : "cycle-log-nba expects a positive argument");

      auto aut = make_twa_graph(dict);
      acc_cond::mark_t isacc = aut->set_buchi();
      aut->new_states(n * n);
      aut->set_init_state(0);
      aut->prop_state_acc(true);
      aut->prop_weak(n == 1);
      aut->prop_universal(n == 1);
      aut->prop_complete(false);

      std::vector<bdd> letters;
      letters.reserve(n);

      if (!onehot)
        {
          // How many AP to we need to represent n letters
          unsigned nap = n == 1 ? 1 : ulog2(n);
          std::vector<int> apvars(nap);
          for (unsigned a = 0; a < nap; ++a)
            apvars[a] = aut->register_ap("p" + std::to_string(a));

          for (unsigned letter = 0; letter < n; ++letter)
            {
              bdd cond = bdd_ibuildcube(letter, nap, apvars.data());
              letters.push_back(cond);
            }
        }
      else
        {
          std::vector<bdd> apvars(n);
          bdd allneg = bddtrue;
          for (unsigned a = 0; a < n; ++a)
            {
              int v = aut->register_ap("p" + std::to_string(a));
              apvars[a] = bdd_ithvar(v);
              allneg &= bdd_nithvar(v);
            }
          for (unsigned a = 0; a < n; ++a)
            letters.push_back(bdd_exist(allneg, apvars[a]) & apvars[a]);
        }

      unsigned n2 = n * n;
      for (unsigned s = 0; s < n; ++s)
        {
          bdd label = letters[s];
          for (unsigned copy = 0; copy < n; ++copy)
            {
              unsigned q = s + copy * n;
              if (s != 0)
                {
                  aut->new_edge(q, q, bddtrue);
                  aut->new_edge(q, (q + 1) % n2, label);
                }
              else
                {
                  aut->new_edge(q, (q + 1) % n2, label, isacc);
                }
            }
        }
      return aut;
    }


    twa_graph_ptr aut_pattern(aut_pattern_id pattern, int n, bdd_dict_ptr dict)
    {
      if (n < 0)
        {
          std::ostringstream err;
          err << "pattern argument for " << aut_pattern_name(pattern)
              << " should be non-negative";
          throw std::runtime_error(err.str());
        }

      switch (pattern)
        {
          // Keep this alphabetically-ordered!
        case AUT_KS_NCA:
          return ks_nca(n, dict);
        case AUT_L_NBA:
          return l_nba(n, dict);
        case AUT_L_DSA:
          return l_dsa(n, dict);
        case AUT_M_NBA:
          return m_nba(n, dict);
        case AUT_CYCLIST_TRACE_NBA:
          return cyclist_trace_or_proof(n, true, dict);
        case AUT_CYCLIST_PROOF_DBA:
          return cyclist_trace_or_proof(n, false, dict);
        case AUT_CYCLE_LOG_NBA:
          return cycle_nba(n, false, dict);
        case AUT_CYCLE_ONEHOT_NBA:
          return cycle_nba(n, true, dict);
        case AUT_END:
          break;
        }
      throw std::runtime_error("unsupported pattern");
    }

    const char* aut_pattern_name(aut_pattern_id pattern)
    {
      static const char* const class_name[] =
        {
          "ks-nca",
          "l-nba",
          "l-dsa",
          "m-nba",
          "cyclist-trace-nba",
          "cyclist-proof-dba",
          "cycle-log-nba",
          "cycle-onehot-nba",
        };
      // Make sure we do not forget to update the above table every
      // time a new pattern is added.
      static_assert(sizeof(class_name)/sizeof(*class_name)
                    == AUT_END - AUT_BEGIN, "size mismatch");
      if (pattern < AUT_BEGIN || pattern >= AUT_END)
        throw std::runtime_error("unsupported pattern");
      return class_name[pattern - AUT_BEGIN];
    }
  }
}
