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
from sys import exit
from unittest import TestCase
tc = TestCase()

# CPython use reference counting, so that automata are destructed
# when we expect them to be.   However other implementations like
# PyPy may call destructors latter, causing different output.
from platform import python_implementation
if python_implementation() == 'CPython':
    def gcollect():
        pass
else:
    import gc
    def gcollect():
        gc.collect()

aut = spot.automaton("""
HOA: v1
States: 2
Start: 0
AP: 2 "a" "b"
acc-name: Buchi
Acceptance: 1 Inf(0)
--BODY--
State: 0
[t] 1 {0}
State: 1
[t] 1 {0}
--END--
""")

aut2 = spot.simulation(aut)
tc.assertEqual(aut2.to_str(), """HOA: v1
States: 1
Start: 0
AP: 2 "a" "b"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc colored complete
properties: deterministic
--BODY--
State: 0 {0}
[t] 0
--END--""")

aut2.copy_state_names_from(aut)
tc.assertEqual(aut2.to_str(), """HOA: v1
States: 1
Start: 0
AP: 2 "a" "b"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc colored complete
properties: deterministic
--BODY--
State: 0 "[0,1]" {0}
[t] 0
--END--""")

del aut
del aut2
gcollect()

aut = spot.translate('GF((p0 -> Gp0) R p1)')

daut = spot.tgba_determinize(aut, True)
tc.assertEqual(daut.to_str(), """HOA: v1
States: 3
Start: 0
AP: 2 "p1" "p0"
acc-name: parity min even 3
Acceptance: 3 Inf(0) | (Fin(1) & Inf(2))
properties: trans-labels explicit-labels trans-acc complete
properties: deterministic stutter-invariant
--BODY--
State: 0 "{₀[0]₀}"
[!0&!1] 0
[!0&1] 0
[0&!1] 0 {0}
[0&1] 1 {2}
State: 1 "{₀[0]{₂[2]₂}₀}{₁[1]₁}"
[!0&!1] 0 {1}
[!0&1] 2
[0&!1] 0 {0}
[0&1] 1 {2}
State: 2 "{₀[0]₀}{₁[1]₁}"
[!0&!1] 0 {1}
[!0&1] 2
[0&!1] 0 {0}
[0&1] 1 {2}
--END--""")

del aut
del daut
gcollect()

aut = spot.automaton("""
HOA: v1
States: 2
Start: 0
AP: 2 "a" "b"
Acceptance: 2 Inf(0)&Inf(1)
--BODY--
State: 0
[0] 0
[1] 1 {1}
State: 1
[0] 1 {0}
[1] 0
--END--
""")

daut = spot.tgba_determinize(aut, True)
tc.assertEqual(daut.to_str(), """HOA: v1
States: 12
Start: 0
AP: 2 "a" "b"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels trans-acc deterministic
--BODY--
State: 0 "{₀[0#0,0#1]₀}"
[!0&1] 1
[0&!1] 0
[0&1] 2
State: 1 "{₀[1#1]₀}"
[!0&1] 0
[0&!1] 3 {0}
[0&1] 4
State: 2 "{₀[0#0,0#1] [1#1]₀}"
[!0&1] 2
[0&!1] 4
[0&1] 5
State: 3 "{₀[1#0]₀}"
[!0&1] 0
[0&!1] 3
[0&1] 6
State: 4 "{₀[0#0,0#1]{₁[1#0]₁}₀}"
[!0&1] 7
[0&!1] 4
[0&1] 8
State: 5 "{₀[0#0,0#1] [1#1]{₁[1#0]₁}₀}"
[!0&1] 7
[0&!1] 4
[0&1] 8
State: 6 "{₀[0#0,0#1] [1#0]₀}"
[!0&1] 2
[0&!1] 6
[0&1] 9
State: 7 "{₀[1#1]{₁[0#0,0#1]₁}₀}"
[!0&1] 10
[0&!1] 6 {0}
[0&1] 9 {0}
State: 8 "{₀[1#1]{₁[0#0,0#1] [1#0]₁}₀}"
[!0&1] 2 {0}
[0&!1] 6 {0}
[0&1] 9 {0}
State: 9 "{₀[0#0,0#1] [1#1] [1#0]₀}"
[!0&1] 2
[0&!1] 4
[0&1] 5
State: 10 "{₀[0#0,0#1]{₁[1#1]₁}₀}"
[!0&1] 7
[0&!1] 4 {0}
[0&1] 11
State: 11 "{₀[1#1]{₁[0#0,0#1]{₂[1#0]₂}₁}₀}"
[!0&1] 2 {0}
[0&!1] 6 {0}
[0&1] 9 {0}
--END--""")

a = spot.translate('!Gp0 xor FG((p0 W Gp1) M p1)')
a = spot.degeneralize_tba(a)
tc.assertEqual(a.num_states(), 8)
b = spot.simulation(a)
tc.assertEqual(b.num_states(), 3)
b.set_init_state(1)
b.purge_unreachable_states()
b.copy_state_names_from(a)

tc.assertEqual(b.to_str(), """HOA: v1
States: 1
Start: 0
AP: 2 "p0" "p1"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels trans-acc complete
properties: deterministic stutter-invariant
--BODY--
State: 0 "[1,7]"
[1] 0
[!1] 0 {0}
--END--""")

aut = spot.automaton('''HOA: v1
States: 12
Start: 0
AP: 2 "a" "b"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels trans-acc deterministic
--BODY--
State: 0
[!0&1] 1
[0&!1] 0
[0&1] 2
State: 1
[!0&1] 0
[0&!1] 3 {0}
[0&1] 4
State: 2
[!0&1] 2
[0&!1] 4
[0&1] 5
State: 3
[!0&1] 0
[0&!1] 3
[0&1] 6
State: 4
[!0&1] 7
[0&!1] 4
[0&1] 8
State: 5
[!0&1] 7
[0&!1] 4
[0&1] 8
State: 6
[!0&1] 2
[0&!1] 6
[0&1] 9
State: 7
[!0&1] 10
[0&!1] 6 {0}
[0&1] 9 {0}
State: 8
[!0&1] 2 {0}
[0&!1] 6 {0}
[0&1] 9 {0}
State: 9
[!0&1] 2
[0&!1] 4
[0&1] 5
State: 10
[!0&1] 7
[0&!1] 4 {0}
[0&1] 11
State: 11
[!0&1] 2 {0}
[0&!1] 6 {0}
[0&1] 9 {0}
--END--''')
tc.assertEqual(spot.reduce_iterated(aut).to_str(), '''HOA: v1
States: 9
Start: 0
AP: 2 "a" "b"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels trans-acc deterministic
--BODY--
State: 0
[0&!1] 0
[!0&1] 1
[0&1] 2
State: 1
[!0&1] 0
[0&!1] 3 {0}
[0&1] 4
State: 2
[!0&1] 2
[0] 4
State: 3
[!0&1] 0
[0&!1] 3
[0&1] 5
State: 4
[0&!1] 4
[!0&1] 6
[0&1] 7
State: 5
[1] 2
[0&!1] 5
State: 6
[0&1] 2 {0}
[0&!1] 5 {0}
[!0&1] 8
State: 7
[1] 2 {0}
[0&!1] 5 {0}
State: 8
[0&!1] 4 {0}
[!0&1] 6
[0&1] 7
--END--''')

aut = spot.automaton('''HOA: v1
States: 6
Start: 0
AP: 2 "a" "b"
acc-name: co-Buchi
Acceptance: 1 Fin(0)
properties: trans-labels explicit-labels state-acc very-weak
--BODY--
State: 0
[1] 1
[1] 2
State: 1 {0}
[0] 1
State: 2
[0] 3
State: 3
[1] 3
State: 4
[1] 5
State: 5
[0] 5
--END--''')
tc.assertEqual(spot.reduce_iterated(aut).to_str(), '''HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
acc-name: co-Buchi
Acceptance: 1 Fin(0)
properties: trans-labels explicit-labels state-acc deterministic
properties: very-weak
--BODY--
State: 0 {0}
[1] 1
State: 1
[0] 2
State: 2
[1] 2
--END--''')

aut = spot.automaton('''HOA: v1
States: 5
Start: 0
AP: 4 "p0" "p1" "p2" "p3"
Acceptance: 2 Fin(0) & Fin(1)
properties: trans-labels explicit-labels trans-acc
--BODY--
State: 0
[0&!1&!2&3] 1
State: 1
[0&1&!2&3] 2 {0 1}
[0&1&!2&3] 3 {0 1}
[0&1&2&!3] 0
[0&1&!2&3] 4 {0 1}
[0&1&!2&3] 1 {0 1}
State: 2
[0&1&!2&3] 2
State: 3
[0&1&!2&3] 2 {1}
[0&1&!2&3] 3 {1}
State: 4
[0&1&!2&3] 2 {0}
[0&1&!2&3] 4 {0}
--END--''')

tc.assertEqual(spot.reduce_direct_cosim(aut).to_str(), '''HOA: v1
States: 5
Start: 0
AP: 4 "p0" "p2" "p3" "p1"
Acceptance: 2 Fin(0) & Fin(1)
properties: trans-labels explicit-labels trans-acc
--BODY--
State: 0
[0&!1&2&!3] 1
State: 1
[0&1&!2&3] 0
[0&!1&2&3] 1 {0 1}
[0&!1&2&3] 2 {0 1}
[0&!1&2&3] 3 {0 1}
[0&!1&2&3] 4 {0 1}
State: 2
[0&!1&2&3] 2
State: 3
[0&!1&2&3] 3 {1}
State: 4
[0&!1&2&3] 4 {0}
--END--''')

aut = spot.automaton('''HOA: v1
States: 2
Start: 0
AP: 2 "a" "b"
Acceptance: 2 Fin(0) & Fin(1)
properties: trans-labels explicit-labels trans-acc
--BODY--
State: 0
[0] 1 {0}
[0] 1 {1}
State: 1
[0] 0
--END--''')
tc.assertEqual(spot.reduce_direct_sim(aut).to_str(), '''HOA: v1
States: 1
Start: 0
AP: 2 "a" "b"
Acceptance: 2 Fin(0) & Fin(1)
properties: trans-labels explicit-labels state-acc deterministic
--BODY--
State: 0
--END--''')

aut = spot.automaton('''HOA: v1
name: "(p1 U p2) U p3"
States: 4
Start: 0
AP: 3 "p1" "p2" "p3"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc stutter-invariant
properties: terminal
--BODY--
State: 0
[1&!2] 0
[2] 1
[0&!2] 2
State: 1 {0}
[t] 1
State: 2
[1&!2] 0
[1&2] 1
[0&!1 | 0&!2] 2
[0&!1&2] 3
State: 3
[1] 1
[0&!1] 3
--END--''')
tc.assertEqual(spot.reduce_direct_cosim_sba(aut).to_str(), '''HOA: v1
States: 4
Start: 0
AP: 3 "p2" "p3" "p1"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc stutter-invariant
properties: terminal
--BODY--
State: 0
[0&!1] 0
[1] 1
[!1&2] 2
State: 1 {0}
[t] 1
State: 2
[0&!1] 0
[0&1] 1
[!0&2 | !1&2] 2
[!0&1&2] 3
State: 3
[0] 1
[!0&2] 3
--END--''')

aut = spot.automaton('''HOA: v1
States: 4
Start: 0
AP: 2 "a" "b"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc
--BODY--
State: 0
[0] 1
State: 1
[1] 2
[1] 3
State: 2
[1] 2
State: 3 {0}
[1] 3
--END--''')
tc.assertEqual(spot.reduce_direct_cosim(aut).to_str(), '''HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc deterministic
--BODY--
State: 0
[0] 1
State: 1
[1] 2
State: 2 {0}
[1] 2
--END--''')

tc.assertEqual(spot.reduce_direct_sim_sba(aut).to_str(), '''HOA: v1
States: 2
Start: 0
AP: 2 "a" "b"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc deterministic
--BODY--
State: 0
[0] 1
State: 1 {0}
[1] 1
--END--''')

aut = spot.automaton('''HOA: v1
States: 3
Start: 0
AP: 1 "a"
Acceptance: 1 t
properties: trans-labels explicit-labels state-acc deterministic
--BODY--
State: 0
[0] 1
State: 1
[0] 2
State: 2 {0}
[0] 2
--END--''')
tc.assertEqual(spot.reduce_iterated_sba(aut).to_str(), '''HOA: v1
States: 1
Start: 0
AP: 1 "a"
acc-name: all
Acceptance: 0 t
properties: trans-labels explicit-labels state-acc deterministic
properties: very-weak
--BODY--
State: 0
[0] 0
--END--''')

aut = spot.automaton('''HOA: v1
States: 30
Start: 0
AP: 4 "c" "d" "a1" "b1"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels trans-acc weak
--BODY--
State: 0
[0&!1&!2&!3] 1
[0&!1&!2&!3] 2
State: 1
[!0&!1&!2&3] 3
[!0&!1&2&!3] 4
State: 2
[!0&!1&!2&3 | !0&!1&2&!3] 5
[!0&1&!2&!3] 6
State: 3
[0&!1&!2&!3] 7
State: 4
[0&!1&!2&!3] 8
State: 5
[0&!1&!2&!3] 1
[0&!1&!2&!3] 9
State: 6
[!0&!1&!2&3 | !0&!1&2&!3] 10
State: 7
[!0&!1&!2&3 | !0&!1&2&!3] 3
[!0&1&!2&!3] 11
[!0&1&!2&!3] 12
[0&!1&!2&!3] 13
State: 8
[!0&!1&!2&3 | !0&!1&2&!3] 4
[!0&1&!2&!3] 14
[!0&1&!2&!3] 15
[0&!1&!2&!3] 16
State: 9
[!0&!1&!2&3 | !0&!1&2&!3] 5
[!0&1&!2&!3] 6
[0&!1&!2&!3] 17
[0&!1&!2&!3] 18
State: 10
[0&!1&!2&!3] 17 {0}
[0&!1&!2&!3] 19
State: 11
[!0&!1&2&!3] 20
[!0&!1&!2&3] 21 {0}
State: 12
[!0&!1&!2&3 | !0&!1&2&!3] 22
State: 13
[0&!1&!2&!3] 13
State: 14
[!0&!1&2&!3] 21 {0}
[!0&!1&!2&3] 23
State: 15
[!0&!1&!2&3 | !0&!1&2&!3] 24
State: 16
[0&!1&!2&!3] 16
State: 17
State: 18
[0&!1&!2&!3] 17
[0&!1&!2&!3] 18
State: 19
[0&!1&!2&!3] 17 {0}
[0&!1&!2&!3] 19
State: 20
[0&!1&!2&!3] 25
State: 21
[0&!1&!2&!3] 26 {0}
State: 22
[0&!1&!2&!3] 27
State: 23
[0&!1&!2&!3] 28
State: 24
[0&!1&!2&!3] 29
State: 25
[0&!1&!2&!3] 25
State: 26
[0&!1&!2&!3] 26 {0}
State: 27
[0&!1&!2&!3] 27
State: 28
[0&!1&!2&!3] 28
State: 29
[0&!1&!2&!3] 29
--END--''')
tc.assertEqual(spot.reduce_iterated(a).to_str(), '''HOA: v1
States: 8
Start: 0
AP: 2 "p0" "p1"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels trans-acc stutter-invariant
--BODY--
State: 0
[0] 0
[!0] 1 {0}
[!0&1] 2
[0&1] 3 {0}
State: 1
[1] 1
[!1] 1 {0}
[!0&1] 2
[0&1] 4
State: 2
[!1] 1 {0}
[!0&1] 2
[0&1] 5
State: 3
[0&1] 3 {0}
State: 4
[0&!1] 6
State: 5
[!0&!1] 1 {0}
[!0&1] 2
[0&1] 5
[0&!1] 6 {0}
State: 6
[!0&!1] 1 {0}
[0] 6
[!0&1] 7
State: 7
[!1] 1 {0}
[0&1] 5
[1] 7
--END--''')


# issue #452
aut = spot.automaton("""HOA: v1
States: 9
Start: 0
AP: 4 "p0" "p1" "p2" "p3"
Acceptance: 2 Fin(0)&Fin(1)
Alias: @p 0&1&!2&3
--BODY--
State: 0
[@p] 1 {0 1}
State: 1
[@p] 2 {0 1}
State: 2
[@p] 0 {0 1}
[@p] 2
[@p] 3 {0}
[@p] 4 {1}
State: 3
[@p] 5 {0 1}
[@p] 6 {0}
State: 4
[@p] 7 {0 1}
[@p] 8 {1}
State: 5
[@p] 8 {0 1}
State: 6
[@p] 2 {0}
[@p] 4 {0 1}
State: 7
[@p] 6 {0 1}
State: 8
[@p] 2 {1}
[@p] 3 {0 1}
--END--""")
aut = spot.simulation(aut)
tc.assertEqual(aut.num_states(), 1)
