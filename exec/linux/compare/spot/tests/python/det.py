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

import spot
from unittest import TestCase
tc = TestCase()

a = spot.translate('FGa | FGb')

# The 8th argument requests the computation of original-classes.
d = spot.tgba_determinize(a, False, True, True, True,
                          None, -1, True)
cld = list(d.get_original_classes())
tc.assertEqual([0, 1, 2, 3, 3], cld)

e = spot.sbacc(d)
tc.assertIsNone(e.get_original_states())
cle = list(e.get_original_classes())
tc.assertEqual(len(cle), e.num_states())
tc.assertEqual(set(cle), set(cld))
