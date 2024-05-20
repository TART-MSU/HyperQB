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

def do_test(subset, superset, expected=True):
    result = spot.contains_forq(superset, subset)
    truth = spot.contains(superset, subset)
    tc.assertTrue(truth == expected)
    tc.assertTrue(result == truth)

def do_symmetric_test(subset, superset):
    do_test(subset, superset, True)
    do_test(superset, subset, False)

always_true = spot.automaton("""
HOA: v1
States: 1
Start: 0
acc-name: Buchi
Acceptance: 1 Inf(0)
AP: 2 "a" "b"
--BODY--
State: 0
[t] 0 {0}
--END--""")

one = spot.automaton("""
HOA: v1
States: 2
Start: 0
acc-name: Buchi
Acceptance: 1 Inf(0)
AP: 2 "a" "b"
--BODY--
State: 0
[0|1] 1
State: 1
[0] 1
[!0] 1 {0}
--END--""")

both = spot.automaton("""
HOA: v1
States: 1
Start: 0
acc-name: Buchi
Acceptance: 1 Inf(0)
AP: 2 "a" "b"
--BODY--
State: 0
[0] 0 {0}
[!0] 0 {0}
--END--""")

do_test(both, always_true)
do_test(always_true, both)
do_symmetric_test(one, always_true)

superset = spot.automaton("""
HOA: v1
States: 3
Start: 0
acc-name: Buchi
Acceptance: 1 Inf(0)
AP: 2 "a" "b"
--BODY--
State: 0
[!0|!1] 1
[0 & 1] 2
State: 1
[t] 1 {0}
State: 2
[t] 2 {0}
--END--""")

subset = spot.automaton("""
HOA: v1
States: 3
Start: 0
acc-name: Buchi
Acceptance: 1 Inf(0)
AP: 2 "a" "b"
--BODY--
State: 0
[!0] 1
[!0&1 | 0&!1] 2
State: 1
[t] 1 {0}
State: 2
[t] 2 {0}
--END--""")

do_symmetric_test(subset, superset)

subset = spot.automaton("""
HOA: v1
States: 1
Start: 0
AP: 1 "__ap832"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc
--BODY--
State: 0 {0}
[!0] 0
--END--""")

superset = spot.automaton("""
HOA: v1
States: 1
Start: 0
AP: 1 "__ap832"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc
--BODY--
State: 0 {0}
[0] 0
--END--""")

do_test(subset, superset, False)
do_test(superset, subset, False)

subset = spot.automaton("""
HOA: v1
States: 2
Start: 0
acc-name: Buchi
Acceptance: 1 Inf(0)
AP: 2 "a" "b"
--BODY--
State: 0
[!0|!1] 1
State: 1
[t] 1 {0}
--END--""")
superset = spot.automaton("""
HOA: v1
States: 3
Start: 0
acc-name: Buchi
Acceptance: 1 Inf(0)
AP: 2 "a" "b"
--BODY--
State: 0
[!0&1 | !0&!1] 1
[!0&1 | 0&!1] 2
State: 1
[t] 1 {0}
State: 2
[t] 2 {0}
--END--""")

# Equivlent Languages
do_test(subset, superset)
do_test(superset, subset)

superset = spot.automaton("""
HOA: v1
States: 20
Start: 0
AP: 4 "__ap876" "__ap877" "__ap878" "__ap879"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc
--BODY--
State: 0
[0&!1&2&!3] 1
State: 1
[!0&!1&!2] 2
[!0&1&!2] 3
[!0&1&!2] 4
[!0&!1&!2&!3] 5
[!0&1&!2&!3] 6
State: 2
[!0&!1&!2] 2
[!0&!1&!2&!3] 5
[!0&1&!2&!3] 6
[!0&1&!2] 7
State: 3
[!0&!1&!2] 3
[0&!1&2&!3] 4
State: 4
[!0&!1&!2] 4
[!0&!2&!3] 6
[!0&1&!2] 7
State: 5
[!0&!2&3] 5
[!0&!2&!3] 6
State: 6
[!0&!1&!2&3] 5
[!0&!1&!2&!3 | !0&1&!2&3] 6
[!0&1&!2&!3] 8
State: 7
[!0&!2&!3] 6
[!0&!2] 7
[0&!1&2&!3] 9
State: 8
[!0&!2&3] 6
[!0&!2&!3] 8
[0&!1&2&!3] 10
State: 9
[!0&!2&!3] 6
[!0&!1&!2] 9
[!0&1&!2] 11
State: 10
[!0&!1&!2&!3] 12
[!0&1&!2&!3] 13
State: 11
[!0&!2&!3] 6
[!0&!2] 11
[0&!1&2&!3] 14
State: 12
[!0&!1&!2&!3] 12
[!0&1&!2&!3] 15
State: 13
[0&!1&2&!3] 12
[!0&!1&!2&!3] 13
[!0&1&!2&!3] 15
State: 14
[!0&!1&!2&!3] 6
[!0&1&!2&!3] 8
[!0&!1&!2] 9
[!0&1&!2] 16
State: 15
[!0&!2&!3] 15
[0&!1&2&!3] 17
State: 16
[!0&!1&!2&!3] 6
[!0&1&!2&!3] 8
[!0&!1&!2] 11
[0&!1&2&!3] 14
[!0&1&!2] 16
State: 17
[!0&!1&!2&!3] 17
[!0&1&!2&!3] 18
State: 18
[!0&!2&!3] 18
[0&!1&2&!3] 19
State: 19 {0}
[!0&!1&!2&!3] 17
[!0&1&!2&!3] 18
--END--""")

subset = spot.automaton("""
HOA: v1
States: 12
Start: 0
AP: 4 "__ap876" "__ap877" "__ap878" "__ap879"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc
--BODY--
State: 0
[0&!1&2&!3] 1
State: 1
[!0&!1&!2&!3] 1
[!0&!1&!2&3] 2
[!0&1&!2&!3] 3
[!0&1&!2&3] 4
State: 2
[!0&!1&!2] 2
[!0&1&!2&3] 4
[!0&1&!2&!3] 5
State: 3
[!0&!2&!3] 3
[!0&!1&!2&3] 4
[!0&1&!2&3] 5
[0&!1&2&!3] 6
State: 4
[!0&!1&!2 | !0&!2&3] 4
[!0&1&!2&!3] 5
State: 5
[!0&!1&!2&3] 4
[!0&1&!2 | !0&!2&!3] 5
[0&!1&2&!3] 7
State: 6
[!0&!1&!2&3] 2
[!0&1&!2&!3] 3
[!0&1&!2&3] 5
[!0&!1&!2&!3] 8
[!0&!1&!2&!3] 9
[!0&1&!2&!3] 10
State: 7
[!0&!1&!2&!3] 1
[!0&!1&!2&3] 2
[!0&1&!2&!3] 3
[!0&1&!2&3] 4
[!0&1&!2&!3] 10
State: 8
[!0&!1&!2&!3] 8
[!0&1&!2&!3] 10
State: 9
[!0&!1&!2&3] 2
[!0&1&!2&!3] 3
[!0&1&!2&3] 5
[!0&!1&!2&!3] 9
State: 10
[!0&!2&!3] 10
[0&!1&2&!3] 11
State: 11 {0}
[!0&!1&!2&!3] 8
[!0&1&!2&!3] 10
--END--""")

do_symmetric_test(subset, superset)


tba = spot.translate('GFa')
tgba = spot.translate('GFa & GFb')
tc.assertTrue(spot.contains(tba, tgba))
try:
    spot.containment_select_version("fork")
except RuntimeError as e:
    tc.assertIn("forq", str(e))
else:
    raise RuntimeError("missing exception")
spot.containment_select_version("forq")
tc.assertTrue(spot.contains(tba, tgba))  # does not call contains_forq
try:
    spot.contains_forq(tba, tgba)  # because contains_forq wants Büchi
except RuntimeError as e:
    tc.assertIn("Büchi", str(e))
else:
    raise RuntimeError("missing exception")

# This shows that exclusive word also depend on
# containment_select_version()
tc.assertEqual(str(one.exclusive_word(both)), "!a & !b; cycle{a}")
spot.containment_select_version("default")
tc.assertEqual(str(one.exclusive_word(both)), "cycle{a}")

tba2 = spot.translate('GFa & GFb', "buchi")
spot.containment_select_version("default")
tc.assertTrue(spot.contains(tba, tba2))
tc.assertFalse(spot.contains(tba2, tba))
spot.containment_select_version("forq")
tc.assertTrue(spot.contains(tba, tba2))
tc.assertFalse(spot.contains(tba2, tba))

a = spot.translate("(p0 & p2) -> G!p1", "buchi")
b = spot.translate("p0 -> G!p1", "buchi")
do_symmetric_test(b, a)

# issue #575
aut1 = spot.automaton("""HOA: v1.1 States: 12 Start: 0 AP: 6 "p13"
"p12" "p15" "p16" "p14" "p11" acc-name: Buchi Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels trans-acc !complete
properties: !deterministic exist-branch --BODY-- State: 0 [!0&!1&!3 |
!2 | !3&!4] 1 [!0&!1&3 | !2 | 3&!4] 2 [0&!3&4 | 1&!3&4 | !2] 3 [0&3&4
| 1&3&4 | !2] 4 [0&!1&3&4 | !2] 6 State: 1 [t] 7 State: 2 [!0&!1&3&!5
| !2 | 3&!4&!5] 2 {0} [0&3&4&!5 | 1&3&4&!5 | !2] 4 [0&!1&3&4&!5 | !2]
6 State: 3 [t] 5 State: 4 [!2 | 3&!4&!5] 2 {0} [!2 | 3&4&!5] 4 State:
5 [t] 9 State: 6 [!0&!1&3&!5 | !0&3&!4&!5 | !2] 2 {0} [!0&1&3&4&!5 |
!2] 4 [0&!1&3&!5 | 0&3&!4&!5 | !2] 6 [0&1&3&4&!5 | !2] 10 State: 7 [t]
11 State: 8 [t] 8 {0} State: 9 [t] 8 {0} State: 10 [!0&3&!4&!5 | !2] 2
{0} [!0&3&4&!5 | !2] 4 [0&3&!4&!5 | !2] 6 [0&3&4&!5 | !2] 10 State: 11
[t] 8 {0} --END--""")
aut2 = spot.automaton("""HOA: v1 States: 5 Start: 0 AP: 6 "p13" "p12"
"p15" "p16" "p14" "p11" acc-name: Buchi Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels trans-acc --BODY-- State: 0
[!0&!1&2&3 | 2&3&!4] 1 [0&2&3&4 | 1&2&3&4] 2 [0&!1&2&3&4] 3 State: 1
[!0&!1&2&3&!5 | 2&3&!4&!5] 1 {0} [0&2&3&4&!5 | 1&2&3&4&!5] 2
[0&!1&2&3&4&!5] 3 State: 2 [2&3&!4&!5] 1 {0} [2&3&4&!5] 2 State: 3
[!0&!1&2&3&!5 | !0&2&3&!4&!5] 1 {0} [!0&1&2&3&4&!5] 2 [0&!1&2&3&!5 |
0&2&3&!4&!5] 3 [0&1&2&3&4&!5] 4 State: 4 [!0&2&3&!4&!5] 1 {0}
[!0&2&3&4&!5] 2 [0&2&3&!4&!5] 3 [0&2&3&4&!5] 4 --END--""")
do_symmetric_test(aut2, aut1)
