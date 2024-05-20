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

formulas = """\
{x[*]}[]-> F({y[*]}<>-> GFz)
<>(({{p12}[*0..3]}[]-> ((p9) || (!(p17)))) V ((true) U (p17)))
{{true} || {[*0]}}[]-> (false)
{{p14} & {{p0}[*]}}[]-> (p11)
{{{!{p6}} -> {!{p3}}}[*]}[]-> ((p3)V((p3) || ((X((false))) && ((p2)V(p18)))))
"""

for f1 in formulas.splitlines():
    f1 = spot.formula(f1)
    a1 = spot.translate(f1)

    f2, aps = spot.suffix_operator_normal_form(f1, 'sonf_')
    a2 = spot.translate(f2)
    rm = spot.remove_ap()
    for ap in aps:
        rm.add_ap(ap)
    a2 = rm.strip(a2)

    tc.assertTrue(spot.are_equivalent(a1, a2))
