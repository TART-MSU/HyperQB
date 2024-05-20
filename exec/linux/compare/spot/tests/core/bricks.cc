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

#include <spot/bricks/brick-assert>
#include <spot/bricks/brick-bitlevel>
#include <spot/bricks/brick-hash>
#include <spot/bricks/brick-hashset>
#include <spot/bricks/brick-shmem>
#include <spot/bricks/brick-types>


struct both
{
  int x;
  int y;
};

template<typename T>
struct mytest_hasher_both
{
  template<typename X>
  mytest_hasher_both(X&)
  { }

  mytest_hasher_both() = default;

  brick::hash::hash128_t hash(both t) const
  {
    return std::make_pair(t.x*10, t.x);
  }
  bool valid(both t) const
  {
    return t.x != 0;
  }
  bool equal(both a, both b) const
  {
    return a.x == b.x;
  }
};

template<class Functor, class...Objects>
void for_all(Functor&& f, Objects&&... objects)
{
    using expand = int[];
    (void) expand { 0, (f(std::forward<Objects>(objects)), 0)... };

}

static void test_brick_hashmap()
{
  brick::t_hashset::Sequential< brick::t_hashset::CS > t1;
  brick::t_hashset::Sequential< brick::t_hashset::FS > t2;
  brick::t_hashset::Sequential< brick::t_hashset::ConCS > t3;
  brick::t_hashset::Sequential< brick::t_hashset::ConFS > t4;

  // In C++ 17, the for_all could be rewritten with fold expression
  // auto tester = [](auto&&... args) { (args.basic(), ...);  };
  // tester(t);
  for_all([](auto& e)
          {
            std::cout << "# " << typeid(e).name() << '\n';
            std::cout << "  [HM -- Sequential] Testing basic\n";
            e.basic();
            std::cout << "  [HM -- Sequential] Testing stress\n";
            e.stress();
            std::cout << "  [HM -- Sequential] Testing set\n";
            e.set();
          }, t1, t2, t3, t4);

  brick::t_hashset::Parallel< brick::t_hashset::ConCS > t5;
  brick::t_hashset::Parallel< brick::t_hashset::ConFS > t6;
  for_all([](auto& e)
          {
            std::cout << "# " << typeid(e).name() << '\n';
            std::cout << "  [HM -- Parallel] Testing insert\n";
            e.insert();
            std::cout << "  [HM -- Parallel] Testing multi\n";
            e.multi();
            std::cout << "  [HM -- Parallel] Testing stress\n";
            e.stress();
            std::cout << "  [HM -- Parallel] Testing empty\n";
            e.empty();
            std::cout << "  [HM -- Parallel] Testing set\n";
            e.set();
          }, t5, t6);
}

static void test_brick_bitlevel()
{
  brick::t_bitlevel::BitTupleTest t1;
  std::cout << "# " << typeid(t1).name() << '\n';
  std::cout << "  [BL -- BitTupleTest] Testing mask\n";
  t1.mask();
  std::cout << "  [BL -- BitTupleTest] Testing bitcopy\n";
  t1.bitcopy();
  std::cout << "  [BL -- BitTupleTest] Testing field\n";
  t1.field();
  std::cout << "  [BL -- BitTupleTest] Testing basic\n";
  t1.basic();
  std::cout << "  [BL -- BitTupleTest] Testing big\n";
  t1.big();
  std::cout << "  [BL -- BitTupleTest] Testing structure\n";
  t1.structure();
  std::cout << "  [BL -- BitTupleTest] Testing nested\n";
  t1.nested();
  std::cout << "  [BL -- BitTupleTest] Testing locked\n";
  t1.locked();
  std::cout << "  [BL -- BitTupleTest] Testing assign\n";
  t1.assign();
  std::cout << "  [BL -- BitTupleTest] Testing operators\n";
  t1.operators();
  std::cout << "  [BL -- BitTupleTest] Testing ones\n";
  t1.ones();

  brick::t_bitlevel::BitVecTest t2;
  std::cout << "# " << typeid(t2).name() << '\n';
  std::cout << "  [BL -- BitVecTest] Testing bvpair_shiftl\n";
  t2.bvpair_shiftl();
  std::cout << "  [BL -- BitVecTest] Testing bvpair_shiftr\n";
  t2.bvpair_shiftr();
}

static void test_brick_hash()
{
  brick::t_hash::Jenkins t1;
  std::cout << "# " << typeid(t1).name() << '\n';
  std::cout << "  [H -- Jenkins] Testing results\n";
  t1.results();
  std::cout << "  [H -- Jenkins] Testing alignment\n";
  t1.alignment();
  std::cout << "  [H -- Jenkins] Testing pieces\n";
  t1.pieces();
}

static void test_brick_shmem()
{
  brick::t_shmem::ThreadTest t1;
  std::cout << "# " << typeid(t1).name() << '\n';
  std::cout << "  [S -- ThreadTest] Testing async_loop\n";
  // t1.async_loop();  // Triggers an alarm that break in test-suite
  std::cout << "  [S -- ThreadTest] Testing thread\n";
  // t1.thread();  // Triggers an alarm that break in test-suite
}

static void test_brick_types()
{
  brick::t_types::Mixins t1;
  std::cout << "# " << typeid(t1).name() << '\n';
  std::cout << "  [T -- Mixins] Testing comparable\n";
  t1.comparable();
  std::cout << "  [T -- Mixins] Testing eq\n";
  t1.eq();
  std::cout << "  [T -- Mixins] Testing ord\n";
  t1.ord();
  std::cout << "  [T -- Mixins] Testing eqord\n";
  t1.eqord();

  brick::t_types::UnionTest t2;
  std::cout << "# " << typeid(t2).name() << '\n';
  std::cout << "  [T -- UnionTest] Testing basic\n";
  t2.basic();
  std::cout << "  [T -- UnionTest] Testing moveNoCopy\n";
  t2.moveNoCopy();
  std::cout << "  [T -- UnionTest] Testing ctorCast\n";
  t2.ctorCast();
  std::cout << "  [T -- UnionTest] Testing eq\n";
  t2.eq();
  std::cout << "  [T -- UnionTest] Testing ord\n";
  t2.ord();

  brick::t_types::StrongEnumFlagsTest t3;
  std::cout << "# " << typeid(t3).name() << '\n';
  std::cout << "  [T -- StrongEnumFlagsTest] Testing regression\n";
  t3.regression();
  std::cout << "  [T -- StrongEnumFlagsTest] Testing enum_uchar\n";
  t3.enum_uchar();
  std::cout << "  [T -- StrongEnumFlagsTest] Testing enum_ushort\n";
  t3.enum_ushort();
  std::cout << "  [T -- StrongEnumFlagsTest] Testing enum_uint\n";
  t3.enum_uint();
  std::cout << "  [T -- StrongEnumFlagsTest] Testing enum_ulong\n";
  t3.enum_ulong();
}

int main()
{
  // Declare concurrent hash table
  brick::hashset::FastConcurrent<both , mytest_hasher_both<both>> ht2;

  // Declare workers and provide them some jobs.
  std::vector<std::thread> workers;
  for (int i = 0; i < 6; i++)
    workers.
      push_back(std::thread([ht2](int tid) mutable
                            {
                              for (int i = 0; i< 2000; ++i)
                                ht2.insert({i, tid});
                            }, i));

  // Wait the end of all threads.
  for (auto& t: workers)
    t.join();

  // Display the whole table.
  for (unsigned i = 0; i < ht2.capacity(); ++ i)
    if (ht2.valid(i))
      std::cout << i << ": {"
                << ht2.valueAt(i).x << ','
                << ht2.valueAt(i).y  << "}\n";

  // Run tests that are already embedded inside of bricks
  test_brick_hashmap();
  test_brick_bitlevel();
  test_brick_hash();
  test_brick_shmem();
  test_brick_types();
  return 0;
}
