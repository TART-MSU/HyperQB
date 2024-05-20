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


for f in ['Fb', 'GFa & GFb',
          '(p0 W Fp0) R ((Gp1 & Fp1) | (F!p1 & G!p1)) & GFp0',
          'GFp0 xor GFp1']:
    aut = spot.translate(f, 'BA')
    tc.assertFalse(spot.is_safety_automaton(aut))

    aut = spot.translate(f, 'BA')
    tc.assertFalse(spot.is_safety_automaton(aut))

    aut = spot.translate(f, 'deterministic', 'complete')
    tc.assertFalse(spot.is_safety_automaton(aut))

    aut = spot.translate(f, 'generic', 'sbacc')
    tc.assertFalse(spot.is_safety_automaton(aut))

for f in ['Gb', 'Ga|Gb|Gc', 'Fr->(!p U r)', 'p1 M F(p1 U (Gp0 U X(0)))',
          '((p1 U !p0) M !FXp1) W p0', 'p0 & ((!p1 | (p1 W X!p1)) M p1)',
          '(p0 W Fp0) R ((Gp1 & Fp1) | (F!p1 & G!p1))']:
    aut = spot.translate(f, 'BA')
    tc.assertTrue(spot.is_safety_automaton(aut))

    ba = spot.translate(f, 'BA', 'complete')
    tc.assertTrue(spot.is_safety_automaton(aut))

    ba = spot.translate(f, 'deterministic', 'complete')
    tc.assertTrue(spot.is_safety_automaton(aut))

    ba = spot.translate(f, 'generic', 'sbacc')
    tc.assertTrue(spot.is_safety_automaton(aut))
