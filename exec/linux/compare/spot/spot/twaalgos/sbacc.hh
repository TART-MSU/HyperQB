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
  /// \brief Transform an automaton to use state-based acceptance
  ///
  /// This transformation is independent on the acceptance condition
  /// used.  The implementation supports alternating automata.
  ///
  /// It works by creating an automaton whose states correspond to
  /// pairs (s,colors) for each edge entering state s with colors.  In
  /// other words, we are pushing colors from the edges to their
  /// outgoing states.  The implementation is also able to pull colors
  /// on incoming states in cases where that helps.
  ///
  /// When called on automata that are already known to have
  /// state-based acceptance, this function returns the input
  /// unmodified, not a copy.
  ///
  /// Trues states (any state with an accepting self-loop labeled by
  /// true) are merged in the process.
  ///
  /// The output will have a named property called "original-states"
  /// that is a vector indexed by the produced states, and giving the
  /// corresponding state in the input automaton.  If the input
  /// automaton also had an "original-states" property, the two
  /// vectors will be composed, so the `original-states[s]` in the
  /// output will contains the value of `original-states[y] if state s
  /// was created from state y.
  ///
  /// If the input has a property named "original-classes", then the
  /// above "original-state" property is replaced by an
  /// "original-classes" property such that
  /// original-classes[s] is the class of the original state of s.
  SPOT_API twa_graph_ptr sbacc(twa_graph_ptr aut);
}
