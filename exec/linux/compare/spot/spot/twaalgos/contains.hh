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
#include <spot/tl/formula.hh>

/// \defgroup containment Language containment checks
/// \ingroup twa_algorithms

namespace spot
{
  /// \ingroup containment
  /// \brief Test if the language of \a right is included in that of \a left.
  ///
  /// Both arguments can be either formulas or automata.  Formulas
  /// will be converted into automata.
  ///
  /// The inclusion check if performed by ensuring that the automaton
  /// associated to \a right does not intersect the automaton
  /// associated to the complement of \a left.  It helps if \a left
  /// is a deterministic automaton or a formula (because in both cases
  /// complementation is easier).
  ///
  /// Complementation is only supported on twa_graph automata, so that
  /// is the reason \a left must be a twa_graph.  Right will be
  /// explored on-the-fly if it is not a twa_graph.
  ///
  /// @{
  SPOT_API bool contains(const_twa_graph_ptr left, const_twa_ptr right);
  SPOT_API bool contains(const_twa_graph_ptr left, formula right);
  SPOT_API bool contains(formula left, const_twa_ptr right);
  SPOT_API bool contains(formula left, formula right);
  /// @}

  /// \ingroup containment
  /// \brief Test if the language of \a left is equivalent to that of \a right.
  ///
  /// Both arguments can be either formulas or automata.  Formulas
  /// will be converted into automata.
  /// @{
  SPOT_API bool are_equivalent(const_twa_graph_ptr left,
                               const_twa_graph_ptr right);
  SPOT_API bool are_equivalent(const_twa_graph_ptr left, formula right);
  SPOT_API bool are_equivalent(formula left, const_twa_graph_ptr right);
  SPOT_API bool are_equivalent(formula left, formula right);
  /// @}

  /// \ingroup containment
  ///
  /// Query, or change the version of the containment check to use by
  /// contains() or twa::exclusive_run().
  ///
  /// By default those containment checks use a complementation-based
  /// algorithm that is generic that work on any acceptance condition.
  /// Alternative algorithms such as contains_forq() are available,
  /// for Büchi automata, but are not used by default.
  ///
  /// When calling this function \a version can be:
  /// - "default" to force the above default containment checks to be used
  /// - "forq" to use contains_forq() when possible
  /// - nullptr do not modify the preference.
  ///
  /// If the first call to containement_select_version() is done with
  /// nullptr as an argument, then the value of the
  /// SPOT_CONTAINMENT_CHECK environment variable is used instead.
  ///
  /// In all cases, the preferred containment check is returned as an
  /// integer.  This integer is meant to be used by Spot's algorithms
  /// to select the desired containment check to apply, but it's
  /// encoding (currently 1 for FORQ, 0 for default) should be
  /// regarded as an implementation detail subject to change.
  SPOT_API int containment_select_version(const char* version = nullptr);
}
