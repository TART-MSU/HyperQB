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

max_even_5 = spot.acc_code.parity(True, False, 5)
tc.assertEqual(max_even_5, spot.acc_code.parity_max_even(5))
tc.assertEqual(max_even_5, spot.acc_code.parity_max(False, 5))
min_even_5 = spot.acc_code.parity(False, False, 5)
tc.assertEqual(min_even_5, spot.acc_code.parity_min_even(5))
tc.assertEqual(min_even_5, spot.acc_code.parity_min(False, 5))

max_odd_5 = spot.acc_code.parity(True, True, 5)
tc.assertEqual(max_odd_5, spot.acc_code.parity_max_odd(5))
tc.assertEqual(max_odd_5, spot.acc_code.parity_max(True, 5))
min_odd_5 = spot.acc_code.parity(False, True, 5)
tc.assertEqual(min_odd_5, spot.acc_code.parity_min_odd(5))
tc.assertEqual(min_odd_5, spot.acc_code.parity_min(True, 5))


for f in ('FGa', 'GFa & GFb & FGc', 'XXX(a U b)'):
    a1 = spot.translate(f, 'parity')
    tc.assertTrue(a1.acc().is_parity())
    a2 = spot.translate(f).postprocess('parity')
    tc.assertTrue(a2.acc().is_parity())
    a3 = spot.translate(f, 'det').postprocess('parity', 'colored')
    tc.assertTrue(a3.acc().is_parity())
    tc.assertTrue(spot.is_colored(a3))

a = spot.translate('GFa & GFb')
try:
    spot.change_parity_here(a, spot.parity_kind_same, spot.parity_style_even)
except RuntimeError as e:
    tc.assertIn('input should have parity acceptance', str(e))
else:
    exit(2)

a = spot.automaton("""
HOA: v1
States: 1
Start: 0
AP: 1 "a"
Acceptance: 2 Fin(0) & Inf(1)
--BODY--
State: 0
[t] 0 {0}
--END--
""")
spot.cleanup_parity_here(a)
tc.assertEqual(a.to_str(), """HOA: v1
States: 1
Start: 0
AP: 1 "a"
acc-name: none
Acceptance: 0 f
properties: trans-labels explicit-labels state-acc complete
properties: deterministic
--BODY--
State: 0
[t] 0
--END--""")

a = spot.automaton("""
HOA: v1
States: 1
Start: 0
AP: 1 "a"
Acceptance: 2 Fin(0) | Inf(1)
--BODY--
State: 0
[t] 0 {1}
--END--
""")
spot.cleanup_parity_here(a)
tc.assertEqual(a.to_str(), """HOA: v1
States: 1
Start: 0
AP: 1 "a"
acc-name: all
Acceptance: 0 t
properties: trans-labels explicit-labels state-acc complete
properties: deterministic
--BODY--
State: 0
[t] 0
--END--""")

a = spot.automaton("""HOA: v1
States: 3
Start: 0
AP: 1 "a"
acc-name: all
Acceptance: 0 t
properties: trans-labels explicit-labels state-acc deterministic
--BODY--
State: 0
[0] 1
State: 1
[0] 2
State: 2
--END--""")

try:
	spot.get_state_players(a)
except RuntimeError as e:
  tc.assertIn("not a game", str(e))
else:
  report_missing_exception()

try:
    spot.set_state_player(a, 1, True)
except RuntimeError as e:
    tc.assertIn("Can only", str(e))
else:
    report_missing__exception()
spot.set_state_players(a, (False, True, False))
tc.assertEqual(spot.get_state_player(a, 0), False)
tc.assertEqual(spot.get_state_player(a, 1), True)
tc.assertEqual(spot.get_state_player(a, 2), False)

try:
	spot.set_state_players(a, [True, False, False, False])
except RuntimeError as e:
  tc.assertIn("many owners as states", str(e))
else:
  report_missing_exception()

try:
	spot.get_state_player(a, 4)
except RuntimeError as e:
  tc.assertIn("invalid state number", str(e))
else:
  report_missing_exception()

try:
	spot.set_state_player(a, 4, True)
except RuntimeError as e:
  tc.assertIn("invalid state number", str(e))
else:
  report_missing_exception()


a = spot.automaton("""HOA: v1 name: "F(!p0 | X!p1)" States: 3
Start: 1 AP: 2 "p0" "p1" acc-name: Buchi Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels trans-acc complete
properties: deterministic terminal --BODY-- State: 0 [t] 0 {0} State:
1 [!0] 0 [0] 2 State: 2 [!0 | !1] 0 [0&1] 2 --END--""")
# Erase the first edge of state 1
oi = a.out_iteraser(1)
oi.erase()
# postprocess used to call reduce_parity that did not
# work correctly on automata with deleted edges.
sm = a.postprocess("gen", "small")
tc.assertEqual(sm.num_states(), 3)
