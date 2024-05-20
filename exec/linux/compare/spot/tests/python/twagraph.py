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
from buddy import bddtrue, bddfalse
from unittest import TestCase
tc = TestCase()

aut = spot.make_twa_graph(spot.make_bdd_dict())

try:
    print(aut.to_str())
    exit(2)
except RuntimeError as e:
    tc.assertIn("no state", str(e))

try:
    aut.set_init_state(2)
except ValueError as e:
    tc.assertIn("nonexisting", str(e))

try:
    aut.set_univ_init_state([2, 1])
except ValueError as e:
    tc.assertIn("nonexisting", str(e))

aut.new_states(3)
aut.set_init_state(2)
tc.assertEqual(aut.get_init_state_number(), 2)
aut.set_univ_init_state([2, 1])
tc.assertEqual([2, 1], list(aut.univ_dests(aut.get_init_state_number())))

try:
    aut.get_init_state()
except RuntimeError as e:
    s = str(e)
    tc.assertIn("abstract interface" in s and "alternating automata", s)

cpy = spot.make_twa_graph(aut, spot.twa_prop_set.all())
tc.assertEqual(aut.to_str(), cpy.to_str())
all = aut.set_buchi()
tc.assertNotEqual(aut.to_str(), cpy.to_str())
cpy = spot.make_twa_graph(aut, spot.twa_prop_set.all())
aut.new_acc_edge(0, 1, bddtrue, True)
tc.assertEqual(aut.num_edges(), 1 + cpy.num_edges())

aut.prop_universal(True)
aut.set_name("some name")
cpy = spot.make_twa_graph(aut, spot.twa_prop_set(False, False, False,
                                                 False, False, False))
tc.assertNotEqual(cpy.prop_universal(), aut.prop_universal())
tc.assertEqual(cpy.prop_universal(), spot.trival.maybe())
tc.assertEqual(cpy.get_name(), None)
cpy = spot.make_twa_graph(aut, spot.twa_prop_set(False, False, False,
                                                 False, False, False), True)
tc.assertEqual(cpy.get_name(), "some name")

from copy import copy
cpy = copy(aut)
tc.assertEqual(aut.to_str(), cpy.to_str())
cpy.set_init_state(1)
tc.assertEqual([2, 1], list(aut.univ_dests(aut.get_init_state_number())))
tc.assertEqual(cpy.get_init_state_number(), 1)
tc.assertEqual(cpy.get_name(), "some name")

try:
    s = aut.state_acc_sets(0)
except RuntimeError as e:
    tc.assertIn("state-based acceptance", str(e))

try:
    s = aut.state_is_accepting(0)
except RuntimeError as e:
    tc.assertIn("state-based acceptance", str(e))

aut.prop_state_acc(True)

tc.assertEqual(aut.state_acc_sets(0), all)
tc.assertEqual(aut.state_is_accepting(0), True)

aut.set_init_state(0)
aut.purge_unreachable_states()
i = aut.get_init_state()
tc.assertEqual(aut.state_is_accepting(i), True)

it = aut.succ_iter(i)
it.first()
tc.assertEqual(aut.edge_number(it), 1)
tc.assertEqual(aut.state_number(it.dst()), 1)
tc.assertEqual(aut.edge_storage(it).src, 0)
tc.assertEqual(aut.edge_storage(1).src, 0)
tc.assertEqual(aut.edge_data(it).cond, bddtrue)
tc.assertEqual(aut.edge_data(1).cond, bddtrue)
aut.release_iter(it)

aut.purge_dead_states()
i = aut.get_init_state()
tc.assertEqual(aut.state_is_accepting(i), False)

aut = spot.translate('FGa')
# Kill the edge between state 0 and 1
tc.assertEqual(aut.edge_storage(2).src, 0)
tc.assertEqual(aut.edge_storage(2).dst, 1)
aut.edge_data(2).cond = bddfalse
tc.assertEqual(aut.num_edges(), 3)
tc.assertEqual(aut.num_states(), 2)
aut.purge_dead_states()
tc.assertEqual(aut.num_edges(), 1)
tc.assertEqual(aut.num_states(), 1)
