#!/usr/bin/python3
# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2016-2017, 2021-2022 Laboratoire de Recherche
# et Développement de l'EPITA.
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
import buddy
from unittest import TestCase
tc = TestCase()

aut = spot.make_twa_graph(spot._bdd_dict)

p1 = buddy.bdd_ithvar(aut.register_ap("p1"))
p2 = buddy.bdd_ithvar(aut.register_ap("p2"))

m = aut.set_buchi()

aut.new_states(3)

aut.set_init_state(0)
aut.new_univ_edge(0, [1, 2], p1, m)
aut.new_univ_edge(0, [0, 1], p2)
aut.new_univ_edge(1, [0, 2, 1], p1 & p2)
aut.new_edge(2, 2, p1 | p2)

tr = [(s, [[x for x in aut.univ_dests(i)] for i in aut.out(s)])
      for s in range(3)]
tc.assertEqual([(0, [[1, 2], [0, 1]]), (1, [[0, 2, 1]]), (2, [[2]])], tr)
tc.assertFalse(aut.is_existential())

received = False
try:
    for i in aut.out(0):
        for j in aut.out(i.dst):
            pass
except RuntimeError:
    received = True
tc.assertTrue(received)

h = aut.to_str('hoa')
tc.assertEqual(h, """HOA: v1
States: 3
Start: 0
AP: 2 "p1" "p2"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels trans-acc univ-branch
--BODY--
State: 0
[0] 1&2 {0}
[1] 0&1
State: 1
[0&1] 0&2&1
State: 2
[0 | 1] 2
--END--""")

aut2 = spot.automaton(h)
h2 = aut2.to_str('hoa')
tc.assertNotEqual(h, h2)

# This will sort destination groups
aut.merge_univ_dests()
h = aut.to_str('hoa')
tc.assertEqual(h, h2)

aut2.set_univ_init_state([0, 1])
h3 = aut2.to_str('hoa')
tc.assertEqual(h3, """HOA: v1
States: 3
Start: 0&1
AP: 2 "p1" "p2"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels trans-acc univ-branch
--BODY--
State: 0
[0] 1&2 {0}
[1] 0&1
State: 1
[0&1] 0&1&2
State: 2
[0 | 1] 2
--END--""")

st = spot.states_and(aut, [0, 2])
st2 = spot.states_and(aut, [1, st])
st3 = spot.states_and(aut, [0, 1, 2])
tc.assertEqual((st, st2, st3), (3, 4, 5))

received = False
try:
    st4 = spot.states_and(aut, [])
except RuntimeError:
    received = True
tc.assertTrue(received)

h = aut.to_str('hoa')
tc.assertEqual(h, """HOA: v1
States: 6
Start: 0
AP: 2 "p1" "p2"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels trans-acc univ-branch
--BODY--
State: 0
[0] 1&2 {0}
[1] 0&1
State: 1
[0&1] 0&1&2
State: 2
[0 | 1] 2
State: 3
[0] 1&2
[1] 0&1&2
State: 4
[0&1] 0&1&2
State: 5
[0&1] 0&1&2
--END--""")

h = spot.split_edges(aut).to_str('hoa')
tc.assertEqual(h, """HOA: v1
States: 6
Start: 0
AP: 2 "p1" "p2"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels trans-acc univ-branch
--BODY--
State: 0
[0&!1] 1&2 {0}
[0&1] 1&2 {0}
[!0&1] 0&1
[0&1] 0&1
State: 1
[0&1] 0&1&2
State: 2
[!0&1] 2
[0&!1] 2
[0&1] 2
State: 3
[0&!1] 1&2
[0&1] 1&2
[!0&1] 0&1&2
[0&1] 0&1&2
State: 4
[0&1] 0&1&2
State: 5
[0&1] 0&1&2
--END--""")


# remove_univ_otf

aut = spot.automaton("""
HOA: v1
States: 3
Start: 0 & 1
Acceptance: 1 Inf(0)
AP: 2 "a" "b"
--BODY--
State: 0
[0] 0 {0}
State: 1
[1] 2 {0}
State: 2
[1] 2
--END--
""")

out = """HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc deterministic
--BODY--
State: 0 {0}
[0&1] 1
State: 1
[0&1] 2
State: 2
[0&1] 2
--END--"""

desalt = spot.remove_univ_otf(aut)
tc.assertEqual(desalt.to_str('hoa'), out)

tc.assertEqual(aut.num_states(), 3)
tc.assertEqual(aut.num_edges(), 3)
aut.edge_storage(3).cond = buddy.bddfalse
aut.purge_dead_states()
tc.assertEqual(aut.num_states(), 1)
tc.assertEqual(aut.num_edges(), 0)
