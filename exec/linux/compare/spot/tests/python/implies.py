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

import sys
from buddy import *
from unittest import TestCase
tc = TestCase()

bdd_init(10000, 10000)
bdd_setvarnum(5)

V = [bdd_ithvar(i) for i in range(5)]

a = V[0] & V[1] & -V[2]
b = V[0] & V[1] & -V[2] & -V[3]
c = -V[0] & V[1] & -V[2] & -V[3]
d = V[1] & -V[2]
e = V[1] & V[2] & -V[3] & V[4]
f = V[0] & -V[3] & V[4]
g = -V[0] | V[1]

tc.assertTrue(bdd_implies(b, a))
tc.assertFalse(bdd_implies(a, b))
tc.assertFalse(bdd_implies(c, a))
tc.assertTrue(bdd_implies(a, d))
tc.assertTrue(bdd_implies(b, d))
tc.assertTrue(bdd_implies(c, d))
tc.assertTrue(bdd_implies(d, d))
tc.assertFalse(bdd_implies(e, d))
tc.assertFalse(bdd_implies(d, e))
tc.assertFalse(bdd_implies(f, e))
tc.assertFalse(bdd_implies(e, f))
tc.assertTrue(bdd_implies(bddfalse, f))
tc.assertFalse(bdd_implies(bddtrue, f))
tc.assertTrue(bdd_implies(f, bddtrue))
tc.assertFalse(bdd_implies(f, bddfalse))
tc.assertTrue(bdd_implies(a, g))

a = (-V[2] & (-V[1] | V[0])) | (-V[0] & V[1] & V[2])
b = V[1] | -V[2]
tc.assertTrue(bdd_implies(a, b))

# Cleanup all BDD variables before calling bdd_done(), otherwise
# bdd_delref will be called after bdd_done() and this is unsafe in
# optimized builds.
V = a = b = c = d = e = f = g = 0
bdd_done()
