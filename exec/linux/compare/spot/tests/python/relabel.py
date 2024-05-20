# -*- mode: python; coding: utf-8 -*-
# Copyright (C) by the Spot authors, see the AUTHORS file for details.
#
# This file is part of Spot, a model checking library.
#
# Spot is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# Spot is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
# License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import spot
from unittest import TestCase
tc = TestCase()

f = spot.formula('GF(a & b) -> (FG(a & b) & Gc)')
m = spot.relabeling_map()
g = spot.relabel_bse(f, spot.Pnn, m)
res = ""
for old, new in m.items():
    res += f"#define {old} {new}\n"
res += str(g)
tc.assertEqual(res, """#define p0 a & b
#define p1 c
GFp0 -> (FGp0 & Gp1)""")

h = spot.relabel_apply(g, m)
tc.assertEqual(h, f)

autg = g.translate()
spot.relabel_here(autg, m)
tc.assertEqual(str(autg.ap()), \
    '(spot.formula("a"), spot.formula("b"), spot.formula("c"))')
tc.assertTrue(spot.isomorphism_checker.are_isomorphic(autg, f.translate()))

a = spot.formula('a')
u = spot.formula('a U b')
m[a] = u
try:
    spot.relabel_here(autg, m)
except RuntimeError as e:
    tc.assertIn("new labels", str(e))

m = spot.relabeling_map()
m[u] = a
try:
    spot.relabel_here(autg, m)
except RuntimeError as e:
    tc.assertIn("old labels", str(e))

f = spot.parse_infix_sere("(p9;p21|p22):(p1&p2;p11&p22;p1&p2)").f
g = spot.relabel_bse(f, spot.Abc)
tc.assertEqual(str(g), "(a;(b | c)):(d;(c & e);d)")
