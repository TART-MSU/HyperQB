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

aut = spot.automaton("""HOA: v1 States: 1 Start: 0 AP: 1 "a"
Acceptance: 1 Inf(0) --BODY-- State: 0 [0] 0 [0] 0 {0} --END--""")
tc.assertEqual(aut.num_edges(), 2)
aut.merge_edges()
tc.assertEqual(aut.num_edges(), 1)

aut = spot.automaton("""
HOA: v1
States: 2
Start: 0
AP: 2 "p0" "p1"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels trans-acc complete
--BODY--
State: 0
[!0] 0 {0}
[0] 1 {0}
State: 1
[!0&!1] 0 {0}
[0 | 1] 1
[0&!1] 1 {0}
--END--""")
tc.assertEqual(aut.num_edges(), 5)
aut.merge_edges()
tc.assertEqual(aut.num_edges(), 5)
tc.assertFalse(spot.is_deterministic(aut))
aut = spot.split_edges(aut)
tc.assertEqual(aut.num_edges(), 9)
aut.merge_edges()
tc.assertEqual(aut.num_edges(), 5)
tc.assertTrue(spot.is_deterministic(aut))

aut = spot.automaton("""
HOA: v1
States: 3
Start: 2
AP: 0
Acceptance: 0 t
--BODY--
State: 0
State: 1
State: 2
--END--""")
tc.assertEqual(aut.num_states(), 3)
tc.assertEqual(aut.num_edges(), 0)
tc.assertEqual(aut.get_init_state_number(), 2)
tc.assertEqual(aut.merge_states(), 2);
tc.assertEqual(aut.num_states(), 1)
tc.assertEqual(aut.num_edges(), 0)
tc.assertEqual(aut.get_init_state_number(), 0)
tc.assertEqual(aut.merge_states(), 0);

for nthread in range(1, 16, 2):
    aut = spot.automaton("""
    HOA: v1
    States: 3
    Start: 0
    AP: 1 "a"
    acc-name: Buchi
    Acceptance: 1 Inf(0)
    properties: trans-labels explicit-labels trans-acc complete
    --BODY--
    State: 0
    [!0] 1 {0}
    [0] 2 {0}
    State: 1
    [!0] 1 {0}
    [0] 1
    State: 2
    [!0] 2 {0}
    [0] 1
    --END--""")
    aut.merge_states(nthread)
    tc.assertEqual(aut.num_edges(), 4)
    tc.assertEqual(aut.num_states(), 2)
    tc.assertTrue(spot.is_deterministic(aut))
    tc.assertTrue(aut.prop_complete())
    aut.merge_states(nthread)
    tc.assertEqual(aut.num_edges(), 4)
    tc.assertEqual(aut.num_states(), 2)
    tc.assertTrue(spot.is_deterministic(aut))
    tc.assertTrue(aut.prop_complete())


    aa = spot.automaton("""
    HOA: v1 States: 41 Start: 0 AP: 3 "allfinished" "finished_0"
    "finished_1" acc-name: parity max odd 4 Acceptance: 4 Inf(3) | (Fin(2)
    & (Inf(1) | Fin(0))) properties: trans-labels explicit-labels
    trans-acc colored properties: deterministic --BODY-- State: 0
    [!0&!1&!2] 1 {1} [!0&!1&2] 2 {1} [!0&1&!2] 3 {1} [!0&1&2] 4 {1}
    [0&!1&!2] 5 {1} [0&!1&2] 6 {1} [0&1&!2] 7 {1} [0&1&2] 8 {1} State: 1
    [!0&!1&!2] 1 {1} [!0&!1&2] 2 {1} [0&1&!2] 7 {1} [0&1&2] 8 {1}
    [!0&1&!2] 9 {1} [!0&1&2] 10 {1} [0&!1&!2] 11 {1} [0&!1&2] 12 {1}
    State: 2 [!0&!1&2] 2 {1} [0&1&!2] 7 {1} [0&1&2] 8 {1} [!0&1&2] 10 {1}
    [0&!1&!2] 11 {1} [0&!1&2] 12 {1} [!0&!1&!2] 13 {1} [!0&1&!2] 14 {1}
    State: 3 [0&1&!2] 7 {1} [0&1&2] 8 {1} [!0&1&!2] 9 {1} [!0&1&2] 10 {1}
    [0&!1&!2] 11 {1} [0&!1&2] 12 {1} [!0&!1&!2] 15 {1} [!0&!1&2] 16 {1}
    State: 4 [0&1&!2] 7 {1} [0&1&2] 8 {1} [!0&1&2] 10 {1} [0&!1&!2] 11 {1}
    [0&!1&2] 12 {1} [!0&1&!2] 14 {1} [!0&!1&2] 16 {1} [!0&!1&!2] 17 {1}
    State: 5 [0&1&2] 8 {3} [!0&1&2] 10 {1} [!0&!1&!2] 18 {1} [!0&!1&2] 19
    {1} [!0&1&!2] 20 {1} State: 6 [0&1&2] 8 {1} [!0&1&2] 10 {1} [!0&!1&2]
    19 {1} [!0&1&!2] 20 {1} [!0&!1&!2] 21 {1} State: 7 [0&1&2] 8 {3}
    [!0&1&2] 10 {1} [!0&!1&2] 19 {1} [!0&1&!2] 20 {1} [!0&!1&!2] 22 {1}
    State: 8 [!0&!1&!2] 5 {1} [0&1&2] 8 {3} [!0&1&2] 10 {1} [!0&!1&2] 19
    {1} [!0&1&!2] 20 {1} State: 9 [0&!1&!2] 5 {1} [0&1&!2] 7 {1} [0&1&2] 8
    {1} [!0&1&!2] 9 {1} [!0&1&2] 10 {1} [0&!1&2] 12 {1} [!0&!1&!2] 23 {1}
    [!0&!1&2] 24 {1} State: 10 [0&!1&!2] 5 {3} [0&1&!2] 7 {3} [0&1&2] 8
    {3} [!0&1&2] 10 {2} [0&!1&2] 12 {3} [!0&1&!2] 14 {1} [!0&!1&2] 24 {1}
    [!0&!1&!2] 25 {1} State: 11 [0&1&2] 8 {3} [!0&1&2] 10 {1} [!0&!1&2] 19
    {1} [!0&!1&!2] 26 {1} [!0&1&!2] 27 {1} State: 12 [0&1&2] 8 {3}
    [!0&1&2] 10 {1} [!0&!1&2] 19 {1} [!0&1&!2] 27 {1} [!0&!1&!2] 28 {1}
    State: 13 [!0&!1&2] 2 {1} [0&1&!2] 7 {1} [0&1&2] 8 {1} [!0&1&2] 10 {1}
    [0&!1&!2] 11 {1} [0&!1&2] 12 {1} [!0&!1&!2] 13 {1} [!0&1&!2] 14 {1}
    State: 14 [0&!1&!2] 5 {3} [0&1&!2] 7 {3} [0&1&2] 8 {3} [!0&1&2] 10 {2}
    [0&!1&2] 12 {3} [!0&1&!2] 14 {1} [!0&!1&2] 24 {2} [!0&!1&!2] 29 {1}
    State: 15 [0&1&!2] 7 {1} [0&1&2] 8 {1} [!0&1&!2] 9 {1} [!0&1&2] 10 {1}
    [0&!1&!2] 11 {1} [0&!1&2] 12 {1} [!0&!1&!2] 15 {1} [!0&!1&2] 16 {1}
    State: 16 [0&1&!2] 7 {1} [0&1&2] 8 {1} [!0&1&2] 10 {1} [0&!1&!2] 11
    {1} [0&!1&2] 12 {1} [!0&1&!2] 14 {1} [!0&!1&2] 16 {1} [!0&!1&!2] 17
    {1} State: 17 [0&1&!2] 7 {1} [0&1&2] 8 {1} [!0&1&2] 10 {1} [0&!1&!2]
    11 {1} [0&!1&2] 12 {1} [!0&1&!2] 14 {1} [!0&!1&2] 16 {1} [!0&!1&!2] 17
    {1} State: 18 [0&1&2] 8 {3} [!0&1&2] 10 {1} [!0&!1&!2] 18 {1}
    [!0&!1&2] 19 {1} [!0&1&!2] 20 {1} State: 19 [0&1&!2] 7 {3} [0&1&2] 8
    {3} [!0&!1&2] 19 {1} [!0&!1&!2] 30 {1} [!0&1&!2] 31 {1} [!0&1&2] 32
    {1} State: 20 [0&1&2] 8 {3} [0&!1&2] 12 {1} [!0&1&!2] 20 {1} [!0&1&2]
    32 {1} [!0&!1&!2] 33 {1} [!0&!1&2] 34 {1} State: 21 [0&1&2] 8 {1}
    [!0&1&2] 10 {1} [!0&!1&!2] 18 {1} [!0&!1&2] 19 {1} [!0&1&!2] 20 {1}
    State: 22 [0&1&2] 8 {3} [!0&1&2] 10 {1} [!0&!1&2] 19 {1} [!0&1&!2] 20
    {1} [!0&!1&!2] 35 {1} State: 23 [0&!1&!2] 5 {1} [0&1&!2] 7 {1} [0&1&2]
    8 {1} [!0&1&!2] 9 {1} [!0&1&2] 10 {1} [0&!1&2] 12 {1} [!0&!1&!2] 23
    {1} [!0&!1&2] 24 {1} State: 24 [0&!1&!2] 5 {3} [0&1&!2] 7 {3} [0&1&2]
    8 {3} [!0&1&2] 10 {2} [0&!1&2] 12 {3} [!0&1&!2] 14 {2} [!0&!1&2] 24
    {1} [!0&!1&!2] 25 {1} State: 25 [0&!1&!2] 5 {3} [0&1&!2] 7 {3} [0&1&2]
    8 {3} [!0&1&2] 10 {2} [0&!1&2] 12 {3} [!0&1&!2] 14 {2} [!0&!1&2] 24
    {1} [!0&!1&!2] 25 {1} State: 26 [0&1&2] 8 {3} [!0&1&2] 10 {1}
    [!0&!1&2] 19 {1} [!0&!1&!2] 26 {1} [!0&1&!2] 27 {1} State: 27 [0&1&2]
    8 {3} [0&!1&2] 12 {3} [!0&1&!2] 27 {1} [!0&1&2] 32 {1} [!0&!1&!2] 36
    {1} [!0&!1&2] 37 {1} State: 28 [0&1&2] 8 {3} [!0&1&2] 10 {1} [!0&!1&2]
    19 {1} [!0&!1&!2] 26 {1} [!0&1&!2] 27 {1} State: 29 [0&!1&!2] 5 {3}
    [0&1&!2] 7 {3} [0&1&2] 8 {3} [!0&1&2] 10 {2} [0&!1&2] 12 {3} [!0&1&!2]
    14 {1} [!0&!1&2] 24 {2} [!0&!1&!2] 29 {1} State: 30 [0&1&!2] 7 {3}
    [0&1&2] 8 {3} [!0&!1&2] 19 {1} [!0&!1&!2] 30 {1} [!0&1&!2] 31 {1}
    [!0&1&2] 32 {1} State: 31 [0&!1&!2] 5 {3} [0&1&!2] 7 {3} [0&1&2] 8 {3}
    [0&!1&2] 12 {3} [!0&1&!2] 31 {1} [!0&1&2] 32 {2} [!0&!1&2] 37 {2}
    [!0&!1&!2] 38 {1} State: 32 [0&!1&!2] 5 {3} [0&1&!2] 7 {3} [0&1&2] 8
    {3} [0&!1&2] 12 {3} [!0&1&!2] 31 {1} [!0&1&2] 32 {2} [!0&!1&2] 37 {1}
    [!0&!1&!2] 39 {1} State: 33 [0&1&2] 8 {3} [0&!1&2] 12 {1} [!0&1&!2] 20
    {1} [!0&1&2] 32 {1} [!0&!1&!2] 33 {1} [!0&!1&2] 34 {1} State: 34
    [0&1&!2] 7 {3} [0&1&2] 8 {3} [0&!1&!2] 11 {1} [0&!1&2] 12 {1}
    [!0&1&!2] 31 {1} [!0&1&2] 32 {1} [!0&!1&2] 34 {1} [!0&!1&!2] 40 {1}
    State: 35 [0&1&2] 8 {3} [!0&1&2] 10 {1} [!0&!1&2] 19 {1} [!0&1&!2] 20
    {1} [!0&!1&!2] 35 {1} State: 36 [0&1&2] 8 {3} [0&!1&2] 12 {3}
    [!0&1&!2] 27 {1} [!0&1&2] 32 {1} [!0&!1&!2] 36 {1} [!0&!1&2] 37 {1}
    State: 37 [0&!1&!2] 5 {3} [0&1&!2] 7 {3} [0&1&2] 8 {3} [0&!1&2] 12 {3}
    [!0&1&!2] 31 {2} [!0&1&2] 32 {2} [!0&!1&2] 37 {1} [!0&!1&!2] 39 {1}
    State: 38 [0&!1&!2] 5 {3} [0&1&!2] 7 {3} [0&1&2] 8 {3} [0&!1&2] 12 {3}
    [!0&1&!2] 31 {1} [!0&1&2] 32 {2} [!0&!1&2] 37 {2} [!0&!1&!2] 38 {1}
    State: 39 [0&!1&!2] 5 {3} [0&1&!2] 7 {3} [0&1&2] 8 {3} [0&!1&2] 12 {3}
    [!0&1&!2] 31 {2} [!0&1&2] 32 {2} [!0&!1&2] 37 {1} [!0&!1&!2] 39 {1}
    State: 40 [0&1&!2] 7 {3} [0&1&2] 8 {3} [0&!1&!2] 11 {1} [0&!1&2] 12
    {1} [!0&1&!2] 31 {1} [!0&1&2] 32 {1} [!0&!1&2] 34 {1} [!0&!1&!2] 40
    {1} --END--""")
    aa.merge_states(nthread)
    # This used to cause a segfault reported by Philipp.
    print(aa.to_str())
