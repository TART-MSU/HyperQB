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

import os
import spot
from unittest import TestCase
tc = TestCase()

contents = '''
HOA: v1 name: "a U b" States: 2 Start: 1 AP: 2 "a" "b" acc-name: Buchi
Acceptance: 1 Inf(0) properties: trans-labels explicit-labels state-acc
deterministic --BODY-- State: 0 {0} [t] 0 State: 1 [1] 0 [0&!1] 1 --END--
'''

filename = 'parsetgba.hoa'

out = open(filename, 'w+')
out.write(contents)
out.close()

a = spot.parse_aut(filename, spot.make_bdd_dict())
tc.assertFalse(a.errors)
spot.print_dot(spot.get_cout(), a.aut)
del a
os.unlink(filename)


autstr = """
HOA: v1
States: 2
Start: 0
AP: 0
Acceptance: 0 t
spot.highlight.edges: 1 1 2 2 3 3 4 4
--BODY--
State: 0
[t] 1
[f] 0
State: 1
[f] 0
[t] 0
--END--
"""

a1 = spot.automaton(autstr)
tc.assertEqual(a1.to_str("hoa", "1.1"), """HOA: v1.1
States: 2
Start: 0
AP: 0
acc-name: all
Acceptance: 0 t
properties: trans-labels explicit-labels state-acc complete
properties: deterministic weak
spot.highlight.edges: 1 1 2 4
--BODY--
State: 0
[t] 1
State: 1
[t] 0
--END--""")
a2 = spot.automaton(autstr, drop_false_edges=False)
tc.assertEqual(a2.to_str("hoa", "1.1"), """HOA: v1.1
States: 2
Start: 0
AP: 0
acc-name: all
Acceptance: 0 t
properties: trans-labels explicit-labels state-acc complete
properties: deterministic weak
spot.highlight.edges: 1 1 2 2 3 3 4 4
--BODY--
State: 0
[t] 1
[f] 0
State: 1
[f] 0
[t] 0
--END--""")

# Issue #555.
a3 = spot.automaton("""HOA: v1.1 States: 6 Start: 0 AP: 3 "a" "b" "c"
acc-name: Buchi Acceptance: 1 Inf(0) properties: trans-labels
explicit-labels state-acc !complete properties: !deterministic
exist-branch spot.highlight.edges: 5 3 6 1 7 3 8 2 --BODY-- State: 0 [0] 1 [0] 2
State: 1 [0] 3 State: 2 [0] 5 State: 3 {0} [0] 3 State: 4 {0} [0] 4
[1] 4 State: 5 {0} [1] 5 [2] 5 --END--""")
a3.purge_dead_states()
tc.assertEqual(a3.to_str("hoa", "1.1"), """HOA: v1.1
States: 5
Start: 0
AP: 3 "a" "b" "c"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc !complete
properties: !deterministic exist-branch
spot.highlight.edges: 5 3 6 2
--BODY--
State: 0
[0] 1
[0] 2
State: 1
[0] 3
State: 2
[0] 4
State: 3 {0}
[0] 3
State: 4 {0}
[1] 4
[2] 4
--END--""")
a3.highlight_edges([5, 6], None)
tc.assertEqual(a3.to_str("hoa", "1.1"), """HOA: v1.1
States: 5
Start: 0
AP: 3 "a" "b" "c"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc !complete
properties: !deterministic exist-branch
--BODY--
State: 0
[0] 1
[0] 2
State: 1
[0] 3
State: 2
[0] 4
State: 3 {0}
[0] 3
State: 4 {0}
[1] 4
[2] 4
--END--""")

# Issue #555 again.   The pairs 30 1 for states and edges are for issue #556
a4 = spot.automaton("""HOA: v1.1 States: 5 Start: 2 AP: 3 "p36" "p38"
"p37" acc-name: Buchi Acceptance: 1 Inf(0) properties: trans-labels
explicit-labels state-acc !complete properties: !deterministic
exist-branch spot.highlight.edges: 1 1 3 1 8 1 12 1 30 1
spot.highlight.states: 30 1 --BODY-- State: 0 [t] 1 State: 1 {0} [!0]
1 State: 2 [!0 | !1 | 2] 0 [t] 3 State: 3 [2] 1 [!2] 4 [2] 1 [!2] 4
State: 4 [!0&2] 1 [!0&!2] 4 [!0&2] 1 [!0&!2] 4 --END--""")
oi = a4.out_iteraser(2)
a4.highlight_edge(40, 10);
a4.highlight_state(40, 10);
while oi:
    n = a4.edge_number(oi.current())
    if n == 3:
        oi.erase()
    else:
        oi.advance()
a4.purge_dead_states()
tc.assertEqual(a4.to_str("hoa", "1.1"),
"""HOA: v1.1
States: 4
Start: 1
AP: 3 "p36" "p38" "p37"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc !complete
properties: !deterministic exist-branch
spot.highlight.edges: 6 1 10 1
--BODY--
State: 0 {0}
[!0] 0
State: 1
[t] 2
State: 2
[2] 0
[!2] 3
[2] 0
[!2] 3
State: 3
[!0&2] 0
[!0&!2] 3
[!0&2] 0
[!0&!2] 3
--END--""")


# Spot supports only one initial state, but the HOA format allows for
# more.  Spot's automaton parser will fuse multiple initial states to
# fit into the Spot definition of an automaton.  Depending on how the
# fusing is done, this can turn a complete automaton into a incomplete
# one, and vice-versa.  See Issue #560.
a = spot.automaton("""HOA: v1 States: 8 Start: 0 Start: 1 AP: 0
acc-name: generalized-Buchi 2 properties: complete Acceptance: 2
Inf(0)&Inf(1) --BODY-- State: 0 {0} [t] 2 [t] 3 [t] 0 State: 1 {0} [t]
4 [t] 5 State: 2 {0} [t] 2 [t] 3 State: 3 {0} [t] 6 [t] 7 State: 4 {1}
[t] 4 [t] 5 State: 5 {1} [t] 6 [t] 7 State: 6 [t] 6 [t] 7 State: 7 [t]
6 [t] 7 --END--""")
tc.assertTrue(a.prop_complete().is_true())

a = spot.automaton("""HOA: v1 States: 8 Start: 0 Start: 1 AP: 0
acc-name: generalized-Buchi 2 properties: complete Acceptance: 2
Inf(0)&Inf(1) --BODY-- State: 0 {0} [t] 2 [t] 3 State: 1 {0} [t] 4 [t]
5 State: 2 {0} [t] 2 [t] 3 State: 3 {0} [t] 6 [t] 7 State: 4 {1} [t] 4
[t] 5 State: 5 {1} [t] 6 [t] 7 State: 6 [t] 6 [t] 7 State: 7 [t] 6 [t]
7 --END--""")
tc.assertTrue(a.prop_complete().is_false())

a = spot.automaton("""HOA: v1.1 States: 8 Start: 0 Start: 1 AP: 1 "a"
acc-name: generalized-Buchi 2 properties: !complete Acceptance: 2
Inf(0)&Inf(1) --BODY-- State: 0 {0} [0] 2 [0] 3 State: 1 {0} [t] 1 [t]
5 State: 2 {0} [t] 2 [t] 3 State: 3 {0} [t] 6 [t] 7 State: 4 {1} [t] 4
[t] 5 State: 5 {1} [t] 6 [t] 7 State: 6 [t] 6 [t] 7 State: 7 [t] 6 [t]
7 --END--""")
tc.assertTrue(a.prop_complete().is_maybe())
tc.assertTrue(spot.is_complete(a))
