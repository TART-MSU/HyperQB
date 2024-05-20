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
#include <spot/mc/unionfind.hh>
#include <cassert>

namespace spot
{
  int int_unionfind::root(int i)
  {
    assert(i > 0);
    int p = id[i];
    if (p == DEAD)
      return DEAD;
    if (p < 0)
      return i;
    int gp = id[p];
    if (gp == DEAD)
      return DEAD;
    if (gp < 0)
      return p;
    p = root(p);
    id[i] = p;
    return p;
  }

  int_unionfind::int_unionfind() : id()
  {
    id.push_back(DEAD);
  }

  void int_unionfind::makeset(int e)
  {
    assert(e == (int) id.size());
    (void)e;
    id.push_back(-1);
  }

  bool int_unionfind::unite(int e1, int e2)
  {
    // IPC - Immediate Parent Check
    int p1 = id[e1];
    int p2 = id[e2];
    if ((p1 < 0 ? e1 : p1) == (p2 < 0 ? e2 : p2))
      return false;
    int root1 = root(e1);
    int root2 = root(e2);
    if (root1 == root2)
      return false;
    int rk1 = -id[root1];
    int rk2 = -id[root2];
    if (rk1 < rk2)
      id[root1] = root2;
    else {
      id[root2] = root1;
      if (rk1 == rk2)
        id[root1] = -(rk1 + 1);
    }
    return true;
  }

  void int_unionfind::markdead(int e)
  {
    id[root(e)] = DEAD;
  }

  bool int_unionfind::sameset(int e1, int e2)
  {
    assert(e1 < (int)id.size() && e2 < (int)id.size());
    return root(e1) == root(e2);
  }

  bool int_unionfind::isdead(int e)
  {
    assert(e < (int)id.size());
    return root(e) == DEAD;
  }
}
