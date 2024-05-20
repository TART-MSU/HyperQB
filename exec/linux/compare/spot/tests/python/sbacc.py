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

aut = spot.translate('GFa')
tc.assertEqual(aut.num_states(), 1)
tc.assertFalse(aut.prop_state_acc().is_true())
aut = spot.sbacc(aut)
tc.assertEqual(aut.num_states(), 2)
tc.assertTrue(aut.prop_state_acc().is_true())

aut = spot.automaton("""HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
Acceptance: 2 Inf(0) | Inf(1)
properties: trans-labels explicit-labels trans-acc
--BODY--
State: 0
[0&1] 1
[0&!1] 2
State: 1
[t] 1 {0}
[0] 0
State: 2
[t] 2 {1}
[0] 1
--END--""")

s = spot.sbacc(aut)
s.copy_state_names_from(aut)
h = s.to_str('hoa')

tc.assertEqual(h, """HOA: v1
States: 2
Start: 0
AP: 2 "a" "b"
Acceptance: 2 Inf(0) | Inf(1)
properties: trans-labels explicit-labels state-acc deterministic
--BODY--
State: 0 "0"
[0] 1
State: 1 "2" {1}
[t] 1
--END--""")

aut = spot.automaton("""HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
Acceptance: 2 Inf(0) & Inf(1)
properties: trans-labels explicit-labels trans-acc
--BODY--
State: 0
[0&1] 1
[0&!1] 2
State: 1
[t] 1 {0 1}
[0] 0
State: 2
[t] 2 {1 0}
[0] 1
--END--""")

d = spot.degeneralize(aut)
d.copy_state_names_from(aut)
h = d.to_str('hoa')

tc.assertEqual(h, """HOA: v1
States: 2
Start: 0
AP: 2 "a" "b"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc deterministic
--BODY--
State: 0 "0#0"
[0] 1
State: 1 "2#0" {0}
[t] 1
--END--""")
