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

# Test that the spot.gen package works, in particular, we want
# to make sure that the objects created from spot.gen methods
# are usable with methods from the spot package.


import spot
from unittest import TestCase
tc = TestCase()

aut5 = spot.automaton("""HOA: v1 States: 28 Start: 0 AP: 4 "alive" "b"
"a" "c" acc-name: Buchi Acceptance: 1 Inf(0) properties: trans-labels
explicit-labels state-acc very-weak --BODY-- State: 0 [0] 1 [0] 2 [0]
3 [0] 4 [0] 5 [0&!1] 6 [0] 7 State: 1 [0] 8 State: 2 [!0] 9 [0] 10
State: 3 [!0] 9 [0] 11 State: 4 [!0] 9 [0] 12 State: 5 [!0] 9 [0] 13
State: 6 [!0] 9 [0&!1] 14 State: 7 [!0] 9 [0&!1&!2] 14 State: 8 [0] 15
State: 9 {0} [!0] 9 State: 10 [!0] 9 [0] 16 State: 11 [!0] 9 [0] 17
State: 12 [!0] 9 [0] 18 State: 13 [!0] 9 [0&!1&!2] 19 State: 14 [!0] 9
[0&!1] 19 State: 15 [0] 20 State: 16 [!0] 9 [0] 21 State: 17 [!0] 9
[0] 22 State: 18 [!0] 9 [0&!1&!2] 23 State: 19 [!0] 9 [0&!1] 23 State:
20 [0] 24 State: 21 [!0] 9 [0] 25 State: 22 [!0] 9 [0&!1&!2] 26 State:
23 [!0] 9 [0&!1] 26 State: 24 [0&3] 27 State: 25 [!0] 9 [0&!1&!2] 27
State: 26 [!0] 9 [0&!1] 27 State: 27 [!0] 9 [0] 27 --END--""")

copy = spot.make_twa_graph(aut5, spot.twa_prop_set.all())

tc.assertFalse(spot.is_deterministic(aut5))
if spot.delay_branching_here(aut5):
    aut5.purge_unreachable_states()
    aut5.merge_edges()
tc.assertEqual(aut5.num_states(), 13)
tc.assertEqual(aut5.num_edges(), 29)
tc.assertTrue(spot.are_equivalent(copy, aut5))

a = spot.automaton("""HOA: v1 States: 8 Start: 0 AP: 3 "a" "b" "c"
Acceptance: 0 t --BODY-- State: 0 [0] 1 [0] 2 [0] 3 State: 1 [!1] 4&5
[1] 5&6 State: 2 [0] 4&6 State: 3 [0] 3&6 State: 4 [!0] 7 State: 5
[!0] 7 State: 6 [!0] 6 State: 7 [0] 7 --END--""")

copy = spot.make_twa_graph(a, spot.twa_prop_set.all())
if spot.delay_branching_here(a):
    a.purge_unreachable_states()
    a.merge_edges()
tc.assertEqual(a.to_str(), """HOA: v1
States: 7
Start: 0
AP: 3 "b" "a" "c"
acc-name: all
Acceptance: 0 t
properties: trans-labels explicit-labels state-acc univ-branch weak
--BODY--
State: 0
[1] 1
[1] 2
State: 1
[1] 3&5
[0] 4&5
[!0] 3&4
State: 2
[1] 2&5
State: 3
[!1] 6
State: 4
[!1] 6
State: 5
[!1] 5
State: 6
[1] 6
--END--""")

a = spot.automaton("""HOA: v1
States: 9
Start: 0 AP: 2 "a" "b"
spot.state-player: 0 1 1 0 0 0 0 1 1
Acceptance: 0 t
--BODY--
State: 0
[0] 1
[0] 2
[0] 3
[0] 4
State: 1
[1] 5
State: 2
[!1] 6
State: 3
[1] 7
State: 4
[!1] 8
State: 5
[t] 5
State: 6
[t] 6
State: 7
[t] 7
State: 8
[t] 8
--END--""")
copy = spot.make_twa_graph(a, spot.twa_prop_set.all())
if spot.delay_branching_here(a):
    a.purge_unreachable_states()
tc.assertTrue(spot.are_equivalent(a, copy))
tc.assertEqual(a.to_str(), """HOA: v1
States: 7
Start: 0
AP: 2 "b" "a"
acc-name: all
Acceptance: 0 t
properties: trans-labels explicit-labels state-acc very-weak
spot-state-player: 0 1 0 0 0 1 1
--BODY--
State: 0
[1] 1
[1] 2
State: 1
[0] 3
[!0] 4
State: 2
[0] 5
[!0] 6
State: 3
[t] 3
State: 4
[t] 4
State: 5
[t] 5
State: 6
[t] 6
--END--""")

# Running delay_branching_here on state-based acceptance may require
# the output to use transition-based acceptance.  (Issue #525.)
a = spot.automaton("""
HOA: v1 States: 4 Start: 0 AP: 2 "a" "b" Acceptance: 1 Inf(0) --BODY--
State: 0 [0] 1 [0] 2 State: 1 [1] 3 State: 2 {0} [!1] 3 State: 3 [t] 0
--END--""")
copy = spot.make_twa_graph(a, spot.twa_prop_set.all())
if spot.delay_branching_here(a):
    a.purge_unreachable_states()
tc.assertTrue(spot.are_equivalent(a, copy))
tc.assertEqual(a.to_str(), """HOA: v1
States: 3
Start: 0
AP: 2 "b" "a"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels trans-acc deterministic
--BODY--
State: 0
[1] 1
State: 1
[0] 2
[!0] 2 {0}
State: 2
[t] 0
--END--""")
