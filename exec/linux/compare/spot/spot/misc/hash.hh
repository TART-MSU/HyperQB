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

#include <climits>
#include <string>
#include <functional>
#include <spot/misc/hashfunc.hh>

#include <unordered_map>
#include <unordered_set>

namespace spot
{

  /// \ingroup hash_funcs
  /// \brief A hash function for pointers.
  template <class T>
  struct ptr_hash
  {
    // A default constructor is needed if the ptr_hash object is
    // stored in a const member.  This occur with the clang version
    // installed by OS X 10.9.
    ptr_hash() noexcept
    {
    }

    size_t operator()(const T* p) const noexcept
    {
      return knuth32_hash(reinterpret_cast<size_t>(p));
    }
  };

  /// \ingroup hash_funcs
  /// \brief A hash function for strings.
  typedef std::hash<std::string> string_hash;

  /// \ingroup hash_funcs
  /// \brief A hash function that returns identity
  template<typename T>
  struct identity_hash
  {
    // A default constructor is needed if the identity_hash object is
    // stored in a const member.
    identity_hash()
    {
    }

    size_t operator()(const T& s) const noexcept
    {
      return s;
    }
  };


  struct pair_hash
  {
    template<typename T, typename U>
    std::size_t operator()(const std::pair<T, U> &p) const noexcept
    {

      if constexpr (std::is_integral<T>::value
                    && sizeof(T) <= sizeof(std::size_t)/2
                    && std::is_integral<U>::value
                    && sizeof(U) <= sizeof(std::size_t)/2)
        {
          constexpr unsigned shift = (sizeof(std::size_t)/2)*CHAR_BIT;
          std::size_t h = p.first;
          h <<= shift;
          h += p.second;
          return h;
        }
      else
        {
          std::hash<T> th;
          std::hash<U> uh;

          return wang32_hash(static_cast<size_t>(th(p.first)) ^
                             static_cast<size_t>(uh(p.second)));
        }
    }
  };

  // From primes.utm.edu shuffled. This primes are used when we simulate
  // transition shuffling using "primitive root modulo n" technique.
  static const unsigned primes[144] =
    {
      295075531, 334214857, 314607103, 314607191, 334214891, 334214779,
      295075421, 472882073, 256203329, 275604599, 314606953, 256203397,
      275604547, 256203631, 275604617, 472882141, 472882297, 472882219,
      256203229, 256203469, 275604643, 472882169, 275604803, 472882283,
      295075463, 334214593, 295075213, 256203373, 314607019, 314607193,
      295075399, 256203523, 314607001, 295075289, 256203293, 256203641,
      256203307, 314607047, 295075373, 314607053, 314606977, 334214681,
      275604691, 275604577, 472882447, 314607031, 275605019, 472882477,
      472882499, 314607173, 295075241, 295075471, 295075367, 256203559,
      314607233, 275604881, 334214941, 472882103, 275604821, 472882511,
      295075357, 472882517, 314607023, 256203317, 295075337, 275605007,
      472882391, 256203223, 334214723, 295075381, 295075423, 275604733,
      314607113, 256203257, 472882453, 256203359, 295075283, 314607043,
      256203403, 472882259, 314606891, 472882253, 314606917, 256203349,
      256203457, 295075457, 472882171, 314607229, 295075513, 472882429,
      334214953, 275604841, 295075309, 472882099, 334214467, 334214939,
      275604869, 314607077, 314607089, 275604947, 275605027, 295075379,
      334214861, 314606909, 334214911, 314607199, 275604983, 314606969,
      256203221, 334214899, 256203611, 256203679, 472882337, 275604767,
      472882199, 295075523, 472882049, 275604817, 334214561, 334214581,
      334214663, 295075489, 295075163, 334214869, 334214521, 295075499,
      275604913, 334214753, 334214687, 256203491, 295075153, 334214893,
      472882411, 472882117, 275604793, 334214833, 334214591, 314607091,
      256203419, 275604727, 256203659, 275604961, 334214557, 275604677
    };
}
