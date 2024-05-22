# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2020, 2022 Laboratoire de Recherche et Développement de l'Epita
# (LRDE).
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

# Test for parts of Issue #497.

import spot
from unittest import TestCase
tc = TestCase()

aut = spot.automaton("""
HOA: v1
States: 1
Start: 0
acc-name: who cares
Acceptance: 0 t
properties: very-weak
AP: 3 "x" "y" "z"
Alias: @p0 0
Alias: @p1 1
Alias: @a !@p0&!@p1
Alias: @b !@p0&@p1
Alias: @c @p0&!@p1
--BODY--
State: 0
[@p0] 0
[!@p0] 0
[@p1] 0
[!@p1] 0
[@a] 0
[!@a] 0
[@b] 0
[!@b] 0
[@c] 0
[!@c] 0
[@a|@b] 0
[@a|@c] 0
[@b|@c] 0
[@a|@b|@c] 0
[!@a|!@b] 0
[!@a|!@c] 0
[!@b|!@c] 0
[@p0&@p1] 0
[!@a&!@b] 0
[!@a&!@c] 0
[!@b&!@c] 0
[!@b&!@c&2] 0
[@a&2] 0
[!@a&2] 0
--END--""")
s = aut.to_str('hoa')
aut2 = spot.automaton(s)
tc.assertTrue(aut.equivalent_to(aut2))
s2 = aut.to_str('hoa')
tc.assertEqual(s, s2)

tc.assertEqual(s, """HOA: v1
States: 1
Start: 0
AP: 3 "x" "y" "z"
acc-name: all
Acceptance: 0 t
properties: trans-labels explicit-labels state-acc complete very-weak
Alias: @p0 0
Alias: @p1 1
Alias: @a !@p1&!@p0
Alias: @b @p1&!@p0
Alias: @c !@p1&@p0
--BODY--
State: 0
[@p0] 0
[!@p0] 0
[@p1] 0
[!@p1] 0
[@a] 0
[!@a] 0
[@b] 0
[!@b] 0
[@c] 0
[!@c] 0
[!@p0] 0
[!@p1] 0
[@c | @b] 0
[@c | @b | @a] 0
[t] 0
[t] 0
[t] 0
[@p1&@p0] 0
[@p0] 0
[@p1] 0
[!@c&!@b] 0
[@a&2 | @p1&@p0&2] 0
[@a&2] 0
[@p0&2 | @p1&2] 0
--END--""")

# Check what happens to aliases when an AP has been removed, but
# the aliases have been preserved...
rem = spot.remove_ap()
rem.add_ap("x")
aut3 = rem.strip(aut)
spot.set_aliases(aut3, spot.get_aliases(aut))
s2 = aut3.to_str('hoa')
# Aliases based on "x" should have disappeared.
tc.assertEqual(s2, """HOA: v1
States: 1
Start: 0
AP: 2 "y" "z"
acc-name: all
Acceptance: 0 t
properties: trans-labels explicit-labels state-acc complete very-weak
Alias: @p1 0
--BODY--
State: 0
[t] 0
[t] 0
[@p1] 0
[!@p1] 0
[!@p1] 0
[t] 0
[@p1] 0
[t] 0
[!@p1] 0
[t] 0
[t] 0
[!@p1] 0
[t] 0
[t] 0
[t] 0
[t] 0
[t] 0
[@p1] 0
[t] 0
[@p1] 0
[t] 0
[1] 0
[!@p1&1] 0
[1] 0
--END--""")
