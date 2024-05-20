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
#include <vector>
#include <string>
#include <climits>
#include <sstream>

namespace spot
{
  /// \brief A cube is only a set of bits in memory.
  ///
  /// This set can be seen as two bitsets
  ///   - true_var  : a bitset representing variables that
  ///                 are set to true
  ///   - false_var : a bitset representing variables that
  ///                 are set to false
  ///
  /// In the  two vectors a bit set to 1 represent a variable set to
  /// true (resp. false) for the true_var (resp. false_var)
  ///
  /// Warning : a variable cannot be set in both bitset at the
  ///           same time (consistency! cannot be true and false)
  ///
  /// The cube for (a & !b) will be represented by:
  ///     - true_var  = 1 0
  ///     - false_var = 0 1
  ///
  /// To represent free variables such as in (a & !b) | (a & b)
  /// (which is equivalent to (a) with b free)
  ///     - true_var  : 1 0
  ///     - false_var : 0 0
  /// This example shows that the representation of free variables
  /// is done by unsetting variable in both vector
  ///
  /// To be memory efficient, these two bitsets are contigious in memory
  /// i.e. if we want to represent 35 variables, a cube will be
  /// represented by 4 unsigned int contiguous in memory. The 35
  /// first bits represent truth values. The 29 bits following are
  /// useless. Then, the 35 bits represents false value and the
  /// rest is useless.
  ///
  /// Note that useless bits are only to perform some action efficiently,
  /// i.e. only by ignoring them. The manipulation of cubes must be done
  /// using the cubeset class
  using cube = unsigned*;

  class SPOT_API cubeset final
  {
    // \brief The total number of variables stored
    size_t size_;

    /// \brief The number of unsigned needed by the cube (for each part)
    size_t uint_size_;

    /// \brief The number of bits for an unsigned int
    size_t nb_bits_;

  public:
    // Some default/deleted constructor/destructors
    cubeset() = delete;
    ~cubeset() = default;

    /// \brief Build the cubeset manager for \a aps variables
    cubeset(int aps);

    /// \brief Allocate a new cube
    cube alloc() const;

    /// \brief Set the variable at position \a x to true.
    void set_true_var(cube c, unsigned int x) const;

    /// \brief Set the variable at position \a x to false.
    void set_false_var(cube c, unsigned int x) const;

    /// \brief Check if the variable at position \a x is true.
    bool is_true_var(cube c, unsigned int x) const;

    /// \brief Check if the variable at position \a x is false.
    bool is_false_var(cube c, unsigned int x) const;

    /// \brief return true if two cube intersect, i.e synchronisables.
    bool intersect(const cube lhs, const cube rhs) const;

    /// \brief return a cube resulting from the intersection of the  two cubes
    cube intersection(const cube lhs, const cube rhs) const;

    /// \brief Check wether \a lhs is valid, is there is not variable
    /// that is true and false at the same time.
    bool is_valid(const cube lhs) const;

    /// \brief Return the size of each cube.
    size_t size() const;

    /// \brief Release a cube.
    void release(cube lhs) const;

    /// \brief Raw display cube
    void display(const cube c) const;

    /// \brief Return the cube binded with atomic proposition names
    std::string dump(cube c, const std::vector<std::string>& aps) const;
  };
}
