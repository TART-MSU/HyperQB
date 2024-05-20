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


a = spot.automaton("""
HOA: v1
States: 3
Start: 0
AP: 3 "p0" "p1" "p2"
Acceptance: 3 Fin(0) & Inf(1) & Fin(2)
properties: trans-labels explicit-labels trans-acc deterministic
--BODY--
State: 0
[!0] 1
[0&!1&2] 0
[0&!1&!2] 0 {2}
[0&1&2] 0 {1}
[0&1&!2] 0 {1 2}
State: 1
[!1&2] 2
[!1&!2] 2 {2}
[1&2] 2 {1}
[1&!2] 2 {1 2}
State: 2
[0&!1&2] 2
[0&!1&!2] 2 {2}
[0&1&2] 2 {1}
[0&1&!2] 2 {1 2}
--END--""")
res = a.postprocess("small", "high", "parity min odd", "colored")
tc.assertEqual(res.to_str(), """HOA: v1
States: 3
Start: 0
AP: 3 "p0" "p1" "p2"
acc-name: parity min odd 3
Acceptance: 3 Fin(0) & (Inf(1) | Fin(2))
properties: trans-labels explicit-labels trans-acc colored
properties: deterministic
--BODY--
State: 0
[0&!2] 0 {0}
[0&1&2] 0 {1}
[0&!1&2] 0 {2}
[!0] 1 {0}
State: 1
[t] 2 {0}
State: 2
[0&!2] 2 {0}
[0&1&2] 2 {1}
[0&!1&2] 2 {2}
--END--""")
