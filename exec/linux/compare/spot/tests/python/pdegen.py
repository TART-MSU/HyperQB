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

a, b, d, f = spot.automata("""
HOA: v1
States: 2
Start: 0
AP: 1 "p0"
Acceptance: 3 Inf(0)&Inf(1)|Fin(2)
--BODY--
State: 0
[0] 1 {1 2}
State: 1
[0] 0 {0}
--END--
HOA: v1
States: 3
Start: 2
AP: 1 "p0"
Acceptance: 3 Inf(0)&Inf(1)|Fin(2)
--BODY--
State: 0
[0] 1 {1 2}
State: 1
[0] 0 {0}
State: 2
[0] 0 {0}
[0] 0 {1}
--END--
HOA: v1
States: 1
Start: 0
AP: 1 "p0"
Acceptance: 1 Fin(0)
--BODY--
State: 0
[0] 0 {0}
--END--
HOA: v1
States: 2
Start: 0
AP: 1 "p0"
Acceptance: 2 Fin(0)|Fin(1)
--BODY--
State: 0
[0] 0 {0}
[0] 1 {1}
State: 1
[!0] 0
--END--
""")

tc.assertEqual(spot.is_partially_degeneralizable(a), [0, 1])
da = spot.partial_degeneralize(a, [0, 1])
tc.assertTrue(da.equivalent_to(a))
tc.assertEqual(da.num_states(), 2)

tc.assertEqual(spot.is_partially_degeneralizable(b), [0, 1])
db = spot.partial_degeneralize(b, [0, 1])
tc.assertTrue(db.equivalent_to(b))
tc.assertEqual(db.num_states(), 3)

db.copy_state_names_from(b)
dbhoa = db.to_str('hoa')
tc.assertEqual(dbhoa, """HOA: v1
States: 3
Start: 0
AP: 1 "p0"
acc-name: Streett 1
Acceptance: 2 Fin(0) | Inf(1)
properties: trans-labels explicit-labels state-acc deterministic
--BODY--
State: 0 "2#0"
[0] 1
State: 1 "0#0" {0 1}
[0] 2
State: 2 "1#0" {1}
[0] 1
--END--""")

c = spot.automaton("randaut -A'(Fin(0)&Inf(1)&Inf(2))|Fin(2)' 1 |")
tc.assertEqual(spot.is_partially_degeneralizable(c), [1, 2])
dc = spot.partial_degeneralize(c, [1, 2])
tc.assertTrue(dc.equivalent_to(c))
tc.assertEqual(str(dc.get_acceptance()), '(Fin(0) & Inf(2)) | Fin(1)')

tc.assertEqual(spot.is_partially_degeneralizable(d), [])
dd = spot.partial_degeneralize(d, [])
tc.assertTrue(dd.equivalent_to(d))
tc.assertEqual(dd.num_states(), 1)
tc.assertEqual(str(dd.get_acceptance()), 'Inf(1) & Fin(0)')

e = spot.dualize(b)
spot.cleanup_acceptance_here(e)
tc.assertEqual(str(e.get_acceptance()), 'Fin(0)|Fin(1)')
de = spot.partial_degeneralize(e, [0, 1])
tc.assertTrue(de.equivalent_to(e))
tc.assertEqual(de.num_states(), 4)

de.copy_state_names_from(e)
dehoa = de.to_str('hoa')
tc.assertEqual(dehoa, """HOA: v1
States: 4
Start: 0
AP: 1 "p0"
acc-name: co-Buchi
Acceptance: 1 Fin(0)
properties: trans-labels explicit-labels trans-acc complete
properties: deterministic
--BODY--
State: 0 "0#0"
[0] 1
[!0] 2
State: 1 "1#0"
[!0] 2
[0] 3 {0}
State: 2 "3#0"
[t] 2
State: 3 "2#0"
[0] 1 {0}
[!0] 2
--END--""")

tc.assertEqual(spot.is_partially_degeneralizable(de), [])

df = spot.partial_degeneralize(f, [0, 1])
df.equivalent_to(f)
tc.assertEqual(str(df.acc()), '(1, Fin(0))')
df2 = spot.degeneralize(f)
df.equivalent_to(f)
tc.assertEqual(str(df2.acc()), '(1, Fin(0))')

try:
    df = spot.partial_degeneralize(f, [0, 1, 2])
except RuntimeError as e:
    tc.assertIn('partial_degeneralize(): {0,1,2} does not', str(e))
else:
    raise RuntimeError("missing exception")

aut5 = spot.automaton(""" HOA: v1 States: 8 Start: 0 AP: 3 "p0" "p1" "p2"
acc-name: generalized-Buchi 10 Acceptance: 10
Inf(0)&Inf(1)&Inf(2)&Inf(3)&Inf(4)&Inf(5)&Inf(6)&Inf(7)&Inf(8)&Inf(9)
properties: trans-labels explicit-labels trans-acc deterministic --BODY--
State: 0 [0&!1&2] 3 {2 4 9} State: 1 [!0&1&2] 5 {2 6 7 8} [!0&!1&!2] 0 {2}
State: 2 [0&!1&2] 3 {1 4 9} State: 3 [0&!1&2] 4 {0 1 5 9} State: 4 [!0&!1&2] 1
{7} [0&!1&2] 6 {1 2} State: 5 [!0&1&2] 7 {3 5} State: 6 [!0&!1&2] 2 {1 3 5}
State: 7 [0&!1&!2] 0 {4 7} --END--""")

daut5 = spot.degeneralize_tba(aut5)
tc.assertTrue(daut5.equivalent_to(aut5))
sets = list(range(aut5.num_sets()))
tc.assertEqual(spot.is_partially_degeneralizable(aut5), sets)
pdaut5 = spot.partial_degeneralize(aut5, sets)
tc.assertTrue(pdaut5.equivalent_to(aut5))
tc.assertEqual(daut5.num_states(), 9)
tc.assertEqual(pdaut5.num_states(), 8)

aut6 = spot.automaton("""HOA: v1 States: 6 Start: 0 AP: 3 "p0" "p1" "p2"
acc-name: generalized-Buchi 3 Acceptance: 3 Inf(0)&Inf(1)&Inf(2) properties:
trans-labels explicit-labels trans-acc deterministic --BODY-- State: 0
[0&!1&!2] 4 {1} State: 1 [!0&!1&!2] 2 {0 1} State: 2 [!0&1&2] 0 {1} State: 3
[0&1&!2] 5 {1} State: 4 [!0&1&!2] 0 {1 2} [0&!1&!2] 3 {0} State: 5 [!0&1&2] 1
--END-- """)
daut6 = spot.degeneralize_tba(aut6)
tc.assertTrue(daut6.equivalent_to(aut6))
sets = list(range(aut6.num_sets()))
tc.assertEqual(spot.is_partially_degeneralizable(aut6), sets)
pdaut6 = spot.partial_degeneralize(aut6, sets)
tc.assertTrue(pdaut6.equivalent_to(aut6))
tc.assertEqual(daut6.num_states(), 8)
tc.assertEqual(pdaut6.num_states(), 8)


aut7 = spot.automaton("""HOA: v1 States: 8 Start: 0 AP: 3 "p0" "p1" "p2"
acc-name: generalized-Buchi 4 Acceptance: 4 Inf(0)&Inf(1)&Inf(2)&Inf(3)
properties: trans-labels explicit-labels trans-acc deterministic --BODY--
State: 0 [0&!1&2] 1 {2 3} State: 1 [0&!1&2] 0 {0 2} [0&!1&!2] 6 State: 2
[0&1&2] 0 [!0&!1&2] 5 [!0&1&!2] 6 {1} State: 3 [0&!1&2] 5 [0&!1&!2] 6 State: 4
[!0&!1&!2] 3 State: 5 [0&1&!2] 0 [!0&1&2] 7 State: 6 [0&1&2] 2 {1} State: 7
[!0&!1&2] 0 {0} [!0&1&!2] 4 --END--""")
daut7 = spot.degeneralize_tba(aut7)
tc.assertTrue(daut7.equivalent_to(aut7))
sets = list(range(aut7.num_sets()))
tc.assertEqual(spot.is_partially_degeneralizable(aut7), sets)
pdaut7 = spot.partial_degeneralize(aut7, sets)
tc.assertTrue(pdaut7.equivalent_to(aut7))
tc.assertEqual(daut7.num_states(), 10)
tc.assertEqual(pdaut7.num_states(), 10)
ddaut7 = spot.dualize(aut7)
ddaut7a = spot.scc_filter(spot.dualize(spot.degeneralize_tba(ddaut7)))
tc.assertTrue(ddaut7a.equivalent_to(aut7))
tc.assertEqual(ddaut7a.num_states(), daut7.num_states())
ddaut7b = spot.scc_filter(spot.dualize(spot.to_nca(ddaut7)))
tc.assertTrue(ddaut7b.equivalent_to(aut7))
tc.assertEqual(ddaut7b.num_states(), daut7.num_states())
ddaut7c = spot.scc_filter(spot.dualize(spot.to_dca(ddaut7)))
tc.assertTrue(ddaut7c.equivalent_to(aut7))
tc.assertEqual(ddaut7c.num_states(), daut7.num_states())

aut8 = spot.automaton("""HOA: v1 States: 8 Start: 0 AP: 3 "p0" "p1" "p2"
acc-name: generalized-Buchi 5 Acceptance: 5 Inf(0)&Inf(1)&Inf(2)&Inf(3)&Inf(4)
properties: trans-labels explicit-labels trans-acc deterministic --BODY--
State: 0 [!0&1&!2] 7 {0} State: 1 [!0&1&2] 1 {4} [0&!1&2] 6 {1 2} State: 2
[!0&!1&2] 3 {1 2} [0&1&2] 5 State: 3 [0&1&2] 2 {2} State: 4 [!0&!1&!2] 3 State:
5 [!0&1&!2] 0 {1 3} State: 6 [0&1&2] 4 [0&1&!2] 6 State: 7 [!0&!1&!2] 1
--END--""")
daut8 = spot.degeneralize_tba(aut8)
tc.assertTrue(daut8.equivalent_to(aut8))
sets = list(range(aut8.num_sets()))
tc.assertEqual(spot.is_partially_degeneralizable(aut8), sets)
pdaut8 = spot.partial_degeneralize(aut8, sets)
tc.assertTrue(pdaut8.equivalent_to(aut8))
tc.assertEqual(daut8.num_states(), 22)
tc.assertEqual(pdaut8.num_states(), 9)

aut9 = spot.dualize(aut8)
pdaut9 = spot.partial_degeneralize(aut9, sets)
tc.assertTrue(pdaut9.equivalent_to(aut9))
# one more state than aut9, because dualize completed the automaton.
tc.assertEqual(pdaut9.num_states(), 10)

aut10 = spot.automaton("""HOA: v1
States: 3
Start: 0
AP: 1 "p0"
Acceptance: 3 (Fin(0)|Fin(1))&Inf(2) | Inf(0)&Inf(1)
--BODY--
State: 0
[0] 1 {0}
State: 1
[0] 2 {2}
[!0] 2
State: 2
[0] 0 {1}
[!0] 1
--END--""")
tc.assertEqual(spot.is_partially_degeneralizable(aut10), [0, 1])
pdaut10 = spot.partial_degeneralize(aut10, [0, 1])
tc.assertTrue(pdaut10.equivalent_to(aut10))
tc.assertEqual(pdaut10.to_str(),  """HOA: v1
States: 3
Start: 0
AP: 1 "p0"
Acceptance: 2 (Fin(1) & Inf(0)) | Inf(1)
properties: trans-labels explicit-labels trans-acc deterministic
--BODY--
State: 0
[0] 1 {1}
State: 1
[!0] 2
[0] 2 {0}
State: 2
[0] 0 {1}
[!0] 1
--END--""")

aut11 = spot.automaton("""HOA: v1
States: 3
Start: 0
AP: 1 "p0"
Acceptance: 4 (Fin(0)|Fin(1))&Inf(2) | Inf(0)&Inf(1)&Inf(3)
--BODY--
State: 0
[0] 1 {0}
State: 1
[0] 2 {2}
[!0] 2
State: 2
[0] 0 {1}
[!0] 1
--END--""")
tc.assertEqual(spot.is_partially_degeneralizable(aut11), [0, 1])
pdaut11 = spot.partial_degeneralize(aut11, [0, 1])
tc.assertTrue(pdaut11.equivalent_to(aut11))
tc.assertEqual(pdaut11.to_str(),  """HOA: v1
States: 3
Start: 0
AP: 1 "p0"
Acceptance: 3 (Fin(2) & Inf(0)) | (Inf(1)&Inf(2))
properties: trans-labels explicit-labels trans-acc deterministic
--BODY--
State: 0
[0] 1 {2}
State: 1
[!0] 2
[0] 2 {0}
State: 2
[0] 0 {2}
[!0] 1
--END--""")

aut12 = spot.automaton("""HOA: v1
States: 3
Start: 0
AP: 1 "p0"
Acceptance: 4 Inf(0)&Inf(1)&Inf(3) | (Inf(0)&Inf(1))&Fin(2)
--BODY--
State: 0
[0] 1 {0}
State: 1
[0] 2 {2}
[!0] 2
State: 2
[0] 2 {1}
[0] 0
[!0] 1 {3}
--END--""")
tc.assertEqual(spot.is_partially_degeneralizable(aut12), [0,1])
aut12b = spot.partial_degeneralize(aut12, [0,1])
aut12c = spot.partial_degeneralize(aut12b, [1,2])
tc.assertTrue(aut12c.equivalent_to(aut12))
tc.assertEqual(aut12c.num_states(), 9)

aut12d = spot.partial_degeneralize(aut12, [0,1,3])
aut12e = spot.partial_degeneralize(aut12d, [0,1])
tc.assertTrue(aut12e.equivalent_to(aut12))
tc.assertEqual(aut12e.num_states(), 9)

aut12f = spot.partial_degeneralize(aut12)
tc.assertTrue(aut12f.equivalent_to(aut12))
tc.assertEqual(aut12f.num_states(), 9)

# Check handling of original-states
dot = aut12f.to_str('dot', 'd')
tc.assertEqual(dot, """digraph "" {
  rankdir=LR
  label="Inf(2) | (Inf(1) & Fin(0))\\n[Rabin-like 2]"
  labelloc="t"
  node [shape="box",style="rounded",width="0.5"]
  I [label="", style=invis, width=0]
  I -> 0
  0 [label="0 (0)"]
  0 -> 1 [label="p0"]
  1 [label="1 (1)"]
  1 -> 2 [label="!p0"]
  1 -> 2 [label="p0\\n{0}"]
  2 [label="2 (2)"]
  2 -> 0 [label="p0"]
  2 -> 3 [label="!p0"]
  2 -> 4 [label="p0\\n{1}"]
  3 [label="3 (1)"]
  3 -> 8 [label="!p0"]
  3 -> 8 [label="p0\\n{0}"]
  4 [label="4 (2)"]
  4 -> 0 [label="p0"]
  4 -> 4 [label="p0"]
  4 -> 5 [label="!p0"]
  5 [label="5 (1)"]
  5 -> 6 [label="!p0"]
  5 -> 6 [label="p0\\n{0}"]
  6 [label="6 (2)"]
  6 -> 5 [label="!p0"]
  6 -> 6 [label="p0"]
  6 -> 7 [label="p0"]
  7 [label="7 (0)"]
  7 -> 3 [label="p0"]
  8 [label="8 (2)"]
  8 -> 3 [label="!p0"]
  8 -> 4 [label="p0\\n{1,2}"]
  8 -> 7 [label="p0"]
}
""")

aut12g = spot.partial_degeneralize(aut12f)
tc.assertEqual(aut12f, aut12g)

aut13 = spot.automaton("""HOA: v1
States: 2
Start: 0
AP: 4 "p9" "p14" "p10" "p7"
acc-name: generalized-Buchi 3
Acceptance: 3 Inf(0)&Inf(1)&Inf(2)
properties: trans-labels explicit-labels trans-acc deterministic
--BODY--
State: 0
[!0&!1&2] 0 {0 1 2}
[!0&!1&!2] 1 {0 1}
State: 1
[!0&!1&2&!3] 0 {0 1 2}
[!0&!1&!2&!3] 1 {0 1}
[!0&!1&!2&3] 1 {0}
[!0&!1&2&3] 1 {0 2}
--END--""")
aut13g = spot.partial_degeneralize(aut13)
tc.assertTrue(aut13g.equivalent_to(aut13))
tc.assertEqual(aut13g.num_states(), 3)


aut14 = spot.automaton("""HOA: v1
States: 2
Start: 0
AP: 2 "p0" "p1"
Acceptance: 5 (Inf(0)&Inf(1)) | ((Fin(2)|Fin(3)) & Fin(4))
--BODY--
State: 0
[!0 & 1] 0 {2 3}
[!0 & !1] 0 {3}
[0] 1
State: 1
[0&1] 1 {1 2 4}
[0&!1] 1 {4}
[!0&1] 1 {0 1 2 3}
[!0&!1] 1 {0 3}
--END--
""")
aut14g = spot.partial_degeneralize(aut14)
tc.assertTrue(aut14g.equivalent_to(aut14))
tc.assertEqual(aut14g.num_states(), 3)

# Extracting an SCC from this large automaton will produce an automaton A in
# which original-states refers to states larger than those in A.  Some version
# of partial_degeneralize(A) incorrectly assumed that orig_states could not be
# larger than A, (because initially partial_degeneralize did not compose
# original-states).
aut15 = spot.automaton(""" HOA: v1 name: "(FGp0 & ((XFp0 & F!p1) | F(Gp1 &
XG!p0))) | G(F!p0 & (XFp0 | F!p1) & F(Gp1 | G!p0))" States: 14 Start: 0 AP: 2
"p1" "p0" Acceptance: 6 (Fin(0) & Fin(1)) | ((Fin(4)|Fin(5)) & (Inf(2)&Inf(3)))
properties: trans-labels explicit-labels trans-acc complete properties:
deterministic --BODY-- State: 0 [!0] 1 [0] 2 State: 1 [!0&!1] 1 {0 1 2 3 5}
[0&!1] 3 [!0&1] 4 [0&1] 5 State: 2 [0&!1] 2 {1} [!0&1] 4 [!0&!1] 6 [0&1] 7
State: 3 [0&!1] 3 {1 3} [!0&1] 4 [!0&!1] 6 {0 1 2 3 5} [0&1] 8 State: 4 [!0&!1]
4 {1 2 3 5} [!0&1] 4 {2 4 5} [0&!1] 5 {1 3} [0&1] 5 {4} State: 5 [!0&1] 4 {2 4
5} [0&!1] 5 {1 3} [0&1] 8 {2 4} [!0&!1] 9 {1 2 3 5} State: 6 [0&!1] 3 {1 3}
[!0&1] 4 [0&1] 5 [!0&!1] 10 State: 7 [!0&1] 4 [0&!1] 7 {1 3} [!0&!1] 11 [0&1]
12 {0 4} State: 8 [!0&1] 4 {2 4 5} [0&1] 5 {4} [0&!1] 8 {1 3} [!0&!1] 11 {1 3
5} State: 9 [!0&1] 4 {2 4 5} [0&!1] 5 {1 3} [0&1] 5 {4} [!0&!1] 11 {1 3 5}
State: 10 [!0&1] 4 [0&1] 8 [!0&!1] 10 {0 1 2 3 5} [0&!1] 13 {1 2 3} State: 11
[!0&1] 4 {2 4 5} [0&!1] 8 {1 2 3} [0&1] 8 {2 4} [!0&!1] 11 {1 2 3 5} State: 12
[!0&1] 4 [0&1] 7 {0 2 4} [!0&!1] 9 [0&!1] 12 {1 3} State: 13 [!0&1] 4 [0&1] 5
[!0&!1] 10 {0 1 3 5} [0&!1] 13 {1 3} --END--""")
si = spot.scc_info(aut15)
aut15b = si.split_on_sets(2, [])[0]; d
aut15c = spot.partial_degeneralize(aut15b)
tc.assertTrue(aut15c.equivalent_to(aut15b))


# Testing property propagation/update
# for propagate_marks_here

s = """HOA: v1
States: 3
Start: 0
AP: 1 "a"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc
--BODY--
State: 0
[0] 1
[!0] 2
State: 1 {0}
[0] 0
State: 2
[!0] 0
--END--"""
aut = spot.automaton(s)
spot.propagate_marks_here(aut)
s2 = aut.to_str("hoa")

tc.assertEqual(s2, """HOA: v1
States: 3
Start: 0
AP: 1 "a"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels trans-acc deterministic
--BODY--
State: 0
[0] 1 {0}
[!0] 2
State: 1
[0] 0 {0}
State: 2
[!0] 0
--END--""")
