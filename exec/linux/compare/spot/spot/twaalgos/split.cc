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
#include <spot/twaalgos/split.hh>
#include <spot/misc/bddlt.hh>
#include <spot/priv/robin_hood.hh>
#include <unordered_set>

namespace spot
{
  namespace
  {
    template<typename genlabels>
    twa_graph_ptr split_edges_aux(const const_twa_graph_ptr& aut,
                                  genlabels gen)
    {
      twa_graph_ptr out = make_twa_graph(aut->get_dict());
      out->copy_acceptance_of(aut);
      out->copy_ap_of(aut);
      out->prop_copy(aut, twa::prop_set::all());
      out->new_states(aut->num_states());
      out->set_init_state(aut->get_init_state_number());

      // We use a cache to avoid the costly loop around minterms_of().
      // Cache entries have the form (id, [begin, end]) where id is the
      // number of a BDD that as been (or will be) split, and begin/end
      // denotes a range of existing transition numbers that cover the
      // split.
      //
      // std::pair causes some noexcept warnings when used in
      // robin_hood::unordered_map with GCC 9.4.  Use robin_hood::pair
      // instead.
      typedef robin_hood::pair<unsigned, unsigned> cached_t;
      robin_hood::unordered_map<int, cached_t> split_cond;

      internal::univ_dest_mapper<twa_graph::graph_t> uniq(out->get_graph());

      for (auto& e: aut->edges())
        {
          bdd cond = e.cond;
          if (cond == bddfalse)
            continue;
          unsigned dst = e.dst;
          if (aut->is_univ_dest(dst))
            {
              auto d = aut->univ_dests(dst);
              dst = uniq.new_univ_dests(d.begin(), d.end());
            }

          auto& [begin, end] = split_cond[cond.id()];
          if (begin == end)
            {
              begin = out->num_edges() + 1;
              for (bdd minterm: gen(cond))
                out->new_edge(e.src, dst, minterm, e.acc);
              end = out->num_edges() + 1;
            }
          else
            {
              auto& g = out->get_graph();
              for (unsigned i = begin; i < end; ++i)
                out->new_edge(e.src, dst, g.edge_storage(i).cond, e.acc);
            }
        }
      return out;
    }
  }

  twa_graph_ptr split_edges(const const_twa_graph_ptr& aut)
  {
    bdd all = aut->ap_vars();
    return split_edges_aux(aut, [&](bdd cond) {
      return minterms_of(cond, all);
    });
  }

  void edge_separator::add_to_basis(bdd label)
  {
    if (label == bddfalse)
      return;
    // Split our current set of labels using this new one.
    //
    // Do not use a range-based or iterator-based for loop here,
    // as push_back invalidates the end iterator.
    for (unsigned cur = 0, sz = basis_.size(); cur < sz; ++cur)
      if (bdd common = basis_[cur] & label;
          common != bddfalse && common != basis_[cur])
        {
          basis_[cur] -= label;
          basis_.push_back(common);
        }
  }

  void edge_separator::add_to_basis(const const_twa_graph_ptr& aut)
  {
    for (formula f: aut->ap())
      aps_.insert(f);

    robin_hood::unordered_set<int> seen{bddtrue.id()};
    for (auto& e: aut->edges())
      if (bdd lab = e.cond; seen.insert(lab.id()).second)
        add_to_basis(lab);
  }

  bool edge_separator::add_to_basis(const const_twa_graph_ptr& aut,
                                    unsigned long max_label)
  {
    std::set<bdd, bdd_less_than> all_labels;
    for (auto& e: aut->edges())
      {
        if (all_labels.insert(e.cond).second)
          if (max_label-- == 0)
            return false;
      }
    for (bdd lab: all_labels)
      add_to_basis(lab);
    for (formula f: aut->ap())
      aps_.insert(f);
    return true;
  }

  twa_graph_ptr
  edge_separator::separate_implying(const const_twa_graph_ptr& aut)
  {
    auto res = split_edges_aux(aut, [this](bdd cond) {
      return this->separate_implying(cond);
    });
    for (formula f: aps_)
      res->register_ap(f);
    return res;
  }

  twa_graph_ptr
  edge_separator::separate_compat(const const_twa_graph_ptr& aut)
  {
    auto res = split_edges_aux(aut, [this](bdd cond) {
      return this->separate_compat(cond);
    });
    for (formula f: aps_)
      res->register_ap(f);
    return res;
  }

  twa_graph_ptr separate_edges(const const_twa_graph_ptr& aut)
  {
    edge_separator es;
    es.add_to_basis(aut);
    return es.separate_implying(aut);
  }
}
