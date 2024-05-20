#!/usr/bin/python3
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


# Test case reduced from a report from Juraj Major <major@fi.muni.cz>.
a = spot.make_twa_graph(spot._bdd_dict)
a.set_acceptance(0, spot.acc_code("t"))
tc.assertTrue(a.prop_state_acc())
a.set_acceptance(1, spot.acc_code("Fin(0)"))
tc.assertEqual(a.prop_state_acc(), spot.trival.maybe())


# Some tests for used_inf_fin_sets(), which return a pair of mark_t.
(inf, fin) = a.get_acceptance().used_inf_fin_sets()
tc.assertEqual(inf, [])
tc.assertEqual(fin, [0])
(inf, fin) = spot.acc_code("(Fin(0)|Inf(1))&Fin(2)&Inf(0)").used_inf_fin_sets()
tc.assertEqual(inf, [0, 1])
tc.assertEqual(fin, [0, 2])

# is_rabin_like() returns (bool, [(inf, fin), ...])
(b, v) = spot.acc_cond("(Fin(0)&Inf(1))|(Fin(2)&Inf(0))").is_rabin_like()
tc.assertTrue(b)
tc.assertEqual(len(v), 2)
tc.assertEqual(v[0].fin, [0])
tc.assertEqual(v[0].inf, [1])
tc.assertEqual(v[1].fin, [2])
tc.assertEqual(v[1].inf, [0])
(b, v) = spot.acc_cond("(Fin(0)|Inf(1))&(Fin(2)|Inf(0))").is_rabin_like()
tc.assertFalse(b)
tc.assertEqual(len(v), 0)
(b, v) = spot.acc_cond("(Fin(0)|Inf(1))&(Fin(2)|Inf(0))").is_streett_like()
tc.assertTrue(b)
tc.assertEqual(repr(v), \
    '(spot.rs_pair(fin=[0], inf=[1]), spot.rs_pair(fin=[2], inf=[0]))')
v2 = (spot.rs_pair(fin=[0], inf=[1]), spot.rs_pair(fin=[2], inf=[0]))
tc.assertEqual(v, v2)

acc = spot.acc_cond("generalized-Rabin 1 2")
(b, v) = acc.is_generalized_rabin()
tc.assertTrue(b)
tc.assertEqual(v, (2,))
(b, v) = acc.is_generalized_streett()
tc.assertFalse(b)
tc.assertEqual(v, ())
(b, v) = acc.is_streett_like()
tc.assertTrue(b)
ve = (spot.rs_pair([0], []), spot.rs_pair([], [1]), spot.rs_pair([], [2]))
tc.assertEqual(v, ve)
tc.assertEqual(acc.name(), "generalized-Rabin 1 2")

# At the time of writting, acc_cond does not yet recognize
# "generalized-Streett", as there is no definition for that in the HOA format,
# and defining it as follows (dual for gen.Rabin) would prevent Streett from
# being a generalized-Streett.  See issue #249.
acc = spot.acc_cond("Inf(0)|Fin(1)|Fin(2)")
(b, v) = acc.is_generalized_streett()
tc.assertTrue(b)
tc.assertEqual(v, (2,))
(b, v) = acc.is_generalized_rabin()
tc.assertFalse(b)
tc.assertEqual(v, ())
# FIXME: We should have a way to disable the following output, as it is not
# part of HOA v1.
tc.assertEqual(acc.name(), "generalized-Streett 1 2")


# issue #469.  This test is meaningful only if Spot is compiled with
# --enable-max-accsets=64 or more.
try:
    m = spot.mark_t([33])
    tc.assertEqual(m.lowest(), m)
    n = spot.mark_t([33,34])
    tc.assertEqual(n.lowest(), m)
except RuntimeError as e:
    if "Too many acceptance sets used." in str(e):
        pass
    else:
        raise e


# issue #468
from gc import collect
acc = spot.translate('a').acc()
collect()
tc.assertEqual(acc, spot.acc_cond('t'))
acc = spot.translate('b').get_acceptance()
collect()
tc.assertEqual(acc, spot.acc_code('t'))


c = spot.acc_cond('Fin(0)&Fin(1)&(Inf(2)|Fin(3))')
m1 = c.fin_unit()
m2 = c.inf_unit()
tc.assertEqual(m1, [0,1])
tc.assertEqual(m2, [])
c = spot.acc_cond('Inf(0)&Inf(1)&(Inf(2)|Fin(3))')
m1 = c.fin_unit()
m2 = c.inf_unit()
tc.assertEqual(m1, [])
tc.assertEqual(m2, [0,1])
c = spot.acc_cond('Inf(0)&Inf(1)|(Inf(2)|Fin(3))')
m1 = c.fin_unit()
m2 = c.inf_unit()
tc.assertEqual(m1, [])
tc.assertEqual(m2, [])
