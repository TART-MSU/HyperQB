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

a = spot.acc_cond('parity min odd 5')
tc.assertEqual(str(a.fin_unit_one_split()),
               f'(0, {a!r}, spot.acc_cond(5, "f"))')

a.set_acceptance('Rabin 3')
tc.assertEqual(str(a.fin_unit_one_split()),
               '(0, spot.acc_cond(5, "Inf(1)"), '
               'spot.acc_cond(5, "(Fin(2) & Inf(3)) | (Fin(4) & Inf(5))"))')

a.set_acceptance('(Fin(0)|Inf(3))&(Fin(1)|Inf(4))&(Fin(2)|Inf(5)) |\
(Fin(0)|Inf(4))&(Fin(1)|Inf(5))&(Fin(2)|Inf(3)) |\
(Fin(0)|Inf(5))&(Fin(1)|Inf(3))&(Fin(2)|Inf(4))')

tc.maxDiff = None
tc.assertEqual(str(a.fin_unit_one_split()),
               '(0, spot.acc_cond(5, '
               '"((Fin(1) | Inf(4)) & (Fin(2) | Inf(5))) | '
               '((Fin(1) | Inf(5)) & (Fin(2) | Inf(3))) | '
               '((Fin(1) | Inf(3)) & (Fin(2) | Inf(4)))"), '
               'spot.acc_cond(5, '
               '"(Inf(3) & (Fin(1) | Inf(4)) & (Fin(2) | Inf(5))) | '
               '(Inf(4) & (Fin(1) | Inf(5)) & (Fin(2) | Inf(3))) | '
               '(Inf(5) & (Fin(1) | Inf(3)) & (Fin(2) | Inf(4)))"))')

a = a.remove([4], True)
tc.assertEqual(str(a.fin_unit_one_split()),
               '(1, spot.acc_cond(5, '
               '"(Fin(0) | Inf(3)) & (Fin(2) | Inf(5))"), '
               'spot.acc_cond(5, '
               '"(Fin(0) & (Fin(1) | Inf(5)) & (Fin(2) | Inf(3))) | '
               '((Fin(0) | Inf(5)) & (Fin(1) | Inf(3)) & Fin(2))"))')

def report_missing_exception():
    raise RuntimeError("missing exception")

a.set_acceptance("Inf(0)")
try:
    a.fin_unit_one_split()
except RuntimeError as e:
    tc.assertIn('no Fin', str(e))
else:
    report_missing_exception()
