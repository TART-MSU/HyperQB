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
#include "bin/common_conv.hh"
#include "bin/common_setup.hh"
#include "bin/common_output.hh"

#include <spot/ltsmin/ltsmin.hh>
#include <spot/ltsmin/spins_kripke.hh>
#include <spot/mc/mc_instanciator.hh>
#include <spot/twaalgos/dot.hh>
#include <spot/tl/defaultenv.hh>
#include <spot/tl/parse.hh>
#include <spot/twaalgos/translate.hh>
#include <spot/twaalgos/emptiness.hh>
#include <spot/twa/twaproduct.hh>

#include <algorithm>
#include <thread>
#include <spot/twacube/twacube.hh>
#include <spot/twacube_algos/convert.hh>
#include <spot/mc/utils.hh>

// FIXME (1) model (2)formula
int main(int argc, char** argv)
{
  // Build the formula
  char* formula = argv[2];
  spot::formula f = nullptr;
  spot::default_environment& env = spot::default_environment::instance();
  spot::atomic_prop_set ap;
  spot::const_twa_graph_ptr prop = nullptr;
  auto dict = spot::make_bdd_dict();
  {
    auto pf = spot::parse_infix_psl(formula, env, false);
    f = pf.f;
    spot::translator trans(dict);
    prop = trans.run(&f);
  }

  atomic_prop_collect(f, &ap);

  auto propcube = spot::twa_to_twacube(prop);

  // Load model
  spot::ltsmin_kripkecube_ptr modelcube = nullptr;
  try
    {
      modelcube = spot::ltsmin_model::load(argv[1])
        .kripkecube(propcube->ap(), spot::formula::tt(), 0, 1);
    }
  catch (const std::runtime_error& e)
    {
      std::cerr << e.what() << '\n';
    }

  // ------------------------------------------------------------------
  // check that cube and bdd approaches agree for the size of the model
  // ------------------------------------------------------------------
  auto result0 =
    spot::ec_instanciator<spot::ltsmin_kripkecube_ptr, spot::cspins_state,
                          spot::cspins_iterator, spot::cspins_state_hash,
                          spot::cspins_state_equal>
    (spot::mc_algorithm::REACHABILITY, modelcube, propcube, false);
  std::cout << "[CUBE] Model: "
            << result0.states[0] << ','
            << result0.transitions[0]
            << '\n';


  spot::kripkecube_to_twa<spot::cspins_state,
                          spot::cspins_iterator,
                          spot::cspins_state_hash,
                          spot::cspins_state_equal> ktt(*modelcube, dict);
  ktt.run();
  auto twa_from_model = ktt.twa();
  std::cout << "[BDD]  Model: "
            << twa_from_model->num_states() << ','
            << twa_from_model->num_edges() << '\n';



  // ------------------------------------------------------------------
  // check that cube and bdd approaches agree for the emptiness and the
  // size of the synchronous product
  // ------------------------------------------------------------------
  auto result1 =
    spot::ec_instanciator<spot::ltsmin_kripkecube_ptr, spot::cspins_state,
                          spot::cspins_iterator, spot::cspins_state_hash,
                          spot::cspins_state_equal>
    (spot::mc_algorithm::BLOEMEN_EC, modelcube, propcube, false);
  auto result2 =
    spot::ec_instanciator<spot::ltsmin_kripkecube_ptr, spot::cspins_state,
                          spot::cspins_iterator, spot::cspins_state_hash,
                          spot::cspins_state_equal>
    (spot::mc_algorithm::REACHABILITY, modelcube, propcube, false);

  std::cout << "[CUBE] Product: "
            << result1.value[0] << ','
            << result1.states[0] << ','
            << result1.transitions[0]
            << '\n';

  spot::product_to_twa<spot::cspins_state,
                       spot::cspins_iterator,
                       spot::cspins_state_hash,
                       spot::cspins_state_equal> ptt(*modelcube, propcube);
  ptt.run();
  auto twa_from_product = ptt.twa();
  std::cout << "[BDD]  Product: "
            << (twa_from_product->is_empty() ? "empty" : "not_empty")
            << ','
            << twa_from_product->num_states() << ','
            << twa_from_product->num_edges()
            << '\n';
}
