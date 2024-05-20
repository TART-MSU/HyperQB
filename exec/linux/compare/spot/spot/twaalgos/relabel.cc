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
#include <spot/twaalgos/relabel.hh>
#include <spot/twa/formula2bdd.hh>
#include <spot/tl/formula.hh>
#include <spot/priv/partitioned_relabel.hh>
#include <cmath>
#include <algorithm>


namespace spot
{
  namespace
  {

    void
    comp_new_letters(bdd_partition& part,
                    twa_graph& aut,
                    const std::string& var_prefix,
                    bool split)
    {
      auto& ig = *part.ig;
      const auto& treated = part.treated;
      auto& new_aps = part.new_aps;
      // Get the new variables and their negations
      const unsigned Nnl = treated.size();
      const unsigned Nnv = std::ceil(std::log2(Nnl));
      std::vector<std::array<bdd, 2>> Nv_vec(Nnv);

      new_aps.reserve(Nnv);
      for (unsigned i = 0; i < Nnv; ++i)
        {
          // todo check if it does not exist / use anonymous?
          new_aps.push_back(formula::ap(var_prefix+std::to_string(i)));
          int v = aut.register_ap(new_aps.back());
          Nv_vec[i] = {bdd_nithvar(v), bdd_ithvar(v)};
        }

      auto leaveidx2label = [&](unsigned idx)
        {
          unsigned c = 0;
          unsigned rem = idx;
          bdd thisbdd = bddtrue;
          while (rem)
            {
              thisbdd &= Nv_vec[c][rem & 1];
              ++c;
              rem >>= 1;
            }
          for (; c < Nnv; ++c)
            thisbdd &= Nv_vec[c][0];
          return thisbdd;
        };

      // Compute only labels of leaves
      for (unsigned idx = 0; idx < Nnl; ++idx)
        ig.state_storage(treated[idx].second).new_label = leaveidx2label(idx);

      // We will label the implication graph with the new letters
      auto relabel_impl = [&](unsigned s, auto&& relabel_impl_rec)
        {
          auto& ss = ig.state_storage(s);
          if (ss.new_label != bddfalse)
            return ss.new_label;
          else
            {
              assert((ss.succ != 0) && "Should not be a leave");
              bdd thisbdd = bddfalse;
              for (const auto& e : ig.out(s))
                thisbdd |= relabel_impl_rec(e.dst, relabel_impl_rec);
              ss.new_label = thisbdd;
              return thisbdd;
            }
        };

      if (!split)
        {
          // For split only leaves is ok,
          // disjunction is done via transitions
          // This will compute the new_label for all states in the ig
          const unsigned Norig = part.all_cond_ptr->size();
          for (unsigned i = 0; i < Norig; ++i)
            relabel_impl(i, relabel_impl);
        }
    } // comp_new_letters

    // Recursive traversal of implication graph
    void replace_label_(unsigned si,
                        unsigned esrc, unsigned edst,
                        bdd& econd,
                        const bdd_partition::implication_graph& ig,
                        twa_graph& aut)
    {
      auto& sstore = ig.state_storage(si);
      if (sstore.succ == 0)
        {
          if (econd == bddfalse)
            econd = sstore.new_label;
          else
            aut.new_edge(esrc, edst, sstore.new_label);
        }
      else
        {
          for (const auto& e_ig : ig.out(si))
            replace_label_(e_ig.dst, esrc, edst, econd, ig, aut);
        }
    }

    relabeling_map
    partitioned_relabel_here_(twa_graph& aut, bool split,
                              unsigned max_letter,
                              unsigned max_letter_mult,
                              const bdd& concerned_ap,
                              bool treat_all,
                              const std::string& var_prefix)
    {
      auto abandon = []()
        {
          return relabeling_map{};
        };


      // When split we need to distinguish effectively new and old edges
      if (split)
        {
          aut.get_graph().remove_dead_edges_();
          aut.get_graph().sort_edges_();
          aut.get_graph().chain_edges_();
        }

      // Get all conditions present in the automaton
      std::vector<bdd> all_cond;
      bdd ignoredcon = bddtrue;
      std::unordered_map<int, unsigned> all_cond_id2idx;

      all_cond.reserve(0.1*aut.num_edges());
      all_cond_id2idx.reserve(0.1*aut.num_edges());

      // Map for all supports
      // and whether or not they are to be relabeled
      std::unordered_map<bdd, std::pair<bool, bdd>, bdd_hash> all_supports;

      for (const auto& e : aut.edges())
        {
          auto it = all_supports.find(e.cond);
          if (it != all_supports.end())
            continue; // Already treated
          bdd se = bddtrue;
          bool is_concerned = true;
          if (!treat_all)
            {
              se = bdd_support(e.cond);
              is_concerned = bdd_implies(concerned_ap, se);
            }

          all_supports.emplace(e.cond,
                               std::make_pair(is_concerned, se));

          if (!is_concerned)
            {
              assert(bdd_existcomp(se, concerned_ap) == bddtrue
                     && "APs are not partitioned");
              continue;
            }

          if (all_cond_id2idx.try_emplace(e.cond.id(), all_cond.size()).second)
            {
              all_cond.push_back(e.cond);
              if (all_cond.size() > max_letter)
                return abandon();
            }
        }

      unsigned stop = max_letter;
      if (max_letter_mult != -1u)
        {
          // Make sure it does not overflow
          if (max_letter_mult <= (-1u / ((unsigned) all_cond.size())))
            stop = std::min(stop,
                            (unsigned) (max_letter_mult*all_cond.size()));
        }

      auto this_partition = try_partition_me(all_cond, stop);

      if (!this_partition.relabel_succ)
        return abandon();

      comp_new_letters(this_partition, aut, var_prefix, split);

      // An original condition is represented by all leaves that imply it
      auto& ig = *this_partition.ig;
      const unsigned Ns = aut.num_states();
      const unsigned Nt = aut.num_edges();
      for (unsigned s = 0; s < Ns; ++s)
        {
          for (auto& e : aut.out(s))
            {
              if (aut.edge_number(e) > Nt)
                continue;
              if (!all_supports.at(e.cond).first)
                continue; // Edge not concerned
              unsigned idx = all_cond_id2idx[e.cond.id()];

              if (split)
                {
                  // initial call
                  // We can not hold a ref to the edge
                  // as the edgevector might get reallocated
                  bdd econd = bddfalse;
                  unsigned eidx = aut.edge_number(e);
                  replace_label_(idx, e.src, e.dst,
                                econd, ig, aut);
                  aut.edge_storage(eidx).cond = econd;
                }
              else
                e.cond = ig.state_storage(idx).new_label;
            } // for edge
        } // for state
      return this_partition.to_relabeling_map(aut);
    }

    void
    relabel_here_ap_(twa_graph_ptr& aut_ptr, relabeling_map relmap)
    {
      assert(aut_ptr);
      twa_graph& aut = *aut_ptr;

      std::unique_ptr<bddPair> pairs(bdd_newpair());
      auto d = aut.get_dict();
      std::vector<int> vars;
      std::set<int> newvars;
      vars.reserve(relmap.size());
      bool bool_subst = false;
      auto aplist = aut.ap();

      for (auto& p: relmap)
        {
          // Don't attempt to rename APs that are not used.
          if (std::find(aplist.begin(), aplist.end(), p.first) == aplist.end())
            continue;

          int oldv = aut.register_ap(p.first);
          vars.emplace_back(oldv);
          if (p.second.is(op::ap))
            {
              int newv = aut.register_ap(p.second);
              newvars.insert(newv);
              bdd_setpair(pairs.get(), oldv, newv);
            }
          else
            {
              p.second.traverse([&](const formula& f)
                                {
                                  if (f.is(op::ap))
                                    newvars.insert(aut.register_ap(f));
                                  return false;
                                });
              bdd newb = formula_to_bdd(p.second, d, aut_ptr);
              bdd_setbddpair(pairs.get(), oldv, newb);
              bool_subst = true;
            }
      }

    bool need_cleanup = false;
    typedef bdd (*op_t)(const bdd&, bddPair*);
    op_t op = bool_subst ?
      static_cast<op_t>(bdd_veccompose) : static_cast<op_t>(bdd_replace);
    for (auto& t: aut.edges())
      {
        bdd c = (*op)(t.cond, pairs.get());
        t.cond = c;
        if (c == bddfalse)
          need_cleanup = true;
      }

    // Erase all the old variables that are not reused in the new set.
    // (E.g., if we relabel a&p0 into p0&p1 we should not unregister
    // p0)
    for (auto v: vars)
      if (newvars.find(v) == newvars.end())
        aut.unregister_ap(v);

    // If some of the edges were relabeled false, we need to clean the
    // automaton.
    if (need_cleanup)
      {
        aut.merge_edges();     // remove any edge labeled by 0
        aut.purge_dead_states(); // remove useless states
      }
    }

  void
  relabel_here_gen_(twa_graph_ptr& aut_ptr, relabeling_map relmap)
    {
      assert(aut_ptr);
      twa_graph& aut = *aut_ptr;

      auto form2bdd = [this_dict = aut.get_dict()](const formula& f)
        {
          return formula_to_bdd(f, this_dict, this_dict);
        };

      auto bdd2form = [bdddict = aut.get_dict()](const bdd& cond)
        {
          return bdd_to_formula(cond, bdddict);
        };


      // translate formula -> bdd
      std::unordered_map<bdd, bdd, bdd_hash> base_letters;
      base_letters.reserve(relmap.size());

      std::unordered_map<bdd, bdd, bdd_hash> comp_letters;
      std::unordered_set<bdd, bdd_hash> ignored_letters;

      // Necessary to detect unused
      bdd new_var_supp = bddtrue;
      auto translate = [&](bdd& cond)
        {
          // Check if known
          for (const auto& map : {base_letters, comp_letters})
            {
              auto it = map.find(cond);
              if (it != map.end())
                {
                  cond = it->second;
                  return;
                }
            }

          // Check if known to be ignored
          if (auto it = ignored_letters.find(cond);
                it != ignored_letters.end())
            return;

          // Check if ignored
          bdd cond_supp = bdd_support(cond);
          if (!bdd_implies(new_var_supp, cond_supp))
            {
              ignored_letters.insert(cond);
              assert(bdd_existcomp(cond_supp, new_var_supp) == bddtrue
                     && "APs are not partitioned");
              return;
            }

          // Compute
          // compose the given cond from a disjunction of base_letters
          bdd old_cond = bddfalse;
          for (const auto& [k, v] : base_letters)
            {
              if (bdd_implies(k, cond))
                old_cond |= v;
            }
          comp_letters[cond] = old_cond;
          cond = old_cond;
          return;
        };

      for (const auto& [new_f, old_f] : relmap)
        {
          bdd new_cond = form2bdd(new_f);
          new_var_supp &= bdd_support(new_cond);
          base_letters[new_cond] = form2bdd(old_f);
        }


      // Save the composed letters? With a special separator like T/F?
      // Is swapping between formula <-> bdd expensive
      for (auto& e : aut.edges())
        translate(e.cond);

      // Remove the new auxiliary variables from the aut
      bdd c_supp = new_var_supp;
      while (c_supp != bddtrue)
        {
          aut.unregister_ap(bdd_var(c_supp));
          c_supp = bdd_high(c_supp);
        }

      return;
    }

  } // Namespace

  void
  relabel_here(twa_graph_ptr& aut, relabeling_map* relmap)
  {
    if (!relmap || relmap->empty())
      return;

    // There are two different types of relabeling maps:
    // 1) The "traditional":
    // New atomic propositions (keys) correspond to general formulas over
    // the original propositions (values)
    // 2) The one resulting from partitioned_relabel_here
    // Here general (boolean) formulas over new propositions (keys)
    // are associated to general formulas over
    // the original propositions (values)

    if (!std::all_of(relmap->begin(), relmap->end(),
                    [](const auto& it){return it.first.is_boolean()
                                              && it.second.is_boolean(); }))
      throw std::runtime_error
            ("relabel_here: old labels and new labels "
             "should be Boolean formulas");

    bool only_ap = std::all_of(relmap->cbegin(), relmap->cend(),
                               [](const auto& p)
                                 {
                                   return p.first.is(op::ap);
                                 });

    if (only_ap)
      relabel_here_ap_(aut, *relmap);
    else
      relabel_here_gen_(aut, *relmap);
  }

  relabeling_map
  partitioned_relabel_here(twa_graph_ptr& aut,
                           bool split,
                           unsigned max_letter,
                           unsigned max_letter_mult,
                           const bdd& concerned_ap,
                           std::string var_prefix)
  {
    if (!aut)
      throw std::runtime_error("aut is null!");

    if (std::find_if(aut->ap().cbegin(), aut->ap().cend(),
                     [var_prefix](const auto& ap)
                      {
                        return ap.ap_name().find(var_prefix) == 0;
                      }) != aut->ap().cend())
      throw std::runtime_error("partitioned_relabel_here(): "
          "The given prefix for new variables may not appear as "
          "a prefix of existing variables.");

    // If concerned_ap == bddtrue -> all aps are concerned
    bool treat_all = concerned_ap == bddtrue;
    bdd concerned_ap_
      = treat_all ? aut->ap_vars() : concerned_ap;
    return partitioned_relabel_here_(*aut, split,
                                     max_letter, max_letter_mult,
                                     concerned_ap_,
                                     treat_all,
                                     var_prefix);
  }
}
