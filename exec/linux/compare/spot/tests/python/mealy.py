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

import spot, buddy
from unittest import TestCase
tc = TestCase()

# Testing Sat-based approach

# Empty bdd intersection bug test
# See issue #472
true = buddy.bddtrue

a = spot.make_twa_graph()
o1 = buddy.bdd_ithvar(a.register_ap("o1"))
o2 = buddy.bdd_ithvar(a.register_ap("o2"))
no1 = buddy.bdd_not(o1)
no2 = buddy.bdd_not(o2)
a.new_states(6)
a.new_edge(0,1,true)
a.new_edge(1,2,no1)
a.new_edge(2,3,true)
a.new_edge(3,4,no2)
a.new_edge(4,5,true)
a.new_edge(5,4,(o1&no2)|(no1&o2))
spot.set_state_players(a, [False,True,False,True,False,True])
spot.set_synthesis_outputs(a, o1&o2)

b = spot.minimize_mealy(a)
tc.assertEqual(list(spot.get_state_players(b)).count(False), 2)
tc.assertTrue(spot.is_split_mealy_specialization(a, b))

test_auts = [
("""HOA: v1
States: 22
Start: 0
AP: 6 "i0" "i1" "i2" "i3" "o0" "o1"
acc-name: all
Acceptance: 0 t
properties: trans-labels explicit-labels state-acc deterministic
spot-state-player: 0 1 0 1 0 1 0 1 0 1 1 0 1 0 1 0 1 1 0 1 0 1
--BODY--
State: 0
[!2&3] 1
[2&!3] 1
[!2&!3] 1
[!0&!1&2&3] 1
[1&2&3] 3
[0&!1&2&3] 5
State: 1
[!4&!5] 0
State: 2
[!2&3] 3
[2&!3] 3
[!2&!3] 3
[!0&!1&2&3] 1
[1&2&3] 7
[0&!1&2&3] 5
State: 3
[!4&!5] 2
State: 4
[!2&3] 5
[2&!3] 5
[!2&!3] 5
[!0&!1&2&3] 1
[1&2&3] 3
[0&!1&2&3] 14
State: 5
[!4&!5] 4
State: 6
[!2&3] 7
[2&!3] 7
[!2&!3] 7
[!0&!1&2&3] 3
[1&2&3] 9
[0&!1&2&3] 5
State: 7
[!4&!5] 6
State: 8
[!2&3] 10
[2&!3] 10
[!2&!3] 10
[!0&!1&2&3] 12
[1&2&3] 10
[0&!1&2&3] 5
State: 9
[!4&!5] 8
State: 10
[4&!5] 8
State: 11
[!2&3] 12
[2&!3] 12
[!2&!3] 12
[!0&!1&2&3] 19
[1&2&3] 3
[0&!1&2&3] 5
State: 12
[!4&!5] 11
State: 13
[!2&3] 14
[2&!3] 14
[!2&!3] 14
[!0&!1&2&3] 5
[1&2&3] 3
[0&!1&2&3] 16
State: 14
[!4&!5] 13
State: 15
[!2&3] 17
[2&!3] 17
[!2&!3] 17
[!0&!1&2&3] 12
[1&2&3] 3
[0&!1&2&3] 17
State: 16
[!4&!5] 15
State: 17
[!4&5] 15
State: 18
[!2&3] 19
[2&!3] 19
[!2&!3] 19
[!0&!1&2&3] 21
[1&2&3] 3
[0&!1&2&3] 5
State: 19
[!4&!5] 18
State: 20
[!2&3] 21
[2&!3] 21
[!2&!3] 21
[!0&!1&2&3] 1
[1&2&3] 3
[0&!1&2&3] 5
State: 21
[!4&!5] 20
--END--""", 7),
("""HOA: v1
States: 39
Start: 0
AP: 14 "i0" "i1" "i2" "i3" "i4" "i5" "i6" "o0" "o1" "o2" "o3" "o4" "o5" "o6"
acc-name: all
Acceptance: 0 t
properties: trans-labels explicit-labels state-acc
spot-state-player: 0 1 0 1 1 0 1 1 1 0 1 0 1 1 1 0 1 0 1 0 1 1 0 1 0 1 1 0 1 0
                   1 1 1 1 0 1 0 0 0
--BODY--
State: 0
[!0] 1
[0&!1&!6] 3
[0&!1&6] 4
[0&1&!6] 6
[0&1&6] 7
State: 1
[!7&!8&!9&!10&!11&!12&!13] 0
State: 2
[0&!1&!2] 8
[0&!1&2&4] 10
[0&!1&2&!4] 12
[!0] 13
[0&1] 14
State: 3
[!7&!8&9&10&!11&!13] 2
State: 4
[!7&!8&!9&10&!11&!13] 2
State: 5
[!0&!5] 31
[0&1&!5] 32
[!0&5] 33
[0&!1] 8
[0&1&5] 35
State: 6
[!7&!8&9&10&!11&12&!13] 5
State: 7
[!7&!8&!9&10&!11&12&!13] 5
State: 8
[!7&!8&!9&!10&!11&!13] 2
State: 9
[0&!1] 18
[!0] 13
[0&1] 14
State: 10
[7&!8&!9&!10&!11&!13] 9
State: 11
[0&!1] 16
[!0] 13
[0&1] 14
State: 12
[7&!8&!9&!10&!11&!13] 11
State: 13
[!7&!8&!9&12&!13] 5
State: 14
[!7&!8&!9&!10&!11&12&!13] 5
State: 15
[0&!1&!4] 12
[0&!1&4] 10
[!0] 13
[0&1] 14
State: 16
[!7&!8&!9&!10&!11&!13] 15
State: 17
[0&!1&3] 10
[0&!1&4] 10
[0&!1&!3&!4] 20
[!0] 13
[0&1] 14
State: 18
[!7&!8&!9&!10&!11&!13] 17
State: 19
[0&!1&!5] 21
[0&!1&5] 23
[!0] 13
[0&1] 14
State: 20
[!7&8&!9&!10&!11&12&!13] 19
State: 21
[!7&8&!9&!10&!11&!12&!13] 19
State: 22
[0&!1] 25
[!0] 13
[0&1] 14
State: 23
[!7&8&!9&!10&!11&!13] 22
State: 24
[0&!1&!5] 26
[0&!1&5] 28
[!0] 13
[0&1] 14
State: 25
[!7&!8&!9&!10&!11&12&!13] 24
State: 26
[!7&!8&!9&!10&!11&!12&!13] 24
State: 27
[0&!1] 30
[!0] 13
[0&1] 14
State: 28
[7&!8&!9&!10&!11&!13] 27
State: 29
[0&!1&!2&3] 30
[0&!1&!3&4] 4
[0&!1&!3&!4] 20
[0&!1&2&3] 28
[!0] 13
[0&1] 14
State: 30
[!7&!8&!9&!10&!11&!13] 29
State: 31
[!7&!8&!9&!12&!13] 5
State: 32
[!7&!8&!9&!10&!11&!12&!13] 5
State: 33
[!7&!8&!9&13] 0
State: 34
[0&1] 35
[0&!1] 8
[!0] 13
State: 35
[!7&!8&!9&!10&11&!13] 34
State: 36
[!0] 13
State: 37
[!0] 13
State: 38
[!0] 13
--END--""", 13),
("""HOA: v1
States: 33
Start: 0
AP: 3 "i0" "i1" "o0"
acc-name: all
Acceptance: 0 t
properties: trans-labels explicit-labels state-acc deterministic
spot-state-player: 0 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0
                   1 0 1
--BODY--
State: 0
[0&!1] 2
State: 1
[0&!1] 4
State: 2
[2] 1
State: 3
[!0&!1] 6
State: 4
[2] 3
State: 5
[0&!1] 8
State: 6
[!2] 5
State: 7
[!0&!1] 10
State: 8
[2] 7
State: 9
[!0&1] 12
State: 10
[!2] 9
State: 11
[0&!1] 14
State: 12
[!2] 11
State: 13
[!0&!1] 16
State: 14
[!2] 13
State: 15
[!0&!1] 18
State: 16
[2] 15
State: 17
[!0&!1] 20
State: 18
[!2] 17
State: 19
[!0&!1] 22
State: 20
[2] 19
State: 21
[0&!1] 24
State: 22
[!2] 21
State: 23
[0&!1] 26
State: 24
[2] 23
State: 25
[!0&1] 28
State: 26
[2] 25
State: 27
[!0&!1] 30
State: 28
[2] 27
State: 29
[!0&1] 32
State: 30
[!2] 29
State: 31
State: 32
[!2] 31
--END--""", 3)
]

for (mealy_str, nenv_min) in test_auts:

    mealy = spot.automaton(mealy_str)
    mealy.merge_edges()

    outs = buddy.bddtrue
    ins = buddy.bddtrue

    for aap in mealy.ap():
        if aap.ap_name().startswith("o"):
            outs = outs & buddy.bdd_ithvar(mealy.register_ap(aap.ap_name()))
        elif aap.ap_name().startswith("i"):
            ins = ins & buddy.bdd_ithvar(mealy.register_ap(aap.ap_name()))
        else:
            raise AssertionError("""Aps must start with either "i" or "o".""")

    spot.set_synthesis_outputs(mealy, outs)

    mealy_min_ks = spot.minimize_mealy(mealy, -1)

    n_e = sum([s == 0 for s in spot.get_state_players(mealy_min_ks)])
    tc.assertEqual(n_e, nenv_min)
    tc.assertTrue(spot.is_split_mealy_specialization(mealy, mealy_min_ks))

    # Test un- and resplit
    tmp = spot.unsplit_2step(mealy_min_ks)
    mealy_min_rs = spot.split_2step(tmp, spot.get_synthesis_outputs(tmp), False)
    tc.assertTrue(spot.is_split_mealy_specialization(mealy, mealy_min_rs, True))
    tc.assertTrue(spot.are_equivalent(mealy_min_ks, mealy_min_rs))


# Testing bisimulation (with output assignment)


# Syntcomp: Alarm_2d1010f8.tlsf
aut = spot.automaton("""HOA: v1
States: 17
Start: 0
AP: 6 "u02alarm29control02alarm29control"
"u02alarm29control0f1d2alarm29turn2off1b"
"u02alarm29control0f1d2alarm29turn2on1b" "p0p0off02alarm29intent"
"p0p0on02alarm29intent" "p0b02alarm29alarm"
acc-name: all
Acceptance: 0 t
properties: trans-labels explicit-labels state-acc deterministic
--BODY--
State: 0
[!0&!1&2&!3&!4 | !0&!1&2&3&4 | !0&1&!2&!3&!4 | !0&1&!2&3&4 | 0&!1&!2&!3&!4
               | 0&!1&!2&3&4] 1
[!0&!1&2&!3&4&!5 | !0&!1&2&3&!4&5] 2
[!0&!1&2&!3&4&5 | !0&!1&2&3&!4&!5] 3
State: 1
[!0&!1&2 | !0&1&!2 | 0&!1&!2] 1
State: 2
[!0&!1&2&!3&!4 | !0&!1&2&3&4 | !0&1&!2&!3&!4 | !0&1&!2&3&4 | 0&!1&!2&!3&!4
               | 0&!1&!2&3&4] 4
[!0&!1&2&!3&4&!5] 5
[!0&!1&2&3&!4&!5] 6
[!0&!1&2&!3&4&5] 7
[!0&1&!2&3&!4&5] 8
State: 3
[!0&!1&2&3&!4&5] 2
[!0&!1&2&!3&!4 | !0&!1&2&3&4 | !0&1&!2&!3&!4 | !0&1&!2&3&4 | 0&!1&!2&!3&!4
               | 0&!1&!2&3&4] 4
[!0&!1&2&!3&4&!5] 5
[!0&!1&2&3&!4&!5] 6
[!0&!1&2&!3&4&5] 7
State: 4
[!0&!1&2&!3&!4 | !0&!1&2&3&4 | !0&!1&2&!4&5 | !0&1&!2&!3&!4 | !0&1&!2&3&4
               | !0&1&!2&!4&5 | 0&!1&!2&!3&!4 | 0&!1&!2&3&4 | 0&!1&!2&!4&5] 4
[!0&!1&2&!3&4 | !0&1&!2&!3&4 | 0&!1&!2&!3&4] 9
[!0&!1&2&3&!4&!5 | !0&1&!2&3&!4&!5 | 0&!1&!2&3&!4&!5] 10
State: 5
[!0&!1&2&!3&!4 | !0&1&!2&!3&!4 | 0&!1&!2&!3&!4] 4
[!0&!1&2&!3&4] 5
[!0&!1&2&3&4 | !0&1&!2&3&4 | 0&!1&!2&3&4] 9
[!0&!1&2&3&!4] 11
State: 6
[!0&!1&2&!3&!4 | !0&!1&2&3&4&5 | !0&1&!2&!3&!4 | !0&1&!2&3&4&5 | 0&!1&!2&!3&!4
               | 0&!1&!2&3&4&5] 4
[!0&!1&2&!3&4] 5
[!0&!1&2&3&!4&!5] 6
[!0&!1&2&3&!4&5] 11
[!0&!1&2&3&4&!5 | !0&1&!2&3&4&!5 | 0&!1&!2&3&4&!5] 10
State: 7
[!0&!1&2&3&!4&5] 2
[!0&!1&2&!3&!4 | !0&1&!2&!3&!4 | 0&!1&!2&!3&!4] 4
[!0&!1&2&!3&4&!5] 5
[!0&!1&2&!3&4&5] 7
[!0&!1&2&3&4&!5 | !0&1&!2&3&4&!5 | 0&!1&!2&3&4&!5] 9
[!0&!1&2&3&!4&!5] 11
[!0&!1&2&3&4&5 | !0&1&!2&3&4&5 | 0&!1&!2&3&4&5] 12
State: 8
[!0&!1&2&!3&!4 | !0&!1&2&3&4 | !0&1&!2&!3&!4 | !0&1&!2&3&4 | 0&!1&!2&!3&!4
               | 0&!1&!2&3&4] 4
[!0&!1&2&3&!4&5] 11
[!0&!1&2&!3&4&5] 13
[!0&!1&2&!3&4&!5] 14
[!0&1&!2&3&!4&!5] 15
State: 9
[!0&!1&2&!4 | !0&1&!2&!4 | 0&!1&!2&!4] 4
[!0&!1&2&4 | !0&1&!2&4 | 0&!1&!2&4] 9
State: 10
[!0&!1&2&!3&!4 | !0&!1&2&3&5 | !0&1&!2&!3&!4 | !0&1&!2&3&5 | 0&!1&!2&!3&!4
               | 0&!1&!2&3&5] 4
[!0&!1&2&!3&4 | !0&1&!2&!3&4 | 0&!1&!2&!3&4] 9
[!0&!1&2&3&!5 | !0&1&!2&3&!5 | 0&!1&!2&3&!5] 10
State: 11
[!0&!1&2&!3&!4 | !0&!1&2&3&4 | !0&1&!2&!3&!4 | !0&1&!2&3&4 | 0&!1&!2&!3&!4
               | 0&!1&!2&3&4] 4
[!0&!1&2&!3&4] 5
[!0&!1&2&3&!4&!5] 6
[!0&!1&2&3&!4&5] 11
State: 12
[!0&!1&2&!4 | !0&1&!2&!4 | 0&!1&!2&!4] 4
[!0&!1&2&4&!5 | !0&1&!2&4&!5 | 0&!1&!2&4&!5] 9
[!0&!1&2&4&5 | !0&1&!2&4&5 | 0&!1&!2&4&5] 12
State: 13
[!0&!1&2&!3&!4 | !0&1&!2&!3&!4 | 0&!1&!2&!3&!4] 4
[!0&!1&2&!3&4&!5] 5
[!0&!1&2&3&4&!5 | !0&1&!2&3&4&!5 | 0&!1&!2&3&4&!5] 9
[!0&!1&2&3&!4] 11
[!0&!1&2&3&4&5 | !0&1&!2&3&4&5 | 0&!1&!2&3&4&5] 12
[!0&!1&2&!3&4&5] 13
State: 14
[!0&!1&2&3&!4&5] 2
[!0&!1&2&!3&!4 | !0&1&!2&!3&!4 | 0&!1&!2&!3&!4] 4
[!0&!1&2&!3&4&!5] 5
[!0&!1&2&!3&4&5] 7
[!0&!1&2&3&4 | !0&1&!2&3&4 | 0&!1&!2&3&4] 9
[!0&!1&2&3&!4&!5] 11
State: 15
[!0&!1&2&!3&!4 | !0&!1&2&3&4&5 | !0&1&!2&!3&!4 | !0&1&!2&3&4&5 | 0&!1&!2&!3&!4
               | 0&!1&!2&3&4&5] 4
[!0&!1&2&!3&4&5] 5
[!0&!1&2&3&!4&5] 11
[!0&!1&2&!3&4&!5] 14
[!0&1&!2&3&!4&!5] 15
[!0&!1&2&3&4&!5 | !0&1&!2&3&4&!5 | 0&!1&!2&3&4&!5] 16
State: 16
[!0&!1&2&!3&!4 | !0&!1&2&3&5 | !0&1&!2&!3&!4 | !0&1&!2&3&5 | 0&!1&!2&!3&!4
               | 0&!1&!2&3&5] 4
[!0&!1&2&!3&4 | !0&1&!2&!3&4 | 0&!1&!2&!3&4] 9
[!0&!1&2&3&!5 | !0&1&!2&3&!5 | 0&!1&!2&3&!5] 16
--END--""")

# Build an equivalent deterministic monitor
# ;
#     ;
#
spot.set_synthesis_outputs(aut,
  buddy.bdd_ithvar(
    aut.register_ap("u02alarm29control02alarm29control"))\
  & buddy.bdd_ithvar(
   aut.register_ap("u02alarm29control0f1d2alarm29turn2on1b"))\
  & buddy.bdd_ithvar(
   aut.register_ap("u02alarm29control0f1d2alarm29turn2off1b")))
min_equiv = spot.reduce_mealy(aut, False)
tc.assertEqual(min_equiv.num_states(), 6)
tc.assertTrue(spot.are_equivalent(min_equiv, aut))

# Build an automaton that recognizes a subset of the language of the original
# automaton
min_sub = spot.reduce_mealy(aut, True)
tc.assertEqual(min_sub.num_states(), 5)
prod = spot.product(spot.complement(aut), min_sub)
tc.assertTrue(spot.generic_emptiness_check(prod))

aut = spot.automaton("""
HOA: v1
States: 4
Start: 0
AP: 2 "a" "b"
acc-name: all
Acceptance: 0 t
properties: trans-labels explicit-labels state-acc deterministic
--BODY--
State: 0
[!0&!1] 1
[!0&1] 2
[0] 3
State: 1
[0] 1
State: 2
[1] 2
State: 3
[0&1] 3
--END--
""")

spot.set_synthesis_outputs(aut, buddy.bdd_ithvar(aut.register_ap("b")))

exp = """HOA: v1
States: 1
Start: 0
AP: 2 "a" "b"
acc-name: all
Acceptance: 0 t
properties: trans-labels explicit-labels state-acc deterministic weak
controllable-AP: 1
--BODY--
State: 0
[0&1] 0
--END--"""

# An example that shows that we should not build a tree when we use inclusion.
res = spot.reduce_mealy(aut, True)
tc.assertEqual(res.to_str(), exp)

aut = spot.automaton("""
HOA: v1
States: 4
Start: 0
AP: 2 "a" "b"
acc-name: all
Acceptance: 0 t
properties: trans-labels explicit-labels state-acc
--BODY--
State: 0
[!0&!1] 1
[!0&1] 2
[0&!1] 3
State: 1
[0] 1
State: 2
[1] 2
State: 3
[0&1] 3
--END--
""")

spot.set_synthesis_outputs(aut, buddy.bdd_ithvar(aut.register_ap("b")))

exp = """HOA: v1
States: 2
Start: 0
AP: 2 "a" "b"
acc-name: all
Acceptance: 0 t
properties: trans-labels explicit-labels state-acc deterministic weak
controllable-AP: 1
--BODY--
State: 0
[!0&!1] 1
[!0&1] 1
[0&!1] 1
State: 1
[0&1] 1
--END--"""

res = spot.reduce_mealy(aut, True)
tc.assertEqual(res.to_str(), exp)

exp = """digraph "" {
  rankdir=LR
  node [shape="circle"]
  I [label="", style=invis, width=0]
  I -> 0
  0 [label="0"]
  0 -> 1 [label=""]
  0 -> 1 [label=""]
  0 -> 1 [label=""]
  1 [label="1"]
  1 -> 1 [label=""]
}
"""
tc.assertEqual(res.to_str("dot", "g"), exp)

# assertion bug: original machine is not always
# correctly split before testing inside minimize_mealy
aut = spot.automaton("""HOA: v1
States: 2
Start: 0
AP: 11 "u0accel0accel" "u0accel0f1dcon23p81b" "u0accel0f1dcon231b"
  "u0gear0f1dmin0f1dcon61b0f1dadd0gear0f1dcon241b1b1b"
  "u0gear0gear" "u0gear0f1dmax0f1dcon241b0f1dsub0gear0f1dcon241b1b1b"
  "u0steer0f1dsteering0angle0trackpos1b" "u0steer0steer"
  "p0p0gt0rpm0f1dcon5523231b" "p0p0lt0rpm0f1dcon32323231b"
  "p0p0lt0speed0f1dsub0target2speed0f1dmultp0f1dabs0steer1b0f1dcon248881b1b1b"
acc-name: all
Acceptance: 0 t
properties: trans-labels explicit-labels state-acc deterministic
controllable-AP: 0 1 2 3 4 5 6 7
--BODY--
State: 0
[!0&!1&2&!3&4&!5&6&!7&!8&!9&!10] 0
[!0&1&!2&!3&4&!5&6&!7&!8&!9&10] 0
[!0&!1&2&!3&!4&5&6&!7&!8&9&!10] 0
[!0&1&!2&!3&!4&5&6&!7&!8&9&10] 0
[!0&!1&2&3&!4&!5&6&!7&8&!9&!10] 0
[!0&1&!2&3&!4&!5&6&!7&8&!9&10] 0
[!0&!1&2&!3&!4&5&!6&7&8&9 | !0&!1&2&!3&!4&5&6&!7&8&9 | !0&!1&2&!3&4&!5&!6&7&8&9
                          | !0&!1&2&!3&4&!5&6&!7&8&9 | !0&!1&2&3&!4&!5&!6&7&8&9
                          | !0&!1&2&3&!4&!5&6&!7&8&9 | !0&1&!2&!3&!4&5&!6&7&8&9
                          | !0&1&!2&!3&!4&5&6&!7&8&9 | !0&1&!2&!3&4&!5&!6&7&8&9
                          | !0&1&!2&!3&4&!5&6&!7&8&9 | !0&1&!2&3&!4&!5&!6&7&8&9
                          | !0&1&!2&3&!4&!5&6&!7&8&9 | 0&!1&!2&!3&!4&5&!6&7&8&9
                          | 0&!1&!2&!3&!4&5&6&!7&8&9 | 0&!1&!2&!3&4&!5&!6&7&8&9
                          | 0&!1&!2&!3&4&!5&6&!7&8&9 | 0&!1&!2&3&!4&!5&!6&7&8&9
                          | 0&!1&!2&3&!4&!5&6&!7&8&9] 1
State: 1
[!0&!1&2&!3&!4&5&!6&7 | !0&!1&2&!3&!4&5&6&!7 | !0&!1&2&!3&4&!5&!6&7
                      | !0&!1&2&!3&4&!5&6&!7 | !0&!1&2&3&!4&!5&!6&7
                      | !0&!1&2&3&!4&!5&6&!7 | !0&1&!2&!3&!4&5&!6&7
                      | !0&1&!2&!3&!4&5&6&!7 | !0&1&!2&!3&4&!5&!6&7
                      | !0&1&!2&!3&4&!5&6&!7 | !0&1&!2&3&!4&!5&!6&7
                      | !0&1&!2&3&!4&!5&6&!7 | 0&!1&!2&!3&!4&5&!6&7
                      | 0&!1&!2&!3&!4&5&6&!7 | 0&!1&!2&!3&4&!5&!6&7
                      | 0&!1&!2&!3&4&!5&6&!7 | 0&!1&!2&3&!4&!5&!6&7
                      | 0&!1&!2&3&!4&!5&6&!7] 1
--END--""")

spot.minimize_mealy(aut, -1)
spot.minimize_mealy(aut, 0)
spot.minimize_mealy(aut, 1)
auts = spot.split_2step(aut)
spot.minimize_mealy(auts, -1)
spot.minimize_mealy(auts, 0)
spot.minimize_mealy(auts, 1)
