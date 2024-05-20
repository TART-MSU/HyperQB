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

a = spot.acc_cond(5)
a.set_acceptance(spot.acc_code('parity min odd 5'))
tc.assertEqual(a.is_parity(), [True, False, True])
a.set_acceptance('parity max even 5')
tc.assertEqual(a.is_parity(), [True, True, False])
a.set_acceptance('generalized-Buchi 5')
tc.assertEqual(a.is_parity()[0], False)
tc.assertEqual(a.is_parity(True)[0], False)
a.set_acceptance('Inf(4) | (Fin(3)&Inf(2)) | (Fin(3)&Fin(1)&Inf(0))')
tc.assertEqual(a.is_parity()[0], False)
tc.assertEqual(a.is_parity(True), [True, True, False])

tc.assertTrue(a.maybe_accepting([1, 2, 3], [0, 4]).is_true())
tc.assertTrue(a.maybe_accepting([0], []).is_true())
tc.assertTrue(a.maybe_accepting([0], [3]).is_false())
tc.assertTrue(a.maybe_accepting([0, 3], []).is_maybe())
tc.assertTrue(a.maybe_accepting([2, 3], [3]).is_false())
tc.assertTrue(a.maybe_accepting([2, 3], []).is_maybe())
tc.assertTrue(a.maybe_accepting([2], []).is_true())
tc.assertTrue(a.maybe_accepting([0, 1], []).is_maybe())
tc.assertTrue(a.maybe_accepting([0, 1], [1]).is_false())

a.set_acceptance('Fin(0)|Fin(1)')
tc.assertTrue(a.maybe_accepting([0, 1], [1]).is_maybe())
tc.assertTrue(a.maybe_accepting([0, 1], [0, 1]).is_false())
tc.assertTrue(a.maybe_accepting([0], []).is_true())
tc.assertTrue(a.maybe_accepting([], [0]).is_true())

a = spot.acc_cond(0)
a.set_acceptance('all')
tc.assertEqual(a.is_rabin(), -1)
tc.assertEqual(a.is_streett(), 0)
tc.assertEqual(a.is_parity(), [True, True, True])
a.set_acceptance('none')
tc.assertEqual(a.is_rabin(), 0)
tc.assertEqual(a.is_streett(), -1)
tc.assertEqual(a.is_parity(), [True, True, False])

a = spot.acc_cond('(Fin(0)&Inf(1))')
tc.assertEqual(a.is_rabin(), 1)
tc.assertEqual(a.is_streett(), -1)
a.set_acceptance('Inf(1)&Fin(0)')
tc.assertEqual(a.is_rabin(), 1)
tc.assertEqual(a.is_streett(), -1)
a.set_acceptance('(Fin(0)|Inf(1))')
tc.assertEqual(a.is_rabin(), -1)
tc.assertEqual(a.is_streett(), 1)
a.set_acceptance('Inf(1)|Fin(0)')
tc.assertEqual(a.is_rabin(), -1)
tc.assertEqual(a.is_streett(), 1)

a = spot.acc_cond('(Fin(0)&Inf(1))|(Fin(2)&Inf(3))')
tc.assertEqual(a.is_rabin(), 2)
tc.assertEqual(a.is_streett(), -1)
a.set_acceptance(spot.acc_code('(Inf(3)&Fin(2))|(Fin(0)&Inf(1))'))
tc.assertEqual(a.is_rabin(), 2)
tc.assertEqual(a.is_streett(), -1)
a.set_acceptance(spot.acc_code('(Inf(2)&Fin(3))|(Fin(0)&Inf(1))'))
tc.assertEqual(a.is_rabin(), -1)
tc.assertEqual(a.is_streett(), -1)
a.set_acceptance(spot.acc_code('(Inf(3)&Fin(2))|(Fin(2)&Inf(1))'))
tc.assertEqual(a.is_rabin(), -1)
tc.assertEqual(a.is_streett(), -1)
a.set_acceptance(spot.acc_code('(Inf(1)&Fin(0))|(Fin(0)&Inf(1))'))
tc.assertEqual(a.is_rabin(), -1)
tc.assertEqual(a.is_streett(), -1)
a.set_acceptance('(Fin(0)&Inf(1))|(Inf(1)&Fin(0))|(Inf(3)&Fin(2))')
tc.assertEqual(a.is_rabin(), 2)
tc.assertEqual(a.is_streett(), -1)

a.set_acceptance('(Fin(0)|Inf(1))&(Fin(2)|Inf(3))')
tc.assertEqual(a.is_rabin(), -1)
tc.assertEqual(a.is_streett(), 2)
a.set_acceptance('(Inf(3)|Fin(2))&(Fin(0)|Inf(1))')
tc.assertEqual(a.is_rabin(), -1)
tc.assertEqual(a.is_streett(), 2)
a.set_acceptance('(Inf(2)|Fin(3))&(Fin(0)|Inf(1))')
tc.assertEqual(a.is_rabin(), -1)
tc.assertEqual(a.is_streett(), -1)
a.set_acceptance('(Inf(3)|Fin(2))&(Fin(2)|Inf(1))')
tc.assertEqual(a.is_rabin(), -1)
tc.assertEqual(a.is_streett(), -1)
a.set_acceptance('(Inf(1)|Fin(0))&(Fin(0)|Inf(1))')
tc.assertEqual(a.is_rabin(), -1)
tc.assertEqual(a.is_streett(), -1)
a.set_acceptance('(Fin(0)|Inf(1))&(Inf(1)|Fin(0))&(Inf(3)|Fin(2))')
tc.assertEqual(a.is_rabin(), -1)
tc.assertEqual(a.is_streett(), 2)

a = spot.acc_code('Inf(0)&Inf(1)&Inf(3) | Fin(0)&(Fin(1)|Fin(3))')
u = a.symmetries()
tc.assertEqual(u[0], 0)
tc.assertEqual(u[1], 1)
tc.assertEqual(u[2], 2)
tc.assertEqual(u[3], 1)
