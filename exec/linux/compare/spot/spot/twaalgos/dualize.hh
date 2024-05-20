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

#include <spot/misc/common.hh>
#include <spot/twa/fwd.hh>

namespace spot
{
  /// \ingroup twa_misc
  /// \brief Complement an automaton by dualizing it.
  ///
  /// Given an automaton \a aut of any type, produces the dual as
  /// output. Before dualization, the automaton will be completed if
  /// it isn't already, but any sink state in the output might then be
  /// removed.
  ///
  /// Dualizing the automaton is done by interpreting the outgoing
  /// transitions of a state as a Boolean function, and then swapping
  /// operators ∧ and ̇∨.  This first step does not have to be done on
  /// deterministic automata.  Additionally, the acceptance condition
  /// is dualized by swapping operators ∧ and ̇∨, and swapping Inf and
  /// Fin.
  ///
  /// For instance, the dual of a generalized Büchi automaton will be
  /// a generalized co-Büchi automaton.
  ///
  /// If the input acceptance condition accepts every infinite path
  /// (such as "t" or "Inf(0)|Fin(0)") and the automaton is not
  /// complete, then the input automaton will be assumed to have Büchi
  /// acceptance in order to complete it, and the output will then
  /// have co-Büchi acceptance.
  ///
  /// Due to a defect in the way transition-based alternating automata
  /// are represented in Spot and in the HOA format, existential
  /// automata with transition-based acceptance will be converted to
  /// use state-based acceptance before dualization.   See
  /// https://github.com/adl/hoaf/issues/68 for more information.
  ///
  /// If the input automaton is deterministic, the output will be deterministic.
  /// If the input automaton is existential, the output will be universal.
  /// If the input automaton is universal, the output will be existential.
  /// Finally, if the input automaton is alternating, the result is alternating.
  /// More can be found on page 22 (Definition 1.6) of \cite loding.98.msc .
  ///
  /// Functions like to_generalized_buchi() or remove_fin() are frequently
  /// called on existential automata after dualize() to obtain an easier
  /// acceptance condition, but maybe at the cost of losing determinism.
  ///
  /// Up to version 2.11.6, this function used to call
  /// cleanup_acceptance_here() to simplify the acceptance condition
  /// after dualization.  This caused some surprizes, users expected
  /// the dual of a Büchi automaton to be a co-Büchi automaton, but
  /// cleanup_acceptance_here() sometimes reduced the condition to `t`
  /// when all states where accepting.  This function is not called
  /// anymore since version 2.12.
  SPOT_API twa_graph_ptr
  dualize(const const_twa_graph_ptr& aut);
}
