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

# Test for parts of Issue #298.

import spot
from unittest import TestCase
tc = TestCase()

a1 = spot.automaton("""genltl --dac=51 | ltl2tgba --med |""")
a1 = spot.degeneralize_tba(a1)
r1 = spot.tgba_determinize(a1, True, False, False)
tc.assertEqual(r1.num_sets(), 3)
tc.assertTrue(a1.prop_complete().is_false())
# This used to fail in 2.9.5 and earlier.
tc.assertTrue(r1.prop_complete().is_maybe())
tc.assertTrue(spot.is_complete(r1))

a2 = spot.automaton("""genltl --dac=51 | ltl2tgba --high |""")
a2 = spot.degeneralize_tba(a2)
r2 = spot.tgba_determinize(a2, True, False, False)
# This used to fail in 2.9.5 and earlier.
tc.assertEqual(r2.num_sets(), 3)
tc.assertTrue(a2.prop_complete().is_false())
tc.assertTrue(r2.prop_complete().is_maybe())
tc.assertTrue(spot.is_complete(r2))
