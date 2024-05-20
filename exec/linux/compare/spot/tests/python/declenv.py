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


# This file tests various error conditions on the twa API

import spot
from unittest import TestCase
tc = TestCase()

env = spot.declarative_environment()
env.declare("a")
env.declare("b")

f1a = spot.parse_infix_psl("a U b")
f1b = spot.parse_infix_psl("a U b", env)
tc.assertFalse(f1a.errors)
tc.assertFalse(f1b.errors)

# In the past, atomic propositions requires via different environments were
# never equal, but this feature was never used and we changed that in Spot 2.0
# for the sake of simplicity.
tc.assertEqual(f1a.f, f1b.f)

f2 = spot.parse_infix_psl("(a U b) U c", env)
tc.assertTrue(f2.errors)
ostr = spot.ostringstream()
f2.format_errors(ostr)
err = ostr.str()
tc.assertIn("unknown atomic proposition `c'", err)

f3 = spot.parse_prefix_ltl("R a d", env)
tc.assertTrue(f3.errors)
ostr = spot.ostringstream()
f3.format_errors(ostr)
err = ostr.str()
tc.assertIn("unknown atomic proposition `d'", err)

f4 = spot.parse_prefix_ltl("R a b", env)
tc.assertFalse(f4.errors)
