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

#include <spot/misc/optionmap.hh>
#include <spot/twa/fwd.hh>
#include <spot/twaalgos/emptiness.hh>

namespace spot
{
  /// \brief Emptiness check based on Geldenhuys and Valmari's
  /// TACAS'04 paper.
  /// \ingroup emptiness_check_algorithms
  /// \pre The automaton \a a must have at most one acceptance condition.
  ///
  /// The original algorithm, coming from \cite geldenhuys.04.tacas ,
  /// has only been slightly modified to work on transition-based
  /// automata.
  SPOT_API emptiness_check_ptr
  explicit_gv04_check(const const_twa_ptr& a, option_map o = option_map());
}
