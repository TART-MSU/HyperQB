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
import spot
from unittest import TestCase
tc = TestCase()

e = spot.default_environment.instance()

l = [('GFa', False),
     ('a U (((b)) xor c)', False),
     ('!(FFx <=> Fx)', True),
     ('a \\/ a \\/ b \\/ a \\/ a', False)]


for str1, isl in l:
    pf = spot.parse_infix_psl(str1, e, False)
    if pf.format_errors(spot.get_cout()):
        sys.exit(1)
    str2 = str(pf.f)
    del pf
    # Try to reparse the stringified formula
    pf = spot.parse_infix_psl(str2, e)
    if pf.format_errors(spot.get_cout()):
        sys.exit(1)
    tc.assertEqual(isl, pf.f.is_leaf())
    del pf

tc.assertTrue(spot.formula('a').is_leaf())
tc.assertTrue(spot.formula('0').is_leaf())

for str1 in ['a * b', 'a xor b', 'a <-> b']:
    pf = spot.parse_infix_boolean(str1, e, False)
    if pf.format_errors(spot.get_cout()):
        sys.exit(1)
    str2 = str(pf.f)
    del pf
    # Try to reparse the stringified formula
    pf = spot.parse_infix_boolean(str2, e)
    if pf.format_errors(spot.get_cout()):
        sys.exit(1)
    del pf

for (x, op) in [('a* <-> b*', "`<->'"),
                ('a* -> b', "`->'"),
                ('a ^ b*', "`^'"),
                ('!(b*)', "`!'"),
                ('a*[=2]', "[=...]"),
                ('a*[->2]', "[->...]")]:
    f5 = spot.parse_infix_sere(x)
    tc.assertTrue(f5.errors)
    ostr = spot.ostringstream()
    f5.format_errors(ostr)
    err = ostr.str()
    tc.assertIn("not a Boolean expression", err)
    tc.assertIn(op, err)
    tc.assertIn("SERE", err)
    del f5

f6 = spot.parse_infix_sere('(a <-> b -> c ^ "b\n\n\rc")[=2] & c[->2]')
tc.assertFalse(f6.errors)
del f6

f6 = spot.parse_infix_sere('-')
tc.assertTrue(f6.errors)
del f6

for (x, msg) in [('{foo[->bug]}', "treating this goto block as [->]"),
                 ('{foo[->', 'missing closing bracket for goto operator'),
                 ('{foo[->3..1]}', "reversed range"),
                 ('{foo[*bug]}', "treating this star block as [*]"),
                 ('{foo[*bug', "missing closing bracket for star"),
                 ('{foo[*3..1]}', "reversed range"),
                 ('{[*3..1]}', "reversed range"),
                 ('{foo[:*bug]}', "treating this fusion-star block as [:*]"),
                 ('{foo[:*3..1]}', "reversed range"),
                 ('{foo[:*bug', "missing closing bracket for fusion-star"),
                 ('{foo[=bug]}', "treating this equal block as [=]"),
                 ('{foo[=bug', "missing closing bracket for equal operator"),
                 ('{foo[=3..1]}', "reversed range"),
                 ('{()}', "treating this brace block as false"),
                 ('{(a b)}', "treating this parenthetical block as false"),
                 ('{(a*}', "missing closing parenthesis"),
                 ('{(a*&)}', "missing right operand for "
                  + "\"non-length-matching and operator\""),
                 ('{(a*&&)}', "missing right operand for "
                  + "\"length-matching and operator\""),
                 ('{(a*|)}', "missing right operand for \"or operator\""),
                 ('{a*;}', "missing right operand for \"concat operator\""),
                 ('{a*::b}', "missing right operand for \"fusion operator\""),
                 ('{a* xor }', "missing right operand for \"xor operator\""),
                 ('{a* -> }', "missing right operand for "
                  + "\"implication operator\""),
                 ('{a <-> <-> b }',
                  "missing right operand for \"equivalent operator\""),
                 ('{a;b b}', "ignoring this"),
                 ('{*', "missing closing brace"),
                 ('{(a', "missing closing parenthesis"),
                 ('{* a',
                  "ignoring trailing garbage and missing closing brace"),
                 ('F(a b)', "ignoring this"),
                 ('F(-)', "treating this parenthetical block as false"),
                 ('F(', "missing closing parenthesis"),
                 ('F(a b', "ignoring trailing garbage and "
                  + "missing closing parenthesis"),
                 ('F(-', "missing closing parenthesis"),
                 ('F(-', "parenthetical block as false"),
                 ('a&', "missing right operand for \"and operator\""),
                 ('a*', "missing right operand for \"and operator\""),
                 ('a|', "missing right operand for \"or operator\""),
                 ('a^', "missing right operand for \"xor operator\""),
                 ('a->', "missing right operand for \"implication operator\""),
                 ('a<->', "missing right operand for \"equivalent operator\""),
                 ('!', "missing right operand for \"not operator\""),
                 ('a W', "missing right operand for \"weak until operator\""),
                 ('a U', "missing right operand for \"until operator\""),
                 ('a R', "missing right operand for \"release operator\""),
                 ('a M', "missing right operand for "
                  + "\"strong release operator\""),
                 ('{a}[]->', "missing right operand for "
                  + "\"universal overlapping concat operator\""),
                 ('{a}[]=>', "missing right operand for "
                  + "\"universal non-overlapping concat operator\""),
                 ('{a}<>->', "missing right operand for "
                  + "\"existential overlapping concat operator\""),
                 ('{a}<>=>', "missing right operand for "
                  + "\"existential non-overlapping concat operator\""),
                 ('(X)', "missing right operand for \"next operator\""),
                 ('("X)', "unclosed string"),
                 ('("X)', "missing closing parenthesis"),
                 ('{"X', "unclosed string"),
                 ('{"X}', "missing closing brace"),
                 ]:
    f7 = spot.parse_infix_psl(x)
    tc.assertTrue(f7.errors)
    ostr = spot.ostringstream()
    f7.format_errors(ostr)
    err = ostr.str()
    print(err)
    tc.assertIn(msg, err)
    del f7

for (x, msg) in [('a&', "missing right operand for \"and operator\""),
                 ('a*', "missing right operand for \"and operator\""),
                 ('a|', "missing right operand for \"or operator\""),
                 ('a^', "missing right operand for \"xor operator\""),
                 ('a->', "missing right operand for \"implication operator\""),
                 ('a<->', "missing right operand for \"equivalent operator\""),
                 ('(-', "parenthetical block as false"),
                 ('(-', "missing closing parenthesis"),
                 ('(-)', "treating this parenthetical block as false"),
                 ('(a', "missing closing parenthesis"),
                 ('(a b)', "ignoring this"),
                 ('(a b', "ignoring trailing garbage and "
                  + "missing closing parenthesis"),
                 ('!', "missing right operand for \"not operator\""),
                 ]:
    f8 = spot.parse_infix_boolean(x)
    tc.assertTrue(f8.errors)
    ostr = spot.ostringstream()
    f8.format_errors(ostr)
    err = ostr.str()
    print(err)
    tc.assertIn(msg, err)
    del f8

for (x, msg) in [('a->', "missing right operand for \"implication operator\""),
                 ('a<->', "missing right operand for \"equivalent operator\""),
                 ('(aa', "missing closing parenthesis"),
                 ('("aa', "missing closing parenthesis"),
                 ('"(aa', "unclosed string"),
                 ('{aa', "missing closing brace"),
                 ]:
    f9 = spot.parse_infix_psl(x, spot.default_environment.instance(),
                              False, True)
    tc.assertTrue(f9.errors)
    ostr = spot.ostringstream()
    f9.format_errors(ostr)
    err = ostr.str()
    print(err)
    tc.assertIn(msg, err)
    del f9

# force GC before fnode_instances_check(), unless it's CPython
from platform import python_implementation
if python_implementation() != 'CPython':
    import gc
    gc.collect()

tc.assertTrue(spot.fnode_instances_check())

f = spot.formula_F(2, 4, spot.formula_ap("a"))
tc.assertEqual(f, spot.formula("XX(a | X(a | X(a)))"))
f = spot.formula_G(2, 4, spot.formula_ap("a"))
tc.assertEqual(f, spot.formula("XX(a & X(a & X(a)))"))
f = spot.formula_X(2, spot.formula_ap("a"))
tc.assertEqual(f, spot.formula("XX(a)"))
f = spot.formula_G(2, spot.formula_unbounded(), spot.formula_ap("a"))
tc.assertEqual(f, spot.formula("XXG(a)"))
f = spot.formula_F(2, spot.formula_unbounded(), spot.formula_ap("a"))
tc.assertEqual(f, spot.formula("XXF(a)"))


pf = spot.parse_infix_sere('a[->3]')
tc.assertEqual(spot.str_sere(pf.f), 'a[->3]')
tc.assertEqual(spot.str_latex_sere(pf.f), 'a\\SereGoto{3}')
pf = spot.parse_infix_sere('(!b)[*];b;(!b)[*]')
tc.assertEqual(spot.str_sere(pf.f), 'b[=1]')
pf = spot.parse_infix_sere('b[=1]')
tc.assertEqual(spot.str_sere(pf.f), 'b[=1]')
tc.assertEqual(spot.str_latex_sere(pf.f), 'b\\SereEqual{1}')
tc.assertEqual(spot.str_sclatex_sere(pf.f), 'b^{=1}')
pf = spot.parse_infix_sere('(!b)[*];b')
tc.assertEqual(spot.str_sere(pf.f), 'b[->]')
pf = spot.parse_infix_sere('b[->1]')
tc.assertEqual(spot.str_latex_sere(pf.f), 'b\\SereGoto{}')
tc.assertEqual(spot.str_sclatex_sere(pf.f), 'b^{\\to}')
