// -*- coding: utf-8 -*-
// Copyright (C) by the Spot authors, see the AUTHORS file for details. (LRE).
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

#pragma once

#include <bddx.h>
#include <spot/graph/graph.hh>
#include <spot/tl/formula.hh>
#include <spot/tl/relabel.hh>
#include <spot/twa/bdddict.hh>
#include <spot/twa/formula2bdd.hh>
#include <spot/twa/twagraph.hh>


using namespace spot;

struct bdd_partition
{
  struct S
  {
    bdd new_label = bddfalse;
  };
  struct T
  {
  };
  using implication_graph = digraph<S, T>;

  // A pointer to the conditions to be partitioned
  const std::vector<bdd>* all_cond_ptr;
  // Graph with the invariant that
  // children imply parents
  // Leaves from the partition
  // original conditions are "root" nodes
  std::unique_ptr<implication_graph> ig;
  // todo: technically there are at most two successors, so a graph
  // is "too" generic
  // All conditions currently part of the partition
  // unsigned corresponds to the associated node
  std::vector<std::pair<bdd, unsigned>> treated;
  std::vector<formula> new_aps;
  bool relabel_succ = false;

  bdd_partition() = default;
  bdd_partition(const std::vector<bdd>& all_cond)
    : all_cond_ptr(&all_cond)
    , ig{std::make_unique<implication_graph>(2*all_cond.size(),
                                             2*all_cond.size())}
  {
    // Create the roots of all old conditions
    // Each condition is associated to the state with
    // the same index
    const unsigned Norig = all_cond.size();
    ig->new_states(Norig);
  }

  // Facilitate conversion
  // This can only be called when letters have already
  // been computed
  relabeling_map
  to_relabeling_map(twa_graph& for_me) const;
}; // bdd_partition


bdd_partition
try_partition_me(const std::vector<bdd>& all_cond, unsigned max_letter);