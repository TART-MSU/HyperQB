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

#include <string>
#include <utility>
#include <vector>

#include <spot/tl/formula.hh>

namespace spot
{
  /// \ingroup tl_rewriting
  /// \brief Helper to rewrite a PSL formula in Suffix Operator Normal Form.
  ///
  /// SONF is described in section 4 of \cite cimatti.06.fmcad
  ///
  /// The formula output by this function is guaranteed to be in Negative Normal
  /// Form.
  ///
  /// \param f the PSL formula to rewrite
  /// \param prefix the prefix to use to name newly introduced aps
  /// \return a pair with the rewritten formula, and a vector containing the
  ///         names of newly introduced aps
  SPOT_API std::pair<formula, std::vector<std::string>>
  suffix_operator_normal_form(formula f, const std::string prefix);
}
