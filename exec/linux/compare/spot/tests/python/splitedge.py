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

def create_aps(aut):
    return [buddy.bdd_ithvar(aut.register_ap(ap.ap_name())) for ap in aut.ap()]

def do_edge_test(aut, aps, edges_before, edges_after):
    tc.assertEqual(aut.num_edges(), edges_before)
    es = spot.edge_separator()
    for ap in aps:
        es.add_to_basis(ap)
    res = es.separate_compat(aut)
    tc.assertEqual(res.num_edges(), edges_after)

aut = spot.automaton("""
HOA: v1
States: 1
Start: 0
AP: 2 "a" "b"
Acceptance: 1 Inf(0)
--BODY--
State: 0
[t] 0
--END--""")

aps = create_aps(aut)
do_edge_test(aut, aps, 1, 4)

aut = spot.automaton("""
HOA: v1
States: 2
Start: 0
AP: 2 "a" "b"
Acceptance: 1 Inf(0)
--BODY--
State: 0
[t] 0
State: 1
[0&1] 1
--END--""")

aps = create_aps(aut)
do_edge_test(aut, aps, 2, 5)

aut = spot.automaton("""
HOA: v1
States: 1
Start: 0
AP: 1 "a"
Acceptance: 1 Inf(0)
--BODY--
State: 0
[f] 0
--END--""")

aps = create_aps(aut)
do_edge_test(aut, aps, 0, 0)

aut = spot.automaton("""
HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
Acceptance: 1 Inf(0)
--BODY--
State: 0
[0|1] 1
[!1] 2
State: 1
State: 2
--END--""")
# Before:
# State: 0
# {a, b, c, d}
# {a, b}
# After:
# State : 0
# {a, b, c}, {d}
# {a, b}

# a = 00
# b = 10
# c = 01
# d = 11

aps = create_aps(aut)
# [{a, b, c}]
aps = [buddy.bdd_not(aps[0]) | buddy.bdd_not(aps[1])]
do_edge_test(aut, aps, 2, 3)

aut = spot.automaton("""
HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
Acceptance: 1 Inf(0)
--BODY--
State: 0
[t] 1
[!0] 2
State: 1
State: 2
--END--""")
# Before:
# State: 0
# {a, b, c, d}
# {a, c}
# After:
# State : 0
# {a, b}, {c, d}
# {a}, {c}

# a = 00
# b = 10
# c = 01
# d = 11

aps = create_aps(aut)
# [{a, b}, {c, d}]
aps = [buddy.bdd_not(aps[1]), aps[1]]
do_edge_test(aut, aps, 2, 4)

aut = spot.automaton("""
HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
Acceptance: 1 Inf(0)
--BODY--
State: 0
[t] 1
[!0&!1 | !0&1] 2
State: 1
State: 2
--END--""")
# Before:
# State: 0
# {a, b, c, d}
# {a, c}
# After:
# State : 0
# {a},{b},{c},{d}
# {a},{c}

# a = 00
# b = 10
# c = 01
# d = 11

aps = create_aps(aut)
neg_aps = [buddy.bdd_not(a) for a in aps]
# [{a},{b},{c},{d}]
aps = [
    neg_aps[0] & neg_aps[1],
    neg_aps[0] & aps[1],
    aps[0] & neg_aps[1],
    aps[0] & aps[1]
]

do_edge_test(aut, aps, 2, 6)

aut = spot.automaton("""
HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
Acceptance: 1 Inf(0)
--BODY--
State: 0
[t] 1
[!0&!1 | 0&!1] 2
State: 1
State: 2
--END--""")
# Before
# State: 0
# {a, b, c, d}
# {a, b}
# After:
# State : 0
# {a, b, c}, {d}
# {a, b}

# a = 00
# b = 10
# c = 01
# d = 11

aps = create_aps(aut)
neg_aps = [buddy.bdd_not(a) for a in aps]
# [{a, b, c}, {d}]
aps = [
    neg_aps[0] | neg_aps[1],
    aps[0] & aps[1]
]
do_edge_test(aut, aps, 2, 3)
