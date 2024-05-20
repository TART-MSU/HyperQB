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

# Test for Issue #471.

import spot
from unittest import TestCase
tc = TestCase()

a = spot.translate('Fa')
a = spot.to_generalized_rabin(a, False)
r1 = a.intersecting_run(a)
r2 = a.accepting_run()
tc.assertEqual(str(r1), str(r2))
tc.assertTrue(a.prop_weak().is_true())
