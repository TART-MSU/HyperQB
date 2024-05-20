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
import spot.gen as gen
from unittest import TestCase
tc = TestCase()

# Make sure we can iterate on the states of a power_map.
a = gen.aut_pattern(gen.AUT_CYCLIST_TRACE_NBA, 1)
p = spot.power_map()
d = spot.tgba_powerset(a, p)
tc.assertEqual(p.states_of(0), (0,))
tc.assertEqual(p.states_of(1), (0,1))
tc.assertEqual(p.states_of(2), (0,2))
