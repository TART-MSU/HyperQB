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

#include <spot/kripke/kripke.hh>
#include <spot/twacube/twacube.hh>
#include <queue>

namespace spot
{
  /// \brief Find the first couple of iterator (from a given pair of
  /// iterators) that intersect. This method can be used in any
  /// DFS/BFS-like exploration algorithm. The \a parameter indicates
  /// whether the state has just been visited since the underlying job
  /// is slightly different.
  template<typename SuccIterator, typename State>
  static void forward_iterators(kripkecube<State, SuccIterator>& sys,
                                twacube_ptr twa,
                                SuccIterator* it_kripke,
                                std::shared_ptr<trans_index> it_prop,
                                bool just_visited,
                                unsigned tid)
  {
    (void) sys; // System is useless, but the API is clearer
    SPOT_ASSERT(!(it_prop->done() && it_kripke->done()));

    // Sometimes kripke state may have no successors.
    if (it_kripke->done())
      return;

    // The state has just been visited  and the 2 iterators intersect.
    // There is no need to move iterators forward.
    SPOT_ASSERT(!(it_prop->done()));
    if (just_visited && twa->get_cubeset()
        .intersect(twa->trans_data(it_prop, tid).cube_,
                   it_kripke->condition()))
      return;

    // Otherwise we have to compute the next valid successor (if it exits).
    // This requires two loops. The most inner one is for the twacube since
    // its costless
    if (it_prop->done())
      it_prop->reset();
    else
      it_prop->next();

    while (!it_kripke->done())
      {
        while (!it_prop->done())
          {
            if (SPOT_UNLIKELY(twa->get_cubeset()
                              .intersect(twa->trans_data(it_prop, tid).cube_,
                                         it_kripke->condition())))
              return;
            it_prop->next();
          }
        it_prop->reset();
        it_kripke->next();
      }
  }
}
