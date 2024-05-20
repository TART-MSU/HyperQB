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


# Tests different ways to explore a BDD label, as discussed in
# https://lists.lrde.epita.fr/hyperkitty/list/spot@lrde.epita.fr/\
# message/WIAOWDKKPXTTK6UBE3MAHDFWIT36EUMX/
import spot
import buddy
import sys
from unittest import TestCase
tc = TestCase()

# CPython use reference counting, so that automata are destructed
# when we expect them to be.   However other implementations like
# PyPy may call destructors latter, causing different output.
from platform import python_implementation
if python_implementation() == 'CPython':
    def gcollect():
        pass
else:
    import gc
    def gcollect():
        gc.collect()


run = spot.translate('a & !b').accepting_run()
b = run.prefix[0].label
c = buddy.bdd_satone(b)
tc.assertNotEqual(c, buddy.bddfalse)
res = []
while c != buddy.bddtrue:
    var = buddy.bdd_var(c)
    h = buddy.bdd_high(c)
    if h == buddy.bddfalse:
        res.append(-var)
        c = buddy.bdd_low(c)
    else:
        res.append(var)
        c = h

tc.assertEqual(res, [0, -1])
del res

res2 = []
for i in run.aut.ap():
    res2.append((str(i), run.aut.register_ap(i)))
tc.assertEqual(str(res2), "[('a', 0), ('b', 1)]")
del res2
del c
gcollect()

res3 = []
for i in buddy.minterms_of(buddy.bddtrue, run.aut.ap_vars()):
    res3.append(str(spot.bdd_to_formula(i)))
tc.assertEqual(res3, ['!a & !b', '!a & b', 'a & !b', 'a & b'])
gcollect()

f = spot.bdd_to_formula(b)
tc.assertTrue(f._is(spot.op_And))
tc.assertTrue(f[0]._is(spot.op_ap))
tc.assertTrue(f[1]._is(spot.op_Not))
tc.assertTrue(f[1][0]._is(spot.op_ap))
tc.assertEqual(str(f), 'a & !b')
del f
gcollect()

try:
    f = spot.bdd_to_formula(b, spot.make_bdd_dict())
    sys.exit(2)
except RuntimeError as e:
    tc.assertIn("not in the dictionary", str(e))

f = spot.bdd_to_cnf_formula(b)
tc.assertEqual(str(f), 'a & !b')

del run
del f

gcollect()

f = spot.bdd_to_cnf_formula(buddy.bddtrue)
tc.assertEqual(str(f), '1')
del f
gcollect()

f = spot.bdd_to_cnf_formula(buddy.bddfalse)
tc.assertEqual(str(f), '0')
del f
gcollect()

aut = spot.translate('(a & b) <-> c')
# With pypy, running GC here will destroy the translator object used
# by translate().  That object has temporary automata that reference
# the BDDs variables and those affect the order in which the
# bdd_to_formula() result is object is presented.  The different order
# is not wrong, but it makes it diffuclt to write tests.
gcollect()

for e in aut.out(aut.get_init_state_number()):
    b = e.cond
    break

f1 = spot.bdd_to_formula(b)
tc.assertEqual(str(f1), '(!a & !c) | (a & b & c) | (!b & !c)')
f2 = spot.bdd_to_cnf_formula(b)
tc.assertEqual(str(f2), '(a | !c) & (!a | !b | c) & (b | !c)')

b1 = spot.formula_to_bdd(f1, spot._bdd_dict, aut)
b2 = spot.formula_to_bdd(f2, spot._bdd_dict, aut)
tc.assertEqual(b1, b2)
