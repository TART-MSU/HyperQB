# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2009, 2010, 2012, 2015, 2018, 2021-2022 Laboratoire de
# Recherche et Développement de l'Epita (LRDE).
# Copyright (C) 2003, 2004 Laboratoire d'Informatique de Paris 6 (LIP6),
# département Systemes Répartis Coopératifs (SRC), Université Pierre
# et Marie Curie.
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
import sys
from unittest import TestCase
tc = TestCase()

# Some of the tests here assume timely destructor calls, as they occur
# in the the reference-counted CPython implementation.  Other
# implementation such as PyPy, should skip those tests.
from platform import python_implementation
is_cpython = python_implementation() == 'CPython'

# ----------------------------------------------------------------------
a = spot.formula.ap('a')
b = spot.formula.ap('b')
c = spot.formula.ap('c')
c2 = spot.formula.ap('c')

tc.assertEqual(c, c2)

op = spot.formula.And([a, b])
op2 = spot.formula.And([op, c])
op3 = spot.formula.And([a, c, b])

tc.assertEqual(op2, op3)

# The symbol for a subformula which hasn't been cloned is better
# suppressed, so we don't attempt to reuse it elsewhere.
del op, c

sys.stdout.write('op2 = %s\n' % str(op2))

del a, b, c2

sys.stdout.write('op3 = %s\n' % str(op3))
tc.assertEqual(op2, op3)

op4 = spot.formula.Or([op2, op3])

sys.stdout.write('op4 = %s\n' % str(op4))
tc.assertEqual(op4, op2)

del op2, op3, op4

# ----------------------------------------------------------------------
a = spot.formula.ap('a')
b = spot.formula.ap('b')
c = spot.formula.ap('c')
T = spot.formula.tt()
F = spot.formula.ff()

f1 = spot.formula.Equiv(c, a)
f2 = spot.formula.Implies(a, b)
f3 = spot.formula.Xor(b, c)
f4 = spot.formula.Not(f3)
del f3
f5 = spot.formula.Xor(F, c)

del a, b, c, T, F, f1, f2, f4, f5

if is_cpython:
    tc.assertTrue(spot.fnode_instances_check())

# ----------------------------------------------------------------------
tc.assertEqual(str([str(x) for x in spot.formula('a &b & c')]),
               "['a', 'b', 'c']")


def switch_g_f(x):
    if x._is(spot.op_G):
        return spot.formula.F(switch_g_f(x[0]))
    if x._is(spot.op_F):
        return spot.formula.G(switch_g_f(x[0]))
    return x.map(switch_g_f)


f = spot.formula('GFa & XFGb & Fc & G(a | b | Fd)')
tc.assertEqual(str(switch_g_f(f)), 'FGa & XGFb & Gc & F(a | b | Gd)')

x = 0


def count_g(f):
    global x
    if f._is(spot.op_G):
        x += 1


f.traverse(count_g)
tc.assertEqual(x, 3)

# ----------------------------------------------------------------------

# The example from tut01.org

formula = spot.formula('a U b U "$strange[0]=name"')
res = """\
Default output:    {f}
Spin syntax:       {f:s}
(Spin syntax):     {f:sp}
Default for shell: echo {f:q} | ...
LBT for shell:     echo {f:lq} | ...
Default for CSV:   ...,{f:c},...
Wring, centered:   {f:w:~^50}""".format(f=formula)

tc.assertEqual(res, """\
Default output:    a U (b U "$strange[0]=name")
Spin syntax:       a U (b U ($strange[0]=name))
(Spin syntax):     (a) U ((b) U ($strange[0]=name))
Default for shell: echo 'a U (b U "$strange[0]=name")' | ...
LBT for shell:     echo 'U "a" U "b" "$strange[0]=name"' | ...
Default for CSV:   ...,"a U (b U ""$strange[0]=name"")",...
Wring, centered:   ~~~~~(a=1) U ((b=1) U ("$strange[0]=name"=1))~~~~~""")


opt = spot.tl_simplifier_options(False, True, True,
                                 True, True, True,
                                 False, False, False)

for (input, output) in [('(a&b)<->b', 'b->(a&b)'),
                        ('b<->(a&b)', 'b->(a&b)'),
                        ('(a&b)->b', '1'),
                        ('b->(a&b)', 'b->(a&b)'),
                        ('(!(a&b)) xor b', 'b->(a&b)'),
                        ('(a&b) xor !b', 'b->(a&b)'),
                        ('b xor (!(a&b))', 'b->(a&b)'),
                        ('!b xor (a&b)', 'b->(a&b)')]:
    f = spot.tl_simplifier(opt).simplify(input)
    tc.assertEqual(f, output)
    tc.assertTrue(spot.are_equivalent(input, output))


def myparse(input):
    env = spot.default_environment.instance()
    pf = spot.parse_infix_psl(input, env)
    return pf.f


# This used to fail, because myparse would return a pointer
# to pf.f inside the destroyed pf.
tc.assertEqual(myparse('a U b'), spot.formula('a U b'))

tc.assertTrue(spot.is_liveness('a <-> GFb'))
tc.assertFalse(spot.is_liveness('a & GFb'))
