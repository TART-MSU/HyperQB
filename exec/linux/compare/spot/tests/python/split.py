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
import buddy
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



def incl(a, b):
    return not b.intersects(spot.dualize(spot.tgba_determinize(a)))


def equiv(a, b):
    return incl(a, b) and incl(b, a)


def do_split(f, out_list):
    aut = spot.translate(f)
    outputs = buddy.bddtrue
    for a in out_list:
        outputs &= buddy.bdd_ithvar(aut.register_ap(a))
    s = spot.split_2step(aut, outputs, False)
    return aut, s

aut, s = do_split('(FG !a) <-> (GF b)', ['b'])
tc.assertTrue(equiv(aut, spot.unsplit_2step(s)))

del aut
del s
gcollect()

aut, s = do_split('GFa && GFb', ['b'])
tc.assertTrue(equiv(aut, spot.unsplit_2step(s)))
# FIXME s.to_str() is NOT the same on Debian stable and on Debian unstable
#       we should investigate this.  See Issue #502.
# tc.assertEqual("""HOA: v1
# States: 3
# Start: 0
# AP: 2 "a" "b"
# acc-name: generalized-Buchi 2
# Acceptance: 2 Inf(0)&Inf(1)
# properties: trans-labels explicit-labels trans-acc complete
# properties: deterministic
# spot-state-player: 0 1 1
# --BODY--
# State: 0
# [0] 1
# [!0] 2
# State: 1
# [!1] 0 {0}
# [1] 0 {0 1}
# State: 2
# [!1] 0
# [1] 0 {1}
# --END--""", s.to_str() )

del aut
del s
gcollect()

aut, s = do_split('! ((G (req -> (F ack))) && (G (go -> (F grant))))', ['ack'])
tc.assertTrue(equiv(aut, spot.unsplit_2step(s)))

# FIXME s.to_str() is NOT the same on Debian stable and on Debian unstable
#       we should investigate this.  See Issue #502.
# tc.assertEqual(s.to_str(), """HOA: v1
# States: 9
# Start: 0
# AP: 4 "ack" "req" "go" "grant"
# acc-name: Buchi
# Acceptance: 1 Inf(0)
# properties: trans-labels explicit-labels state-acc
# --BODY--
# State: 0
# [1&!2] 3
# [!1&!2] 4
# [1&2] 5
# [!1&2] 6
# State: 1
# [t] 7
# State: 2
# [t] 8
# State: 3
# [t] 0
# [!0] 1
# State: 4
# [t] 0
# State: 5
# [t] 0
# [!0] 1
# [!3] 2
# State: 6
# [t] 0
# [!3] 2
# State: 7 {0}
# [!0] 1
# State: 8 {0}
# [!3] 2
# --END--""")

del aut
del s
gcollect()

aut, s = do_split('((G (((! g_0) || (! g_1)) && ((r_0 && (X r_1)) -> (F (g_0 \
    && g_1))))) && (G (r_0 -> F g_0))) && (G (r_1 -> F g_1))',
                  ['g_0', 'g_1'])
tc.assertTrue(equiv(aut, spot.unsplit_2step(s)))
