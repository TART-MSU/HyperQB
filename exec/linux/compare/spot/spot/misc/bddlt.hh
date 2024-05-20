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

#include <bddx.h>
#include <functional>

namespace spot
{
  /// \ingroup misc_tools
  /// \brief Comparison functor for BDDs.
  ///
  /// This comparison function use BDD ids for efficiency.  An
  /// algorithm depending on this order may return different results
  /// depending on how the BDD library has been used before.
  struct bdd_less_than
  {
    bool
    operator()(const bdd& left, const bdd& right) const
    {
      return left.id() < right.id();
    }
  };

  /// \ingroup misc_tools
  /// \brief Comparison functor for BDDs.
  ///
  /// This comparison function actually check for BDD variables, so as
  /// long as the variable order is the same, the output of this
  /// comparison will be stable and independent on previous BDD
  /// operations.
  struct bdd_less_than_stable
  {
    bool
    operator()(const bdd& left, const bdd& right) const
    {
      return bdd_stable_cmp(left, right) < 0;
    }
  };

  /// \ingroup misc_tools
  /// \brief Hash functor for BDDs.
  struct bdd_hash
  {
    size_t
    operator()(const bdd& b) const noexcept
    {
      return b.id();
    }
  };
}
