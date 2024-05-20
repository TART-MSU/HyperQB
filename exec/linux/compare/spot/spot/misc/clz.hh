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
#include <cstddef>
#include <climits>

namespace spot
{
  template<typename Type,
           typename = std::enable_if_t<std::is_unsigned<Type>::value>>
  inline constexpr unsigned clz(Type n) noexcept
  {
    unsigned res = CHAR_BIT * sizeof(Type);
    while (n)
      {
        --res;
        n >>= 1;
      }
    return res;
  }

#if __GNUC__
  template<>
  inline constexpr unsigned clz(unsigned n) noexcept
  {
    return __builtin_clz(n);
  }

  template<>
  inline constexpr unsigned clz(unsigned long n) noexcept
  {
    return __builtin_clzl(n);
  }

  template<>
  inline constexpr unsigned clz(unsigned long long n) noexcept
  {
    return __builtin_clzll(n);
  }
#endif
}
