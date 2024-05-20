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
#include <unordered_map>
#include <spot/misc/common.hh>
#include <spot/twacube/cube.hh>
#include <spot/twacube/twacube.hh>
#include <spot/twa/twagraph.hh>

namespace spot
{
  /// \brief Transform one truth assignment represented as a BDD
  /// into a \a cube cube passed in parameter. The parameter
  /// \a binder map bdd indexes to cube indexes.
  SPOT_API spot::cube satone_to_cube(bdd one, cubeset& cubeset,
                                     std::unordered_map<int, int>& binder);

  /// \brief Transform a \a cube cube into bdd using the map
  /// that bind cube indexes to bdd indexes.
  SPOT_API bdd cube_to_bdd(spot::cube cube, const cubeset& cubeset,
                           std::unordered_map<int, int>& reverse_binder);

  /// \brief Extract the atomic propositions from the automaton. This method
  /// also fill the binder, i.e. the mapping between BDD indexes to cube indexes
  SPOT_API std::vector<std::string>*
  extract_aps(spot::const_twa_graph_ptr aut,
              std::unordered_map<int, int>& ap_binder);

  /// \brief Convert a twa into a twacube
  SPOT_API twacube_ptr
  twa_to_twacube(spot::const_twa_graph_ptr aut);

  /// \brief Convert a twacube into a twa.
  /// When \a d is specified, the BDD_dict in parameter is used rather than
  /// creating a new one.
  SPOT_API spot::twa_graph_ptr
  twacube_to_twa(spot::twacube_ptr twacube,
                 spot::bdd_dict_ptr d = nullptr);

  /// \brief Check wether a twacube and a twa are equivalent
  SPOT_API bool are_equivalent(const spot::twacube_ptr twacube,
                               const spot::const_twa_graph_ptr twa);
}
