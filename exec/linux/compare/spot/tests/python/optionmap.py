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

o = spot.option_map()
res = o.parse_options("optA, opta=2M, optb =4 ; optB =       7\
                       ,   optC=  10")
tc.assertFalse(res)

tc.assertEqual(o.get('optA'), 1)
tc.assertEqual(o.get('opta'), 2*1024*1024)
tc.assertEqual(o.get('optb'), 4)
tc.assertEqual(o.get('optB'), 7)
tc.assertEqual(o.get('optC'), 10)
tc.assertEqual(o.get('none'), 0)
tc.assertEqual(o.get('none', 16), 16)

o.set('optb', 40)
tc.assertEqual(o.get('optb'), 40)

res = o.parse_options("!optA !optb optC, !optB")
tc.assertFalse(res)
tc.assertEqual(o.get('optA'), 0)
tc.assertEqual(o.get('opta'), 2*1024*1024)
tc.assertEqual(o.get('optb'), 0)
tc.assertEqual(o.get('optB'), 0)
tc.assertEqual(o.get('optC'), 1)

res = o.parse_options("!")
tc.assertEqual(res, "!")

res = o.parse_options("foo, !opt = 1")
tc.assertEqual(res, "!opt = 1")

res = o.parse_options("foo=3, opt == 1")
tc.assertEqual(res, "opt == 1")

res = o.parse_options("foo=3opt == 1")
tc.assertEqual(res, "3opt == 1")

aut1 = spot.translate('GF(a <-> XXa)', 'det')
tc.assertEqual(aut1.num_states(), 4)
aut2 = spot.translate('GF(a <-> XXa)', 'det', xargs='gf-guarantee=0')
tc.assertEqual(aut2.num_states(), 9)

try:
    spot.translate('GF(a <-> XXa)', 'det', xargs='foobar=1')
except RuntimeError as e:
    tc.assertIn("option 'foobar' was not used", str(e))
else:
    raise RuntimeError("missing exception")

try:
    spot.translate('GF(a <-> XXa)').postprocess('det', xargs='gf-guarantee=0')
except RuntimeError as e:
    tc.assertIn("option 'gf-guarantee' was not used", str(e))
else:
    raise RuntimeError("missing exception")

try:
    spot.translate('GF(a <-> XXa)').postprocess('det', xargs='gf-guarantee=x')
except RuntimeError as e:
    tc.assertIn("failed to parse option at: 'gf-guarantee=x'", str(e))
else:
    raise RuntimeError("missing exception")
