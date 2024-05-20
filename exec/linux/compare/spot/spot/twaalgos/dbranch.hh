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

#pragma once

#include <spot/twa/twagraph.hh>

namespace spot
{
  /// \ingroup twa_algorithms
  /// \brief Merge states to delay
  ///
  /// In an automaton with transition-based acceptance, if a state (x)
  /// has two outgoing transitions (x,l,m,y) and (x,l,m,z) going to
  /// states (x) and (y) that have no other incoming edges, then (y)
  /// and (z) can be merged (keeping the union of their outgoing
  /// destinations).
  ///
  /// If the input automaton uses state-based acceptance, running this
  /// function might make the acceptance transition-based, but only if
  /// two states with different acceptance are merged at some point.
  ///
  /// \return true iff the automaton was modified.
  SPOT_API bool delay_branching_here(const twa_graph_ptr& aut);
}
