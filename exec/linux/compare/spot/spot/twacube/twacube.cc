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
#include "twacube.hh"
#include <iostream>

namespace spot
{
  cstate::cstate(cstate&&) noexcept
  { }

  transition::transition(transition&& t) noexcept:
  cube_(std::move(t.cube_)), acc_(std::move(t.acc_))
  { }

  transition::transition(const cube& cube,
                         acc_cond::mark_t acc):
    cube_(cube),  acc_(acc)
  { }

  twacube::twacube(const std::vector<std::string> aps):
    init_(0U), aps_(aps), cubeset_(aps.size())
  { }

  twacube::~twacube()
  {
    const spot::cubeset cs = get_cubeset();
    for (unsigned i = 1; i <= theg_.num_edges(); ++i)
      cs.release(theg_.edge_data(i).cube_);
  }

  acc_cond& twacube::acc()
  {
    return acc_;
  }

  std::vector<std::string> twacube::ap() const
  {
    return aps_;
  }

  unsigned twacube::new_state()
  {
    return theg_.new_state();
  }

  void twacube::set_initial(unsigned init)
  {
    init_ = init;
  }

  unsigned twacube::get_initial() const
  {
    if (theg_.num_states() == 0)
      throw std::runtime_error("automaton has no state at all");

    return init_;
  }

  cstate* twacube::state_from_int(unsigned i)
  {
    return &theg_.state_data(i);
  }

  void
  twacube::create_transition(unsigned src, const cube& cube,
                             const acc_cond::mark_t& mark, unsigned dst)
  {
    theg_.new_edge(src, dst, cube, mark);
  }

  const cubeset&
  twacube::get_cubeset()  const
  {
    return cubeset_;
  }

  bool
  twacube::succ_contiguous() const
  {
    unsigned i = 1;
    while (i <= theg_.num_edges())
      {
        unsigned j = i;

        // Walk first bucket of successors
        while (j <= theg_.num_edges() &&
               theg_.edge_storage(i).src == theg_.edge_storage(j).src)
          ++j;

        // Remove the next bucket
        unsigned itmp = j;

        // Look if there are some transitions missing in this bucket.
        while (j <= theg_.num_edges())
          {
            if (theg_.edge_storage(i).src == theg_.edge_storage(j).src)
              return false;
            ++j;
          }
        i = itmp;
      }
    return true;
  }

  std::ostream&
  operator<<(std::ostream& os, const twacube& twa)
  {
    spot::cubeset cs = twa.get_cubeset();
    os << "init : " << twa.init_ << '\n';
     for (unsigned i = 1; i <= twa.theg_.num_edges(); ++i)
       os << twa.theg_.edge_storage(i).src << "->"
          << twa.theg_.edge_storage(i).dst <<  " : "
          << cs.dump(twa.theg_.edge_data(i).cube_, twa.aps_)
          << ' ' << twa.theg_.edge_data(i).acc_
          << '\n';
    return os;
  }
}
