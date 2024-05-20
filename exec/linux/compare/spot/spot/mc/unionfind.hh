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

#include <spot/misc/common.hh>
#include <vector>

namespace spot
{

  /// \brief This Union-Find data structure is a particular
  /// union-find, dedicated for emptiness checks below, see ec.hh. The
  /// key of this union-find is int. Moreover, we suppose that only
  /// consecutive int are inserted. This union-find includes most of
  /// the classical optimisations (IPC, LR, PC, MS).
  class SPOT_API int_unionfind final
  {
  private:
    // Store the parent relation, i.e. -1 or x < id.size()
    std::vector<int> id;

    // id of a specially managed partition of "dead" elements
    const int DEAD = 0;

    int root(int i);

  public:
    int_unionfind();
    void makeset(int e);
    bool unite(int e1, int e2);
    void markdead(int e);
    bool sameset(int e1, int e2);
    bool isdead(int e);
  };
}
