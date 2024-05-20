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

g = spot.automaton("""HOA: v1 States: 9 Start: 0 AP: 2 "a" "b"
acc-name: Streett 1 Acceptance: 2 Fin(0) | Inf(1) properties:
trans-labels explicit-labels state-acc spot-state-player: 0 1 0 1 0 1
0 1 1 --BODY-- State: 0 {1} [1] 1 [1] 3 State: 1 {1} [1] 2 State: 2
{1} [0] 8 State: 3 {1} [1] 4 State: 4 {1} [0] 5 State: 5 {1} [0] 6
State: 6 {1} [1] 7 State: 7 State: 8 {1} [0] 2 --END--""")

tc.assertFalse(spot.solve_parity_game(g))

s = spot.highlight_strategy(g).to_str("HOA", "1.1")
tc.assertEqual(s, """HOA: v1.1
States: 9
Start: 0
AP: 2 "a" "b"
acc-name: Streett 1
Acceptance: 2 Fin(0) | Inf(1)
properties: trans-labels explicit-labels state-acc !complete
properties: !deterministic exist-branch
spot.highlight.states: 0 5 1 4 2 4 3 5 4 5 5 5 6 5 7 5 8 4
spot.highlight.edges: 2 5 3 4 6 5 8 5 9 4
spot.state-player: 0 1 0 1 0 1 0 1 1
--BODY--
State: 0 {1}
[1] 1
[1] 3
State: 1 {1}
[1] 2
State: 2 {1}
[0] 8
State: 3 {1}
[1] 4
State: 4 {1}
[0] 5
State: 5 {1}
[0] 6
State: 6 {1}
[1] 7
State: 7
State: 8 {1}
[0] 2
--END--""")

# Testing case where parity_game optimization
# lead to wrong results
si = spot.synthesis_info()

game = spot.automaton("""HOA: v1
States: 27
Start: 7
AP: 11 "a" "b" "c" "d" "e" "f" "g" "h" "i" "j" "k"
acc-name: parity max odd 3
Acceptance: 3 Fin(2) & (Inf(1) | Fin(0))
properties: trans-labels explicit-labels trans-acc colored
properties: deterministic
spot-state-player: 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
controllable-AP: 0 1 2 3 4 5 6 7
--BODY--
State: 0
[t] 8 {0}
State: 1
[8&9] 8 {0}
[!8&!10 | !9&!10] 9 {0}
[!8&10 | !9&10] 10 {0}
State: 2
[8&9] 8 {0}
[!8&!10 | !9&!10] 11 {0}
[!8&10 | !9&10] 12 {0}
State: 3
[8&9] 8 {0}
[!9&!10] 13 {0}
[!8&10 | !9&10] 14 {0}
[!8&9&!10] 15 {0}
State: 4
[8&9] 8 {0}
[!8&!10 | !9&!10] 16 {0}
[!8&!9&10] 17 {0}
[!8&9&10] 18 {0}
[8&!9&10] 19 {0}
State: 5
[8&9] 8 {0}
[!9&!10] 20 {0}
[!8&10 | !9&10] 21 {0}
[!8&9&!10] 22 {0}
State: 6
[8&9] 8 {0}
[!8&!10 | !9&!10] 23 {0}
[!8&!9&10] 24 {0}
[!8&9&10] 25 {0}
[8&!9&10] 26 {0}
State: 7
[8&9] 8 {0}
[!9&!10] 13 {0}
[!8&9&!10] 15 {0}
[!8&!9&10] 17 {0}
[!8&9&10] 18 {0}
[8&!9&10] 19 {0}
State: 8
[!0&!1&2&!3&!4&5&!6&7 | !0&!1&2&!3&!4&5&6&!7 | !0&!1&2&!3&4&!5&!6&7 |
!0&!1&2&!3&4&!5&6&!7 | !0&!1&2&3&!4&!5&!6&7 | !0&!1&2&3&!4&!5&6&!7 |
!0&1&!2&!3&!4&5&!6&7 | !0&1&!2&!3&!4&5&6&!7 | !0&1&!2&!3&4&!5&!6&7 |
!0&1&!2&!3&4&!5&6&!7 | !0&1&!2&3&!4&!5&!6&7 | !0&1&!2&3&!4&!5&6&!7 |
0&!1&!2&!3&!4&5&!6&7 | 0&!1&!2&!3&!4&5&6&!7 | 0&!1&!2&!3&4&!5&!6&7 |
 0&!1&!2&!3&4&!5&6&!7 | 0&!1&!2&3&!4&!5&!6&7 | 0&!1&!2&3&!4&!5&6&!7] 0 {1}
State: 9
[!0&!1&2&!3&!4&5&!6&7 | !0&!1&2&!3&!4&5&6&!7 | !0&!1&2&!3&4&!5&!6&7 |
!0&!1&2&!3&4&!5&6&!7 | !0&1&!2&!3&!4&5&!6&7 | !0&1&!2&!3&!4&5&6&!7 |
!0&1&!2&!3&4&!5&!6&7 | !0&1&!2&!3&4&!5&6&!7 | 0&!1&!2&!3&!4&5&!6&7 |
0&!1&!2&!3&!4&5&6&!7 | 0&!1&!2&!3&4&!5&!6&7 | 0&!1&!2&!3&4&!5&6&!7] 1 {2}
[!0&!1&2&3&!4&!5&!6&7 | !0&!1&2&3&!4&!5&6&!7 | !0&1&!2&3&!4&!5&!6&7 |
!0&1&!2&3&!4&!5&6&!7 | 0&!1&!2&3&!4&!5&!6&7 | 0&!1&!2&3&!4&!5&6&!7] 2 {2}
State: 10
[!0&!1&2&!3&!4&5&!6&7 | !0&!1&2&!3&!4&5&6&!7 | !0&!1&2&!3&4&!5&!6&7 |
!0&!1&2&!3&4&!5&6&!7 | !0&1&!2&!3&!4&5&!6&7 | !0&1&!2&!3&!4&5&6&!7 |
!0&1&!2&!3&4&!5&!6&7 | !0&1&!2&!3&4&!5&6&!7 | 0&!1&!2&!3&!4&5&!6&7 |
0&!1&!2&!3&!4&5&6&!7 | 0&!1&!2&!3&4&!5&!6&7 | 0&!1&!2&!3&4&!5&6&!7] 0 {1}
[!0&!1&2&3&!4&!5&!6&7 | !0&!1&2&3&!4&!5&6&!7 | !0&1&!2&3&!4&!5&!6&7 |
!0&1&!2&3&!4&!5&6&!7 | 0&!1&!2&3&!4&!5&!6&7 | 0&!1&!2&3&!4&!5&6&!7] 2 {2}
State: 11
[!0&!1&2&!3&4&!5&!6&7 | !0&!1&2&!3&4&!5&6&!7 | !0&!1&2&3&!4&!5&!6&7 |
!0&!1&2&3&!4&!5&6&!7 | !0&1&!2&!3&4&!5&!6&7 | !0&1&!2&!3&4&!5&6&!7 |
!0&1&!2&3&!4&!5&!6&7 | !0&1&!2&3&!4&!5&6&!7 | 0&!1&!2&!3&4&!5&!6&7 |
0&!1&!2&!3&4&!5&6&!7 | 0&!1&!2&3&!4&!5&!6&7 | 0&!1&!2&3&!4&!5&6&!7] 0 {1}
[!0&!1&2&!3&!4&5&!6&7 | !0&!1&2&!3&!4&5&6&!7 | !0&1&!2&!3&!4&5&!6&7 |
!0&1&!2&!3&!4&5&6&!7 | 0&!1&!2&!3&!4&5&!6&7 | 0&!1&!2&!3&!4&5&6&!7] 1 {2}
State: 12
[!0&!1&2&!3&!4&5&!6&7 | !0&!1&2&!3&!4&5&6&!7 | !0&1&!2&!3&!4&5&!6&7 |
!0&1&!2&!3&!4&5&6&!7 | 0&!1&!2&!3&!4&5&!6&7 | 0&!1&!2&!3&!4&5&6&!7] 1 {2}
[!0&!1&2&!3&4&!5&!6&7 | !0&!1&2&!3&4&!5&6&!7 | !0&!1&2&3&!4&!5&!6&7 |
!0&!1&2&3&!4&!5&6&!7 | !0&1&!2&!3&4&!5&!6&7 | !0&1&!2&!3&4&!5&6&!7 |
!0&1&!2&3&!4&!5&!6&7 | !0&1&!2&3&!4&!5&6&!7 | 0&!1&!2&!3&4&!5&!6&7 |
0&!1&!2&!3&4&!5&6&!7 | 0&!1&!2&3&!4&!5&!6&7 | 0&!1&!2&3&!4&!5&6&!7] 2 {2}
State: 13
[!0&!1&2&!3&!4&5&6&!7 | !0&!1&2&!3&4&!5&6&!7 | !0&1&!2&!3&!4&5&!6&7 |
!0&1&!2&!3&!4&5&6&!7 | !0&1&!2&!3&4&!5&!6&7 | !0&1&!2&!3&4&!5&6&!7 |
0&!1&!2&!3&!4&5&!6&7 | 0&!1&!2&!3&!4&5&6&!7 | 0&!1&!2&!3&4&!5&!6&7 |
0&!1&!2&!3&4&!5&6&!7] 1 {1}
[!0&!1&2&3&!4&!5&6&!7 | !0&1&!2&3&!4&!5&!6&7 | !0&1&!2&3&!4&!5&6&!7 |
0&!1&!2&3&!4&!5&!6&7 | 0&!1&!2&3&!4&!5&6&!7] 2 {1}
[!0&!1&2&!3&!4&5&!6&7 | !0&!1&2&!3&4&!5&!6&7] 3 {1}
[!0&!1&2&3&!4&!5&!6&7] 5 {1}
State: 14
[!0&!1&2&!3&!4&5&!6&7 | !0&!1&2&!3&!4&5&6&!7 | !0&!1&2&!3&4&!5&!6&7 |
!0&!1&2&!3&4&!5&6&!7 | !0&1&!2&!3&!4&5&!6&7 | !0&1&!2&!3&!4&5&6&!7 |
!0&1&!2&!3&4&!5&!6&7 | !0&1&!2&!3&4&!5&6&!7 | 0&!1&!2&!3&!4&5&!6&7 |
0&!1&!2&!3&!4&5&6&!7 | 0&!1&!2&!3&4&!5&!6&7 | 0&!1&!2&!3&4&!5&6&!7] 0 {1}
[!0&!1&2&3&!4&!5&!6&7 | !0&!1&2&3&!4&!5&6&!7 | !0&1&!2&3&!4&!5&!6&7 |
!0&1&!2&3&!4&!5&6&!7 | 0&!1&!2&3&!4&!5&!6&7 | 0&!1&!2&3&!4&!5&6&!7] 2 {1}
State: 15
[!0&!1&2&!3&!4&5&6&!7 | !0&!1&2&!3&4&!5&6&!7 | !0&1&!2&!3&!4&5&!6&7 |
!0&1&!2&!3&!4&5&6&!7 | !0&1&!2&!3&4&!5&!6&7 | !0&1&!2&!3&4&!5&6&!7 |
0&!1&!2&!3&!4&5&!6&7 | 0&!1&!2&!3&!4&5&6&!7 | 0&!1&!2&!3&4&!5&!6&7 |
0&!1&!2&!3&4&!5&6&!7] 1 {1}
[!0&!1&2&3&!4&!5&6&!7 | !0&1&!2&3&!4&!5&!6&7 | !0&1&!2&3&!4&!5&6&!7 |
0&!1&!2&3&!4&!5&!6&7 | 0&!1&!2&3&!4&!5&6&!7] 2 {1}
[!0&!1&2&!3&!4&5&!6&7 | !0&!1&2&!3&4&!5&!6&7] 4 {1}
[!0&!1&2&3&!4&!5&!6&7] 6 {1}
State: 16
[!0&!1&2&!3&!4&5&!6&7 | !0&!1&2&!3&!4&5&6&!7 | !0&!1&2&!3&4&!5&!6&7 |
!0&!1&2&!3&4&!5&6&!7 | !0&1&!2&!3&!4&5&!6&7 | !0&1&!2&!3&!4&5&6&!7 |
!0&1&!2&!3&4&!5&!6&7 | !0&1&!2&!3&4&!5&6&!7 | 0&!1&!2&!3&!4&5&!6&7 |
0&!1&!2&!3&!4&5&6&!7 | 0&!1&!2&!3&4&!5&!6&7 | 0&!1&!2&!3&4&!5&6&!7] 1 {1}
[!0&!1&2&3&!4&!5&!6&7 | !0&!1&2&3&!4&!5&6&!7 | !0&1&!2&3&!4&!5&!6&7 |
!0&1&!2&3&!4&!5&6&!7 | 0&!1&!2&3&!4&!5&!6&7 | 0&!1&!2&3&!4&!5&6&!7] 2 {1}
State: 17
[!0&!1&2&!3&!4&5&!6&7 | !0&!1&2&!3&!4&5&6&!7 | !0&!1&2&!3&4&!5&!6&7 |
!0&!1&2&!3&4&!5&6&!7 | !0&1&!2&!3&!4&5&!6&7 | !0&1&!2&!3&!4&5&6&!7 |
!0&1&!2&!3&4&!5&!6&7 | !0&1&!2&!3&4&!5&6&!7 | 0&!1&!2&!3&!4&5&!6&7 |
0&!1&!2&!3&!4&5&6&!7 | 0&!1&!2&!3&4&!5&!6&7 | 0&!1&!2&!3&4&!5&6&!7] 0 {1}
[!0&!1&2&3&!4&!5&!6&7 | !0&!1&2&3&!4&!5&6&!7 | !0&1&!2&3&!4&!5&6&!7 |
0&!1&!2&3&!4&!5&!6&7 | 0&!1&!2&3&!4&!5&6&!7] 2 {1}
[!0&1&!2&3&!4&!5&!6&7] 6 {1}
State: 18
[!0&!1&2&!3&!4&5&!6&7 | !0&!1&2&!3&!4&5&6&!7 | !0&!1&2&!3&4&!5&!6&7 |
!0&!1&2&!3&4&!5&6&!7 | !0&1&!2&!3&!4&5&!6&7 | !0&1&!2&!3&!4&5&6&!7 |
!0&1&!2&!3&4&!5&!6&7 | !0&1&!2&!3&4&!5&6&!7 | 0&!1&!2&!3&!4&5&!6&7 |
0&!1&!2&!3&!4&5&6&!7 | 0&!1&!2&!3&4&!5&!6&7 | 0&!1&!2&!3&4&!5&6&!7] 0 {1}
[!0&!1&2&3&!4&!5&!6&7 | !0&!1&2&3&!4&!5&6&!7 | !0&1&!2&3&!4&!5&6&!7 |
0&!1&!2&3&!4&!5&!6&7 | 0&!1&!2&3&!4&!5&6&!7] 2 {1}
[!0&1&!2&3&!4&!5&!6&7] 5 {1}
State: 19
[!0&!1&2&!3&!4&5&!6&7 | !0&!1&2&!3&!4&5&6&!7 | !0&!1&2&!3&4&!5&!6&7 |
!0&!1&2&!3&4&!5&6&!7 | !0&1&!2&!3&!4&5&!6&7 | !0&1&!2&!3&!4&5&6&!7 |
!0&1&!2&!3&4&!5&!6&7 | !0&1&!2&!3&4&!5&6&!7 | 0&!1&!2&!3&!4&5&!6&7 |
0&!1&!2&!3&!4&5&6&!7 | 0&!1&!2&!3&4&!5&!6&7 | 0&!1&!2&!3&4&!5&6&!7] 0 {1}
[!0&!1&2&3&!4&!5&!6&7 | !0&!1&2&3&!4&!5&6&!7 | !0&1&!2&3&!4&!5&!6&7 |
0&!1&!2&3&!4&!5&!6&7 | 0&!1&!2&3&!4&!5&6&!7] 2 {1}
[!0&1&!2&3&!4&!5&6&!7] 6 {1}
State: 20
[!0&!1&2&!3&4&!5&!6&7 | !0&!1&2&!3&4&!5&6&!7 | !0&!1&2&3&!4&!5&!6&7 |
!0&!1&2&3&!4&!5&6&!7 | !0&1&!2&!3&4&!5&!6&7 | !0&1&!2&!3&4&!5&6&!7 |
!0&1&!2&3&!4&!5&!6&7 | !0&1&!2&3&!4&!5&6&!7 | 0&!1&!2&!3&4&!5&!6&7 |
0&!1&!2&!3&4&!5&6&!7 | 0&!1&!2&3&!4&!5&!6&7 | 0&!1&!2&3&!4&!5&6&!7] 0 {1}
[!0&!1&2&!3&!4&5&6&!7 | !0&1&!2&!3&!4&5&!6&7 | !0&1&!2&!3&!4&5&6&!7 |
0&!1&!2&!3&!4&5&!6&7 | 0&!1&!2&!3&!4&5&6&!7] 1 {1}
[!0&!1&2&!3&!4&5&!6&7] 3 {1}
State: 21
[!0&!1&2&!3&!4&5&!6&7 | !0&!1&2&!3&!4&5&6&!7 | !0&1&!2&!3&!4&5&!6&7 |
!0&1&!2&!3&!4&5&6&!7 | 0&!1&!2&!3&!4&5&!6&7 | 0&!1&!2&!3&!4&5&6&!7] 1 {1}
[!0&!1&2&!3&4&!5&!6&7 | !0&!1&2&!3&4&!5&6&!7 | !0&!1&2&3&!4&!5&!6&7 |
!0&!1&2&3&!4&!5&6&!7 | !0&1&!2&!3&4&!5&!6&7 | !0&1&!2&!3&4&!5&6&!7 |
!0&1&!2&3&!4&!5&!6&7 | !0&1&!2&3&!4&!5&6&!7 | 0&!1&!2&!3&4&!5&!6&7 |
0&!1&!2&!3&4&!5&6&!7 | 0&!1&!2&3&!4&!5&!6&7 | 0&!1&!2&3&!4&!5&6&!7] 2 {1}
State: 22
[!0&!1&2&!3&4&!5&!6&7 | !0&!1&2&!3&4&!5&6&!7 | !0&!1&2&3&!4&!5&!6&7 |
!0&!1&2&3&!4&!5&6&!7 | !0&1&!2&!3&4&!5&!6&7 | !0&1&!2&!3&4&!5&6&!7 |
!0&1&!2&3&!4&!5&!6&7 | !0&1&!2&3&!4&!5&6&!7 | 0&!1&!2&!3&4&!5&!6&7 |
0&!1&!2&!3&4&!5&6&!7 | 0&!1&!2&3&!4&!5&!6&7 | 0&!1&!2&3&!4&!5&6&!7] 0 {1}
[!0&!1&2&!3&!4&5&6&!7 | !0&1&!2&!3&!4&5&!6&7 | !0&1&!2&!3&!4&5&6&!7 |
0&!1&!2&!3&!4&5&!6&7 | 0&!1&!2&!3&!4&5&6&!7] 1 {1}
[!0&!1&2&!3&!4&5&!6&7] 4 {1}
State: 23
[!0&!1&2&!3&4&!5&!6&7 | !0&!1&2&!3&4&!5&6&!7 | !0&!1&2&3&!4&!5&!6&7 |
!0&!1&2&3&!4&!5&6&!7 | !0&1&!2&!3&4&!5&!6&7 | !0&1&!2&!3&4&!5&6&!7 |
!0&1&!2&3&!4&!5&!6&7 | !0&1&!2&3&!4&!5&6&!7 | 0&!1&!2&!3&4&!5&!6&7 |
0&!1&!2&!3&4&!5&6&!7 | 0&!1&!2&3&!4&!5&!6&7 | 0&!1&!2&3&!4&!5&6&!7] 0 {1}
[!0&!1&2&!3&!4&5&!6&7 | !0&!1&2&!3&!4&5&6&!7 | !0&1&!2&!3&!4&5&!6&7 |
!0&1&!2&!3&!4&5&6&!7 | 0&!1&!2&!3&!4&5&!6&7 | 0&!1&!2&!3&!4&5&6&!7] 1 {1}
State: 24
[!0&!1&2&!3&!4&5&!6&7 | !0&!1&2&!3&!4&5&6&!7 | !0&1&!2&!3&!4&5&6&!7 |
0&!1&!2&!3&!4&5&!6&7 | 0&!1&!2&!3&!4&5&6&!7] 1 {1}
[!0&!1&2&!3&4&!5&!6&7 | !0&!1&2&!3&4&!5&6&!7 | !0&!1&2&3&!4&!5&!6&7 |
!0&!1&2&3&!4&!5&6&!7 | !0&1&!2&!3&4&!5&6&!7 | !0&1&!2&3&!4&!5&6&!7 |
0&!1&!2&!3&4&!5&!6&7 | 0&!1&!2&!3&4&!5&6&!7 | 0&!1&!2&3&!4&!5&!6&7 |
0&!1&!2&3&!4&!5&6&!7] 2 {1}
[!0&1&!2&!3&!4&5&!6&7] 4 {1}
[!0&1&!2&!3&4&!5&!6&7 | !0&1&!2&3&!4&!5&!6&7] 6 {1}
State: 25
[!0&!1&2&!3&!4&5&!6&7 | !0&!1&2&!3&!4&5&6&!7 | !0&1&!2&!3&!4&5&6&!7 |
0&!1&!2&!3&!4&5&!6&7 | 0&!1&!2&!3&!4&5&6&!7] 1 {1}
[!0&!1&2&!3&4&!5&!6&7 | !0&!1&2&!3&4&!5&6&!7 | !0&!1&2&3&!4&!5&!6&7 |
!0&!1&2&3&!4&!5&6&!7 | !0&1&!2&!3&4&!5&6&!7 | !0&1&!2&3&!4&!5&6&!7 |
0&!1&!2&!3&4&!5&!6&7 | 0&!1&!2&!3&4&!5&6&!7 | 0&!1&!2&3&!4&!5&!6&7 |
0&!1&!2&3&!4&!5&6&!7] 2 {1}
[!0&1&!2&!3&!4&5&!6&7] 3 {1}
[!0&1&!2&!3&4&!5&!6&7 | !0&1&!2&3&!4&!5&!6&7] 5 {1}
State: 26
[!0&!1&2&!3&!4&5&!6&7 | !0&!1&2&!3&!4&5&6&!7 | !0&1&!2&!3&!4&5&!6&7 |
0&!1&!2&!3&!4&5&!6&7 | 0&!1&!2&!3&!4&5&6&!7] 1 {1}
[!0&!1&2&!3&4&!5&!6&7 | !0&!1&2&!3&4&!5&6&!7 | !0&!1&2&3&!4&!5&!6&7 |
!0&!1&2&3&!4&!5&6&!7 | !0&1&!2&!3&4&!5&!6&7 | !0&1&!2&3&!4&!5&!6&7 |
0&!1&!2&!3&4&!5&!6&7 | 0&!1&!2&!3&4&!5&6&!7 | 0&!1&!2&3&!4&!5&!6&7 |
0&!1&!2&3&!4&!5&6&!7] 2 {1}
[!0&1&!2&!3&!4&5&6&!7] 4 {1}
[!0&1&!2&!3&4&!5&6&!7 | !0&1&!2&3&!4&!5&6&!7] 6 {1}
--END--""")

tc.assertTrue(spot.solve_game(game, si))

games = spot.split_edges(game)
spot.set_state_players(games, spot.get_state_players(game))
tc.assertTrue(spot.solve_game(games, si))

g = spot.translate("GF(a&X(a)) -> GFb")
a = buddy.bdd_ithvar(g.register_ap("a"))
b = buddy.bdd_ithvar(g.register_ap("b"))
gdpa = spot.tgba_determinize(spot.degeneralize_tba(g),
                             False, True, True, False)
spot.change_parity_here(gdpa, spot.parity_kind_max, spot.parity_style_odd)
gsdpa = spot.split_2step(gdpa, b, True)
spot.colorize_parity_here(gsdpa, True)
tc.assertTrue(spot.solve_parity_game(gsdpa))
tc.assertEqual(spot.highlight_strategy(gsdpa).to_str("HOA", "1.1"),
"""HOA: v1.1
States: 18
Start: 0
AP: 2 "a" "b"
acc-name: parity max odd 5
Acceptance: 5 Fin(4) & (Inf(3) | (Fin(2) & (Inf(1) | Fin(0))))
properties: trans-labels explicit-labels trans-acc colored complete
properties: deterministic
spot.highlight.states: 0 4 1 4 2 4 3 4 4 4 5 4 6 4 7 4 8 4 9 4 """
+"""10 4 11 4 12 4 13 4 14 4 15 4 16 4 17 4
spot.highlight.edges: 15 4 17 4 20 4 22 4 24 4 26 4 28 4 30 4 31 4 32 4 33 4
spot.state-player: 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 1
controllable-AP: 1
--BODY--
State: 0
[!0] 7 {0}
[0] 8 {0}
State: 1
[!0] 9 {3}
[0] 10 {3}
State: 2
[!0] 11 {1}
[0] 12 {1}
State: 3
[!0] 9 {3}
[0] 13 {4}
State: 4
[!0] 11 {1}
[0] 14 {2}
State: 5
[!0] 15 {3}
[0] 16 {3}
State: 6
[!0] 15 {3}
[0] 17 {4}
State: 7
[!1] 1 {0}
[1] 2 {0}
State: 8
[!1] 3 {0}
[1] 4 {0}
State: 9
[!1] 1 {3}
[1] 5 {3}
State: 10
[!1] 3 {3}
[1] 6 {3}
State: 11
[!1] 2 {1}
[1] 2 {3}
State: 12
[!1] 4 {1}
[1] 4 {3}
State: 13
[!1] 3 {4}
[1] 4 {4}
State: 14
[!1] 4 {2}
[1] 4 {3}
State: 15
[t] 5 {3}
State: 16
[t] 6 {3}
State: 17
[t] 4 {4}
--END--"""
)

# Test the different parity conditions
gdpa = spot.tgba_determinize(spot.degeneralize_tba(g),
                             False, True, True, False)

g_test = spot.change_parity(gdpa, spot.parity_kind_max, spot.parity_style_odd)
g_test_split = spot.split_2step(g_test, b, True)
sp = spot.get_state_players(g_test_split)
g_test_split_c = spot.colorize_parity(g_test_split)
spot.set_state_players(g_test_split_c, sp)
tc.assertTrue(spot.solve_parity_game(g_test_split_c))
c_strat = spot.get_strategy(g_test_split_c)
# All versions of parity need to result in the same strategy
for kind in [spot.parity_kind_min, spot.parity_kind_max]:
    for style in [spot.parity_style_even, spot.parity_style_odd]:
        g_test_split1 = spot.change_parity(g_test_split, kind, style)
        spot.set_state_players(g_test_split1, sp)
        tc.assertTrue(spot.solve_parity_game(g_test_split1))
        c_strat1 = spot.get_strategy(g_test_split1)
        tc.assertTrue(c_strat == c_strat1)

# Test that strategies are not appended
# if solve is called multiple times
aut = spot.make_twa_graph()
aut.set_buchi()
aut.new_states(2)
aut.new_edge(0,1,buddy.bddtrue, [0])
aut.new_edge(1,0,buddy.bddtrue, [])
spot.set_state_players(aut, [False, True])
spot.solve_game(aut)
S1 = list(spot.get_strategy(aut))
spot.solve_game(aut)
S2 = list(spot.get_strategy(aut))
tc.assertEqual(S1, S2)


# Finite games
alive = "__alive__"
def finite_existential(auts):
    # 1 Accepting state -> selfloop
    # 2 Prune
    acc_state = set()
    sp = list(spot.get_state_players(auts))
    for e in auts.edges():
        if e.acc:
            acc_state.add(e.src)
    for s in acc_state:
        e_kill = auts.out_iteraser(s)
        while (e_kill):
            e_kill.erase()
    for s in acc_state:
        sprime = auts.new_state()
        sp.append(not sp[s])
        auts.new_edge(s, sprime, buddy.bddtrue, [0])
        auts.new_edge(sprime, s, buddy.bddtrue, [0])
    spot.set_state_players(auts, sp)
    auts.purge_dead_states()
    spot.alternate_players(auts, False, False)
    return auts

def is_input_complete(auts):
    sp = spot.get_state_players(auts)
    for s in range(auts.num_states()):
        if sp[s]:
            continue # Player
        cumul = buddy.bddfalse
        for e in auts.out(s):
            cumul |= e.cond
        if cumul != buddy.bddtrue:
            return False

    return True

def synt_from_ltlf(f:str, outs):
    ff = spot.from_ltlf(f, alive)
    aut = ff.translate("buchi", "sbacc")
    outbdd = buddy.bddtrue
    for out in outs:
        outbdd &= buddy.bdd_ithvar(aut.register_ap(out))
    alive_bdd = buddy.bdd_ithvar(aut.register_ap(alive))
    auts = spot.split_2step(aut, outbdd & alive_bdd, False)
    auts = spot.to_finite(auts, alive)
    spot.alternate_players(auts, False, False)
    spot.set_synthesis_outputs(auts, outbdd)
    if not is_input_complete(auts):
        print("Not synthesizable")
        return None
    auts = finite_existential(auts)

    return auts

def synt_ltlf(f:str, outs, res:str = "aut"):
    auts = synt_from_ltlf(f, outs)

    succ = spot.solve_parity_game(auts)
    if not succ:
        if res == "aut":
            return False, auts
        else:
            return False, None

    mealy_cc = spot.solved_game_to_split_mealy(auts)

    if res == "aut":
        return True, mealy_cc
    elif res == "aig":
        return True, spot.mealy_machine_to_aig(mealy_cc, "isop")
    else:
        raise RuntimeError("Unknown option")


sink_player = None

def negate_ltlf(f:str, outs, opt = "buchi"):

    global sink_player
    sink_player = None

    aut = synt_from_ltlf(f, outs)
    # Implies input completeness
    # We need output completeness
    acc = []

    sp = list(spot.get_state_players(aut))

    def get_sink():
        global sink_player
        if sink_player is None:
            sink_player = aut.new_states(2)
            aut.new_edge(sink_player, sink_player + 1, buddy.bddtrue, acc)
            aut.new_edge(sink_player + 1, sink_player, buddy.bddtrue, acc)
            sp.append(False)
            sp.append(True)
            spot.set_state_players(aut, sp)
        return sink_player

    for s in range(aut.num_states()):
        if not sp[s]:
            continue
        rem = buddy.bddtrue
        for e in aut.out(s):
            rem -= e.cond
        if rem != buddy.bddfalse:
            aut.new_edge(s, get_sink(), rem)

    # Better to invert colors or condition?
    if opt == "buchi":
        for e in aut.edges():
            if e.acc:
                e.acc = spot.mark_t()
            else:
                e.acc = spot.mark_t([0])
    elif opt == "cobuchi":
        aut.set_co_buchi()
    else:
        raise RuntimeError("Unknown opt")
    return aut

# Game where the edge_vector is larger
# than the number of transitions
f1 = "((((G (F (idle))) && (G (((idle) && (X ((! (grant_0)) \
      && (! (grant_1))))) -> (X (idle))))) && (G ((X (! (grant_0))) \
      || (X (((! (request_0)) && (! (idle))) U ((! (request_0)) \
      && (idle))))))) -> (((G (((((X (((! (grant_0)) && (true)) \
      || ((true) && (! (grant_1))))) && ((X (grant_0)) -> (request_0))) \
      && ((X (grant_1)) -> (request_1))) && ((request_0) -> (grant_1))) \
      && ((! (idle)) -> (X ((! (grant_0)) && (! (grant_1))))))) \
      && (! (F (G ((request_0) && (X (! (grant_0)))))))) \
      && (! (F (G ((request_1) && (X (! (grant_1)))))))))"
outs = ["grant_0", "grant1"]
tc.assertEqual(synt_ltlf(f1, outs)[0], False)