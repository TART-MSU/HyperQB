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

#include "config.h"
#include <sstream>
#include <iostream>
#include <spot/twacube/cube.hh>

namespace spot
{
  cubeset::cubeset(int aps)
  {
    size_ = aps;
    nb_bits_ = sizeof(unsigned int) * CHAR_BIT;
    uint_size_ = 1;
    while ((aps = aps -  nb_bits_)>0)
      ++uint_size_;
  }

  cube cubeset::alloc() const
  {
    return new unsigned int[2*uint_size_]();
  }

  void cubeset::set_true_var(cube c, unsigned int x) const
  {
    *(c+x/nb_bits_) |= 1 << (x%nb_bits_);
    *(c+uint_size_+x/nb_bits_) &= ~(1 << (x%nb_bits_));
  }

  void cubeset::set_false_var(cube c, unsigned int x) const
  {
    *(c+uint_size_+x/nb_bits_) |= 1 << (x%nb_bits_);
    *(c+x/nb_bits_) &= ~(1 << (x%nb_bits_));
  }

  bool cubeset::is_true_var(cube c, unsigned int x) const
  {
    unsigned int i = x/nb_bits_;
    bool true_var = (*(c+i) >> x) & 1;
    bool false_var = (*(c+i+uint_size_) >> x) & 1;
    return true_var && !false_var;
  }

  bool cubeset::is_false_var(cube c, unsigned int x) const
  {
    unsigned int i = x/nb_bits_;
    bool true_var = (*(c+i) >> x) & 1;
    bool false_var = (*(c+i+uint_size_) >> x) & 1;
    return !true_var && false_var;
  }

  bool cubeset::intersect(const cube lhs, const cube rhs) const
  {
    unsigned int true_elt;
    unsigned int false_elt;
    bool incompatible = false;
    for (unsigned int i = 0; i < uint_size_ && !incompatible; ++i)
      {
        true_elt = *(lhs+i) | *(rhs+i);
        false_elt = *(lhs+i+uint_size_) | *(rhs+i+uint_size_);
        incompatible |=  (true_elt & false_elt);
      }
    return !incompatible;
  }

  cube cubeset::intersection(const cube lhs, const cube rhs) const
  {
    auto* res = new unsigned int[2*uint_size_];
    for (unsigned int i = 0; i < uint_size_; ++i)
      {
        res[i] = *(lhs+i) | *(rhs+i);
        res[i+uint_size_] = *(lhs+i+uint_size_) | *(rhs+i+uint_size_);
      }
    return res;
  }

  bool cubeset::is_valid(const cube lhs) const
  {
    bool incompatible = false;
    for (unsigned int i = 0; i < uint_size_ && !incompatible; ++i)
      incompatible |= *(lhs+i) & *(lhs+i+uint_size_);
    return !incompatible;
  }

  size_t cubeset::size() const
  {
    return size_;
  }

  void cubeset::release(cube c) const
  {
    delete[] c;
  }

  void cubeset::display(const cube c) const
  {
    for (unsigned int i = 0; i < 2*uint_size_; ++i)
      {
        if (i == uint_size_)
          std::cout << '\n';

        for (unsigned x = 0; x < nb_bits_; ++x)
          std::cout << ((*(c+i) >> x) & 1);
      }
    std::cout << '\n';
  }

  std::string cubeset::dump(cube c, const std::vector<std::string>& aps) const
  {
    std::ostringstream oss;
    bool all_free = true;
    unsigned int cpt = 0;
    for (unsigned int i = 0; i < uint_size_; ++i)
      {
        for (unsigned x = 0; x < nb_bits_ && cpt != size_; ++x)
          {
            bool true_var = (*(c+i) >> x) & 1;
            bool false_var = (*(c+i+uint_size_) >> x) & 1;
            if (true_var)
              {
                oss << aps[cpt]
                    << (cpt != (size_ - 1) ? "&": "");
                all_free = false;
              }
            else if (false_var)
              {
                oss << '!' << aps[cpt]
                    << (cpt != (size_ - 1) ? "&": "");
                all_free = false;
              }
            ++cpt;
          }
      }
    if (all_free)
      oss << '1';

    std::string res = oss.str();
    if (res.back() == '&')
      res.pop_back();
    if (res.front() == '&')
      res =  res.substr(1);
    return res;
  }
}
