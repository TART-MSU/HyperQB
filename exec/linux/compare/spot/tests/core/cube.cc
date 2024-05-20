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
#include <bddx.h>

#include <iostream>
#include <spot/twacube/cube.hh>
#include <spot/misc/hash.hh>
#include <spot/twacube/cube.hh>
#include <spot/twacube_algos/convert.hh>
#include <spot/twa/bdddict.hh>
#include <spot/tl/environment.hh>
#include <spot/tl/defaultenv.hh>
#include <unordered_map>


static bool test_translation(bdd& input,  spot::cubeset& cubeset,
                             std::unordered_map<int, int>& binder,
                             std::unordered_map<int, int>& reverse_binder,
                             std::vector<std::string>& aps)
{
  // The BDD used to detect if the conversion works
  bdd res = bddfalse;
  bdd initial = input;

  std::cout << "bdd  : " << input << '\n';
  while (initial != bddfalse)
    {
      bdd one = bdd_satone(initial);
      initial -= one;
      auto cube = spot::satone_to_cube(one, cubeset, binder);
      res |= spot::cube_to_bdd(cube, cubeset, reverse_binder);
      std::cout << "cube : " << cubeset.dump(cube, aps) << '\n';
      delete[] cube;
    }

  // Translating BDD to cubes and cubes to BDD should provide same BDD.
  return input == res;
}

static void test_bdd_to_cube()
{
  auto d = spot::make_bdd_dict();
  spot::environment& e = spot::default_environment::instance();

  // Some of these variables are not desired into the final cube
  auto ap_0 = e.require("0");
  int idx_0 = d->register_proposition(ap_0, d);
  auto ap_a = e.require("a");
  int idx_a = d->register_proposition(ap_a, d);
  auto ap_b = e.require("b");
  int idx_b = d->register_proposition(ap_b, d);
  auto ap_1 = e.require("1");
  int idx_1 = d->register_proposition(ap_1, d);
  auto ap_c = e.require("c");
  int idx_c = d->register_proposition(ap_c, d);
  auto ap_d = e.require("d");
  int idx_d = d->register_proposition(ap_d, d);
  auto ap_e = e.require("e");
  int idx_e = d->register_proposition(ap_e, d);

  // Prepare cube
  std::vector<std::string> aps = {"a", "b", "c", "d", "e"};
  spot::cubeset cubeset(aps.size());

  // Map Bdd indexes to cube indexes
  std::unordered_map<int, int> binder = {
    {idx_a, 0},
    {idx_b, 1},
    {idx_c, 2},
    {idx_d, 3},
    {idx_e, 4}
  };

  // Map cube indexes to Bdd indexes
  std::unordered_map<int, int> reverse_binder = {
    {0, idx_a},
    {1, idx_b},
    {2, idx_c},
    {3, idx_d},
    {4, idx_e}
  };

  // The BDD to convert
  bdd x;
  int result;

  // Test bddtrue
  x = bddtrue;
  result = test_translation(x, cubeset, binder, reverse_binder, aps);
  assert(result);

  // Test bddfalse
  x = bddfalse;
  result = test_translation(x, cubeset, binder, reverse_binder, aps);
  assert(result);

  // Test bdddeterministic bdd
  x = bdd_ithvar(idx_a) & !bdd_ithvar(idx_b) & bdd_ithvar(idx_c)  &
    !bdd_ithvar(idx_d) & bdd_ithvar(idx_e);
  result = test_translation(x, cubeset, binder, reverse_binder, aps);
  assert(result);

  // // Test some free var bdd
  x = (bdd_ithvar(idx_a) | bdd_ithvar(idx_b)) & bdd_ithvar(idx_d);
  result = test_translation(x, cubeset, binder, reverse_binder, aps);
  assert(result);

  (void)result;

  // Free all variables
  d->unregister_variable(idx_e, d);
  d->unregister_variable(idx_d, d);
  d->unregister_variable(idx_c, d);
  d->unregister_variable(idx_1, d);
  d->unregister_variable(idx_b, d);
  d->unregister_variable(idx_a, d);
  d->unregister_variable(idx_0, d);
}

int main()
{
  std::vector<std::string> aps = {"a", "b", "c", "d", "e"};
  spot::cubeset cs(aps.size());
  spot::cube mc = cs.alloc();
  cs.set_true_var(mc, 0);
  cs.set_false_var(mc, 3);
  std::cout << "size : " << cs.size() << '\n';
  std::cout << "cube : " << cs.dump(mc, aps) << '\n';
  std::cout << "valid : " << cs.is_valid(mc) << '\n';
  std::cout << "intersect(c,c) : " << cs.intersect(mc, mc) << '\n';
  cs.display(mc);

  spot::cube mc1 = cs.alloc();
  cs.set_false_var(mc1, 0);
  cs.set_true_var(mc1, 1);
  std::cout << "size : " << cs.size() << '\n';
  std::cout << "cube : " << cs.dump(mc1, aps) << '\n';
  std::cout << "valid : " << cs.is_valid(mc1) << '\n';
  std::cout << "intersect(c1,c1) : " << cs.intersect(mc1, mc1) << '\n';
  std::cout << "intersect(c,c1) : " << cs.intersect(mc, mc1) << '\n';
  std::cout << "intersect(c1,c) : " << cs.intersect(mc1, mc) << '\n';
  cs.display(mc1);

  spot::cube mc2 = cs.alloc();
  cs.set_true_var(mc2, 1);
  cs.set_true_var(mc2, 3);
  std::cout << "size : " << cs.size() << '\n';
  std::cout << "cube : " << cs.dump(mc2, aps) << '\n';
  std::cout << "valid : " << cs.is_valid(mc2) << '\n';
  std::cout << "intersect(c2,c1) : " << cs.intersect(mc1, mc2) << '\n';
  std::cout << "intersect(c2,c) : " << cs.intersect(mc, mc2) << '\n';
  cs.display(mc2);

  std::vector<std::string> apsx;
  for (unsigned i = 0; i < 44; ++i)
    apsx.push_back("a" + std::to_string(i));
  spot::cubeset csx(apsx.size());
  spot::cube mcx = csx.alloc();
  csx.set_true_var(mcx, 0);
  csx.set_false_var(mcx, 42);
  std::cout << "cube : " << csx.dump(mcx, apsx) << '\n';
  csx.display(mcx);

  cs.release(mcx);
  cs.release(mc2);
  cs.release(mc1);
  cs.release(mc);
  test_bdd_to_cube();
}
