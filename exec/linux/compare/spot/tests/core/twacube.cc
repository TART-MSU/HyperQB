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

#include <spot/tl/parse.hh>
#include <spot/twaalgos/translate.hh>
#include <spot/twaalgos/contains.hh>

#include <iostream>
#include <spot/twacube/twacube.hh>
#include <spot/twacube_algos/convert.hh>
#include <iostream>
#include <spot/twa/twagraph.hh>
#include <spot/twaalgos/dot.hh>
#include <spot/tl/defaultenv.hh>

static void checkit(std::string f_str)
{
  spot::default_environment& env = spot::default_environment::instance();
  auto pf = spot::parse_infix_psl(f_str, env, false);
  spot::formula f = pf.f;
  auto dict = spot::make_bdd_dict();
  spot::translator trans(dict);
  auto prop = trans.run(&f);

  auto propcube = spot::twa_to_twacube(prop);
  assert(spot::are_equivalent(propcube, prop));

  auto propcubeback = spot::twacube_to_twa(propcube, dict);
  assert(spot::are_equivalent(propcube, propcubeback));

  assert(spot::are_equivalent(prop, propcubeback));
}

int main()
{
  // Build some TWA
  auto d = spot::make_bdd_dict();
  auto tg = make_twa_graph(d);
  bdd p1 = bdd_ithvar(tg->register_ap("p1"));
  bdd p2 = bdd_ithvar(tg->register_ap("p2"));
  tg->acc().add_sets(2);
  auto s1 = tg->new_state();
  auto s2 = tg->new_state();
  auto s3 = tg->new_state();
  tg->new_edge(s1, s1, bddfalse, {});
  tg->new_edge(s1, s2, p1, {});
  tg->new_edge(s1, s3, p2, tg->acc().mark(1));
  tg->new_edge(s2, s3, p1 & p2, tg->acc().mark(0));
  tg->new_edge(s3, s1, p1 | p2, spot::acc_cond::mark_t({0, 1}));
  tg->new_edge(s3, s2, p1 >> p2, {});
  tg->new_edge(s3, s3, bddtrue, spot::acc_cond::mark_t({0, 1}));

  // Test translation
  auto aut = twa_to_twacube(tg);
  assert(spot::are_equivalent(aut, tg));

  spot::print_dot(std::cout, tg, "A");
  std::cout << "-----------\n" << *aut << "-----------\n";

  const std::vector<std::string>& aps = aut->ap();
  unsigned int seed = 17;
  auto it = aut->succ(2);
  SPOT_ASSUME(it); // GCC 7 warns about potential nullptr.
  for (; !it->done(); it->next())
    {
      auto& t = aut->trans_storage(it, seed);
      auto& d = aut->trans_data(it, seed);
      std::cout << t.src << ' ' << t.dst << ' '
                << ' ' << aut->get_cubeset().dump(d.cube_, aps)
                << ' ' << d.acc_
                << std::endl;
    }
  spot::print_dot(std::cout, spot::twacube_to_twa(aut), "A");

  // Now check the equivalence of multiple translations
  checkit("F!p8 U (((!p11 & p17 & Gp4) | (!p17 & (p11 | F!p4))) R (p4 U p2))");
  checkit("F(((p9 & F!p12) | (!p9 & Gp12)) R (G!p7 & ((p16 R p5) M p4)))");
  checkit("Gp14 U ((Fp11 W (p13 R p6)) M ((Fp16 & Fp10) | G(!p10 & !p16)))");
  checkit("F(((p9 & F!p12) | (!p9 & Gp12)) R (G!p7 & ((p16 R p5) M p4)))");
  checkit("Gp14 U ((Fp11 W (p13 R p6)) M ((Fp16 & Fp10) | G(!p10 & !p16)))");
}
