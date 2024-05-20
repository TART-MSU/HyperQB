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
from unittest import TestCase
tc = TestCase()

v1 = spot.trival()
v2 = spot.trival(False)
v3 = spot.trival(True)
v4 = spot.trival_maybe()
tc.assertNotEqual(v1, v2)
tc.assertNotEqual(v1, v3)
tc.assertNotEqual(v2, v3)
tc.assertEqual(v2, spot.trival(spot.trival.no_value))
tc.assertNotEqual(v2, spot.trival(spot.trival.yes_value))
tc.assertNotEqual(v4, v2)
tc.assertNotEqual(v4, v3)
tc.assertEqual(v2, False)
tc.assertEqual(True, v3)
tc.assertNotEqual(v2, True)
tc.assertNotEqual(False, v3)
tc.assertEqual(v4, spot.trival_maybe())
tc.assertEqual(v4, spot.trival(spot.trival.maybe_value))
tc.assertTrue(v3)
tc.assertTrue(-v2)
tc.assertFalse(-v1)
tc.assertFalse(v1)
tc.assertFalse(-v3)

for u in (v1, v2, v3):
    for v in (v1, v2, v3):
        tc.assertEqual((u & v), -(-u | -v))
