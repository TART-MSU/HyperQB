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
#include <map>
#include <spot/tl/apcollect.hh>
#include <spot/twa/twagraph.hh>
#include <spot/twa/bdddict.hh>
#include <spot/twaalgos/game.hh>
#include <spot/twaalgos/sccinfo.hh>
#include <spot/tl/relabel.hh>
#include <spot/priv/robin_hood.hh>

namespace spot
{
  atomic_prop_set create_atomic_prop_set(unsigned n)
  {
    atomic_prop_set res;
    for (unsigned i = 0; i < n; ++i)
      {
        std::ostringstream p;
        p << 'p' << i;
        res.insert(formula::ap(p.str()));
      }
    return res;
  }

  atomic_prop_set*
  atomic_prop_collect(formula f, atomic_prop_set* s)
  {
    if (!s)
      s = new atomic_prop_set;
    f.traverse([&](const formula& f)
               {
                 if (f.is(op::ap))
                   s->insert(f);
                 return false;
               });
    return s;
  }

  bdd
  atomic_prop_collect_as_bdd(formula f, const twa_ptr& a)
  {
    spot::atomic_prop_set aps;
    atomic_prop_collect(f, &aps);
    bdd res = bddtrue;
    for (auto f: aps)
      res &= bdd_ithvar(a->register_ap(f));
    return res;
  }

  atomic_prop_set collect_literals(formula f)
  {
    atomic_prop_set res;

    // polarity: 0 = negative, 1 = positive, 2 or more = both.
    auto rec = [&res](formula f, unsigned polarity, auto self)
    {
      switch (f.kind())
        {
        case op::ff:
        case op::tt:
        case op::eword:
          return;
        case op::ap:
          if (polarity != 0)
            res.insert(f);
          if (polarity != 1)
            res.insert(formula::Not(f));
          return;
        case op::Not:
        case op::NegClosure:
        case op::NegClosureMarked:
          self(f[0], polarity ^ 1, self);
          return;
        case op::Xor:
        case op::Equiv:
          self(f[0], 2, self);
          self(f[1], 2, self);
          return;
        case op::Implies:
        case op::UConcat:
          self(f[0], polarity ^ 1, self);
          self(f[1], polarity, self);
          return;
        case op::U:
        case op::R:
        case op::W:
        case op::M:
        case op::EConcat:
        case op::EConcatMarked:
          self(f[0], polarity, self);
          self(f[1], polarity, self);
          return;
        case op::X:
        case op::F:
        case op::G:
        case op::Closure:
        case op::Or:
        case op::OrRat:
        case op::And:
        case op::AndRat:
        case op::AndNLM:
        case op::Concat:
        case op::Fusion:
        case op::Star:
        case op::FStar:
        case op::first_match:
        case op::strong_X:
          for (formula c: f)
            self(c, polarity, self);
          return;
        }
    };
    rec(f, 1, rec);
    return res;
  }

  std::vector<std::vector<spot::formula>>
  collect_equivalent_literals(formula f)
  {
    std::map<spot::formula, unsigned> l2s;
    // represent the implication graph as a twa_graph so we cab reuse
    // scc_info.  Literals can be converted to states using the l2s
    // map.
    twa_graph_ptr igraph = make_twa_graph(make_bdd_dict());

    auto state_of = [&](formula a)
    {
      auto [it, b] = l2s.insert({a, 0});
      if (b)
        it->second = igraph->new_state();
      return it->second;
    };

    auto implies = [&](formula a, formula b)
    {
      unsigned pos_a = state_of(a);
      unsigned neg_a = state_of(formula::Not(a));
      unsigned pos_b = state_of(b);
      unsigned neg_b = state_of(formula::Not(b));
      igraph->new_edge(pos_a, pos_b, bddtrue);
      igraph->new_edge(neg_b, neg_a, bddtrue);
    };

    auto collect = [&](formula f, bool in_g, auto self)
    {
      switch (f.kind())
        {
        case op::ff:
        case op::tt:
        case op::eword:
        case op::ap:
        case op::UConcat:
        case op::Not:
        case op::NegClosure:
        case op::NegClosureMarked:
        case op::U:
        case op::R:
        case op::W:
        case op::M:
        case op::EConcat:
        case op::EConcatMarked:
        case op::X:
        case op::F:
        case op::Closure:
        case op::OrRat:
        case op::AndRat:
        case op::AndNLM:
        case op::Concat:
        case op::Fusion:
        case op::Star:
        case op::FStar:
        case op::first_match:
        case op::strong_X:
          return;
        case op::Xor:
          if (in_g && f[0].is_literal() && f[1].is_literal())
            {
              implies(f[0], formula::Not(f[1]));
              implies(formula::Not(f[0]), f[1]);
            }
          return;
        case op::Equiv:
          if (in_g && f[0].is_literal() && f[1].is_literal())
            {
              implies(f[0], f[1]);
              implies(formula::Not(f[0]), formula::Not(f[1]));
            }
          return;
        case op::Implies:
          if (in_g && f[0].is_literal() && f[1].is_literal())
            implies(f[0], f[1]);
          return;
        case op::G:
          self(f[0], true, self);
          return;
        case op::Or:
          if (f.size() == 2 && f[0].is_literal() && f[1].is_literal())
            implies(formula::Not(f[0]), f[1]);
          return;
        case op::And:
          for (formula c: f)
            self(c, in_g, self);
          return;
        }
    };
    collect(f, false, collect);

    scc_info si(igraph, scc_info_options::PROCESS_UNREACHABLE_STATES);

    // print_hoa(std::cerr, igraph);

    // Build sets of equivalent literals.
    unsigned nscc = si.scc_count();
    std::vector<std::vector<spot::formula>> scc(nscc);
    for (auto [f, i]: l2s)
      scc[si.scc_of(i)].push_back(f);

    // For each set, we will decide if we keep it or not.
    std::vector<bool> keep(nscc, true);

    for (unsigned i = 0; i < nscc; ++i)
      {
        if (keep[i] == false)
          continue;
        // We don't keep trivial SCCs
        if (scc[i].size() <= 1)
          {
            keep[i] = false;
            continue;
          }
        // Each SCC will appear twice.  Because if {a,!b,c,!d,!e} are
        // equivalent literals, then so are {!a,b,!c,d,e}.   We will
        // keep the SCC with the fewer negation if we can.
        unsigned neg_count = 0;
        for (formula f: scc[i])
          {
            SPOT_ASSUME(f != nullptr);
            neg_count += f.is(op::Not);
          }
        if (neg_count > scc[i].size()/2)
          {
            keep[i] = false;
            continue;
          }
        // We will keep the current SCC.  Just
        // mark the dual one for removal.
        keep[si.scc_of(state_of(formula::Not(*scc[i].begin())))] = false;
      }
    // purge unwanted SCCs
    unsigned j = 0;
    for (unsigned i = 0; i < nscc; ++i)
      {
        if (keep[i] == false)
          continue;
        if (i > j)
          scc[j] = std::move(scc[i]);
        ++j;
      }
    scc.resize(j);
    return scc;
  }

  realizability_simplifier::realizability_simplifier
  (formula f, const std::vector<std::string>& inputs,
   unsigned options, std::ostream* verbose)
  {
    bool first_mapping = true;
    relabeling_map rm;
    auto add_to_mapping = [&](formula from, bool from_is_input, formula to)
    {
      mapping_.emplace_back(from, from_is_input, to);
      rm[from] = to;
      if (SPOT_LIKELY(!verbose))
        return;
      if (first_mapping)
        {
          *verbose << "the following signals can be temporarily removed:\n";
          first_mapping = false;
        }
      *verbose << "  " << from << " := " << to <<'\n';
    };
    global_equiv_output_only_ =
      (options & global_equiv_output_only) == global_equiv_output_only;

    robin_hood::unordered_set<spot::formula> ap_inputs;
    for (const std::string& ap: inputs)
      ap_inputs.insert(spot::formula::ap(ap));

    formula oldf;
    f_ = f;
    do
      {
        bool rm_has_new_terms = false;
        oldf = f_;

        if (options & polarity)
          {
            // Check if some output propositions are always in
            // positive form, or always in negative form.  In
            // syntcomp, this occurs more frequently for input
            // variables than output variable.  See issue #529 for
            // some examples.
            std::set<spot::formula> lits = spot::collect_literals(f_);
            for (const formula& lit: lits)
              if (lits.find(spot::formula::Not(lit)) == lits.end())
                {
                  formula ap = lit;
                  bool neg = false;
                  if (lit.is(op::Not))
                    {
                      ap = lit[0];
                      neg = true;
                    }
                  bool is_input = ap_inputs.find(ap) != ap_inputs.end();
                  formula to = (is_input == neg)
                    ? spot::formula::tt() : spot::formula::ff();
                  add_to_mapping(ap, is_input, to);
                  rm_has_new_terms = true;
                }
            if (rm_has_new_terms)
              {
                f_ = spot::relabel_apply(f_, &rm);
                if (verbose)
                  *verbose << "new formula: " << f_ << '\n';
                rm_has_new_terms = false;
              }
          }
        if (options & global_equiv)
          {
            // check for equivalent terms
            spot::formula_ptr_less_than_bool_first cmp;
            for (std::vector<spot::formula>& equiv:
                   spot::collect_equivalent_literals(f_))
              {
                // For each set of equivalent literals, we want to
                // pick a representative.  That representative
                // should be an input if one of the literal is an
                // input.  (If we have two inputs or more, the
                // formula is not realizable.)
                spot::formula repr = nullptr;
                bool repr_is_input = false;
                spot::formula input_seen = nullptr;
                for (spot::formula lit: equiv)
                  {
                    spot::formula ap = lit;
                    if (ap.is(spot::op::Not))
                      ap = ap[0];
                    if (ap_inputs.find(ap) != ap_inputs.end())
                      {
                        if (input_seen)
                          {
                            // ouch! we have two equivalent inputs.
                            // This means the formula is simply
                            // unrealizable.  Make it false for the
                            // rest of the algorithm.
                            f = spot::formula::ff();
                            return;
                          }
                        input_seen = lit;
                        // Normally, we want the input to be the
                        // representative.  However as a special
                        // case, we ignore the input literal from
                        // the set if we are asked to print a
                        // game.  Fixing the game to add a i<->o
                        // equivalence would require more code
                        // than I care to write.
                        //
                        // So if the set was {i,o1,o2}, instead
                        // of the desirable
                        //     o1 := i
                        //     o2 := i
                        // we only do
                        //     o2 := o1
                        // when printing games.
                        if (!global_equiv_output_only_)
                          {
                            repr_is_input = true;
                            repr = lit;
                          }
                      }
                    else if (!repr_is_input && (!repr || cmp(ap, repr)))
                      repr = lit;
                  }
                // now map equivalent each atomic proposition to the
                // representative
                spot::formula not_repr = spot::formula::Not(repr);
                for (spot::formula lit: equiv)
                  {
                    // input or representative are not removed
                    // (we have repr != input_seen either when input_seen
                    // is nullptr, or if want_game is true)
                    if (lit == repr || lit == input_seen)
                      continue;
                    SPOT_ASSUME(lit != nullptr);
                    if (lit.is(spot::op::Not))
                      add_to_mapping(lit[0], false, not_repr);
                    else
                      add_to_mapping(lit, false, repr);
                    rm_has_new_terms = true;
                  }
              }
            if (rm_has_new_terms)
              {
                f_ = spot::relabel_apply(f_, &rm);
                if (verbose)
                  *verbose << "new formula: " << f_ << '\n';
                rm_has_new_terms = false;
              }
          }
      }
    while (oldf != f_);
  }

  void
  realizability_simplifier::merge_mapping(const realizability_simplifier& other)
  {
    for (auto [from, from_is_input, to]: other.get_mapping())
      mapping_.emplace_back(from, from_is_input, to);
  }

  void realizability_simplifier::patch_mealy(twa_graph_ptr mealy) const
  {
    bdd add = bddtrue;
    bdd additional_outputs = bddtrue;
    for (auto [k, k_is_input, v]: mapping_)
      {
        int i = mealy->register_ap(k);
        // skip inputs (they are don't care)
        if (k_is_input)
          continue;
        if (v.is_tt())
          {
            bdd bv = bdd_ithvar(i);
            additional_outputs &= bv;
            add &= bv;
          }
        else if (v.is_ff())
          {
            additional_outputs &= bdd_ithvar(i);
            add &= bdd_nithvar(i);
          }
        else
          {
            bdd left = bdd_ithvar(i); // this is necessarily an output
            additional_outputs &= left;
            bool pos = v.is(spot::op::ap);
            const std::string apname =
              pos ? v.ap_name() : v[0].ap_name();
            bdd right = bdd_ithvar(mealy->register_ap(apname));
            if (pos)
              add &= bdd_biimp(left, right);
            else
              add &= bdd_xor(left, right);
          }
      }
    for (auto& e: mealy->edges())
      e.cond &= add;
    set_synthesis_outputs(mealy,
                          get_synthesis_outputs(mealy)
                          & additional_outputs);
  }

  void realizability_simplifier::patch_game(twa_graph_ptr game) const
  {
    if (SPOT_UNLIKELY(!global_equiv_output_only_))
      throw std::runtime_error("realizability_simplifier::path_game() requires "
                               "option global_equiv_output_only");

    auto& sp = spot::get_state_players(game);
    bdd add = bddtrue;
    for (auto [k, k_is_input, v]: mapping_)
      {
        int i = game->register_ap(k);
        if (k_is_input)
          continue;
        if (v.is_tt())
          add &= bdd_ithvar(i);
        else if (v.is_ff())
          add &= bdd_nithvar(i);
        else
          {
            bdd bv;
            if (v.is(spot::op::ap))
              bv = bdd_ithvar(game->register_ap(v.ap_name()));
            else // Not Ap
              bv = bdd_nithvar(game->register_ap(v[0].ap_name()));
            add &= bdd_biimp(bdd_ithvar(i), bv);
          }
      }
    for (auto& e: game->edges())
      if (sp[e.src])
        e.cond &= add;
    set_synthesis_outputs(game,
                          get_synthesis_outputs(game)
                          & bdd_support(add));
  }

}
