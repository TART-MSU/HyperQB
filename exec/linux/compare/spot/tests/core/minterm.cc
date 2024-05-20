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

#undef NDEBUG

#include "config.h"

#include <bddx.h>
#include <cassert>

int main()
{
  bdd_init(10000, 10000);
  bdd_setvarnum(5);

  bdd a = bdd_ithvar(0);
  bdd b = bdd_ithvar(1);
  bdd c = bdd_ithvar(2);
  bdd d = bdd_ithvar(3);
  bdd e = bdd_ithvar(4);

  bdd fun = a | (b & d);
  bdd var = a & b & c & d & e;
  std::cout << fun << '\n';
  std::cout << var << "\n\n";
  bdd sum = bddfalse;
  int count = 0;
  for (auto minterm: minterms_of(fun, var))
    {
      ++count;
      sum |= minterm;
    }
  assert(count == 20);
  assert(fun == sum);

  var = a & b & d;
  std::cout << fun << '\n';
  std::cout << var << "\n\n";
  sum = bddfalse;
  count = 0;
  for (auto minterm: minterms_of(fun, var))
    {
      ++count;
      sum |= minterm;
    }
  assert(count == 5);
  assert(fun == sum);

  fun = bddtrue;
  std::cout << fun << '\n';
  std::cout << var << "\n\n";
  sum = bddfalse;
  count = 0;
  for (auto minterm: minterms_of(fun, var))
    {
      ++count;
      sum |= minterm;
    }
  assert(count == 8);
  assert(fun == sum);

  fun = var;
  std::cout << fun << '\n';
  std::cout << var << "\n\n";
  sum = bddfalse;
  count = 0;
  for (auto minterm: minterms_of(fun, var))
    {
      ++count;
      sum |= minterm;
    }
  assert(count == 1);
  assert(fun == sum);

  fun = var = bddtrue;
  std::cout << fun << '\n';
  std::cout << var << "\n\n";
  sum = bddfalse;
  count = 0;
  for (auto minterm: minterms_of(fun, var))
    {
      ++count;
      sum |= minterm;
    }
  assert(count == 1);
  assert(fun == sum);

  fun = bddfalse;
  std::cout << fun << '\n';
  std::cout << var << "\n\n";
  sum = bddfalse;
  count = 0;
  for (auto minterm: minterms_of(fun, var))
    {
      ++count;
      sum |= minterm;
    }
  assert(count == 0);
  assert(fun == sum);

  fun = (!a) | b;
  var = a & b;
  std::cout << fun << '\n';
  std::cout << var << "\n\n";
  sum = bddfalse;
  count = 0;
  for (auto minterm: minterms_of(fun, var))
    {
      ++count;
      sum |= minterm;
    }
  assert(count == 3);
  assert(fun == sum);

  fun = b;
  var = a & b;
  std::cout << fun << '\n';
  std::cout << var << "\n\n";
  sum = bddfalse;
  count = 0;
  for (auto minterm: minterms_of(fun, var))
    {
      ++count;
      sum |= minterm;
    }
  assert(count == 2);
  assert(fun == sum);

  fun = a;
  std::cout << fun << '\n';
  std::cout << var << "\n\n";
  sum = bddfalse;
  count = 0;
  for (auto minterm: minterms_of(fun, var))
    {
      ++count;
      sum |= minterm;
    }
  assert(count == 2);
  assert(fun == sum);

  fun = ((!a)&d) | (b&c&d);
  var = a & b & c & d;
  std::cout << fun << '\n';
  std::cout << var << "\n\n";
  sum = bddfalse;
  count = 0;
  for (auto minterm: minterms_of(fun, var))
    {
      ++count;
      sum |= minterm;
    }
  assert(count == 5);
  assert(fun == sum);


}
