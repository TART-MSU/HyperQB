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


# Test some function that must return exceptions on error.  Doing
# so is mostly a way to improve the coverage report.


import spot
import buddy
from unittest import TestCase
tc = TestCase()


def report_missing_exception():
    raise RuntimeError("missing exception")


aut = spot.translate('GFa & GFb & GFc')
aut.set_acceptance(spot.acc_cond("parity min even 4"))
try:
    spot.iar(aut)
except RuntimeError as e:
    tc.assertIn('iar() expects Rabin-like or Streett-like input', str(e))
else:
    report_missing_exception()

alt = spot.dualize(spot.translate('FGa | FGb'))
try:
    spot.tgba_determinize(alt)
except RuntimeError as e:
    tc.assertIn('tgba_determinize() does not support alternation', str(e))
else:
    report_missing_exception()

aut = spot.translate('a U b U c')
aps = aut.ap()
rem = spot.remove_ap()
rem.add_ap('"a"=0,b')
aut = rem.strip(aut)
tc.assertEqual(aut.ap(), aps[2:])
try:
    rem.add_ap('"a=0,b')
except ValueError as e:
    tc.assertIn("""missing closing '"'""", str(e))
else:
    report_missing_exception()

try:
    rem.add_ap('a=0=b')
except ValueError as e:
    tc.assertIn("""unexpected '=' at position 3""", str(e))
else:
    report_missing_exception()

si = spot.scc_info(aut)
for meth in ('scc_has_rejecting_cycle', 'is_inherently_weak_scc',
             'is_weak_scc', 'is_complete_scc', 'is_terminal_scc'):
    try:
        getattr(spot, meth)(si, 20)
    except ValueError as e:
        tc.assertIn("invalid SCC number", str(e))
    else:
        report_missing_exception()


s1 = alt.new_state()
s2 = alt.new_state()
alt.new_edge(0, s1, buddy.bddtrue)
alt.new_edge(s1, s2, buddy.bddtrue, [0])
alt.new_edge(s2, s1, buddy.bddtrue)
alt.prop_inherently_weak(False)
alt.prop_state_acc(False)
si = spot.scc_info(alt)
try:
    si.determine_unknown_acceptance()
except RuntimeError as e:
    tc.assertIn("scc_info::determine_unknown_acceptance() does not supp",
                str(e))
else:
    report_missing_exception()

try:
    alt.set_init_state(999)
except ValueError as e:
    tc.assertIn("set_init_state()", str(e))
else:
    report_missing_exception()

alt.set_univ_init_state([s1, s2])
u = alt.get_init_state_number()
alt.set_init_state(u)

try:
    alt.set_init_state(u - 1)
except ValueError as e:
    tc.assertIn("set_init_state()", str(e))
else:
    report_missing_exception()


r = spot.twa_run(aut)
try:
    a = r.as_twa()
except RuntimeError as e:
    tc.assertIn("empty cycle", str(e))
else:
    report_missing_exception()

try:
    a = r.replay(spot.get_cout())
except RuntimeError as e:
    tc.assertIn("empty cycle", str(e))
else:
    report_missing_exception()

try:
    a = r.reduce()
except RuntimeError as e:
    tc.assertIn("empty cycle", str(e))
else:
    report_missing_exception()

a = spot.translate('Fa')
a = spot.to_generalized_rabin(a, False)
r = a.accepting_run()
r = r.reduce()
tc.assertEqual(r.cycle[0].acc, spot.mark_t([1]))
r.cycle[0].acc = spot.mark_t([0])
try:
    r.reduce();
except RuntimeError as e:
    tc.assertIn("expects an accepting cycle", str(e))
else:
    report_missing_exception()

f = spot.formula('GF(a | Gb)')
try:
    spot.gf_guarantee_to_ba(f, spot._bdd_dict)
except RuntimeError as e:
    tc.assertIn("guarantee", str(e))
else:
    report_missing_exception()

f = spot.formula('FG(a | Fb)')
try:
    spot.fg_safety_to_dca(f, spot._bdd_dict)
except RuntimeError as e:
    tc.assertIn("safety", str(e))
else:
    report_missing_exception()

n = spot.mark_t.max_accsets()
m = spot.mark_t([n - 1])
try:
    m = spot.mark_t([0]) << n
except RuntimeError as e:
    tc.assertIn("Too many acceptance sets", str(e))
else:
    report_missing_exception()

try:
    m.set(n)
except RuntimeError as e:
    tc.assertIn("bit index is out of bounds", str(e))
else:
    report_missing_exception()

try:
    m = spot.mark_t([0, n, 1])
except RuntimeError as e:
    tc.assertIn("Too many acceptance sets used.  The limit is", str(e))
else:
    report_missing_exception()

try:
    spot.complement_semidet(spot.translate('Gb R a', 'ba'))
except RuntimeError as e:
    tc.assertIn("requires a semi-deterministic input", str(e))
else:
    report_missing_exception()

try:
    spot.translate('F(G(a | !a) & ((b <-> c) W d))', 'det', 'any')
except ValueError as e:
    s = str(e)
    tc.assertIn('det', s)
    tc.assertIn('any', s)
else:
    report_missing_exception()

a1 = spot.translate('FGa')
a2 = spot.translate('Gb')
tc.assertFalse(spot.is_deterministic(a1))
tc.assertTrue(spot.is_deterministic(a2))
try:
    spot.product_xor(a1, a2)
except RuntimeError as e:
    tc.assertIn("product_xor() only works with deterministic automata", str(e))
else:
    report_missing_exception()
try:
    spot.product_xor(a2, a1)
except RuntimeError as e:
    tc.assertIn("product_xor() only works with deterministic automata", str(e))
else:
    report_missing_exception()
try:
    spot.product_xnor(a1, a2)
except RuntimeError as e:
    tc.assertIn("product_xnor() only works with deterministic automata", str(e))
else:
    report_missing_exception()
try:
    spot.product_xnor(a2, a1)
except RuntimeError as e:
    tc.assertIn("product_xnor() only works with deterministic automata", str(e))
else:
    report_missing_exception()

try:
    spot.solve_safety_game(a1)
except RuntimeError as e:
    tc.assertIn(
        "solve_safety_game(): arena should have true acceptance",
        str(e))
else:
    report_missing_exception()

try:
    spot.formula_Star(spot.formula("a"), 10, 333)
except OverflowError as e:
    tc.assertIn("333", str(e))
    tc.assertIn("254", str(e))
else:
    report_missing_exception()

try:
    spot.formula_FStar(spot.formula("a"), 333, 400)
except OverflowError as e:
    tc.assertIn("333", str(e))
    tc.assertIn("254", str(e))
else:
    report_missing_exception()

try:
    spot.formula_nested_unop_range(spot.op_F, spot.op_Or, 333, 400,
                                   spot.formula("a"))
except OverflowError as e:
    tc.assertIn("333", str(e))
    tc.assertIn("254", str(e))
else:
    report_missing_exception()

try:
    spot.formula_FStar(spot.formula("a"), 50, 40)
except OverflowError as e:
    tc.assertIn("reversed", str(e))
else:
    report_missing_exception()


a = spot.translate("a")
b = spot.translate("b")
spot.set_synthesis_outputs(a, b.ap_vars())
try:
    a.to_str()
except RuntimeError as e:
    se = str(e)
    tc.assertIn("synthesis-outputs", se)
    tc.assertIn("unregistered proposition", se)
else:
    report_missing_exception()


a = spot.make_twa_graph()
s = a.new_state()
b = spot.formula_to_bdd("a & b", a.get_dict(), a)
a.new_edge(s, s, b, [])
try:
    print(a.to_str('hoa'))
except RuntimeError as e:
    tc.assertIn("unregistered atomic propositions", str(e))
else:
    report_missing_exception()

a.register_aps_from_dict()
tc.assertEqual(a.to_str('hoa'), """HOA: v1
States: 1
Start: 0
AP: 2 "a" "b"
acc-name: all
Acceptance: 0 t
properties: trans-labels explicit-labels state-acc deterministic
--BODY--
State: 0
[0&1] 0
--END--""")

try:
    a.register_aps_from_dict()
except RuntimeError as e:
    se = str(e)
    tc.assertIn("register_aps_from_dict", se)
    tc.assertIn("already registered", se)
else:
    report_missing_exception()


try:
    spot.minimize_mealy(a, 100)
except RuntimeError as e:
    se = str(e)
    tc.assertIn("minimize_mealy", se)
    tc.assertIn("minimize_lvl", se)
else:
    report_missing_exception()

opt = spot.synthesis_info()
opt.minimize_lvl = 3
try:
    spot.minimize_mealy(a, opt)
except RuntimeError as e:
    se = str(e)
    tc.assertIn("minimize_mealy", se)
    tc.assertIn("synthesis-output", se)

spot.set_synthesis_outputs(a, buddy.bdd_ithvar(a.register_ap("b")))
filename = "/THIS-FILE/SHOULD/NOT/EXIST"
opt.opt.set_str("satlogdimacs", filename)
try:
    spot.minimize_mealy(a, opt)
except RuntimeError as e:
    tc.assertIn(filename, str(e))
else:
    report_missing_exception()

opt.opt.set_str("satlogdimacs", "")
opt.opt.set_str("satlogcsv", filename)
try:
    spot.minimize_mealy(a, opt)
except RuntimeError as e:
    tc.assertIn(filename, str(e))
else:
    report_missing_exception()


# Relabeling must use new variables
aut = spot.make_twa_graph()
aut.new_states(2)
ap = buddy.bdd_ithvar(aut.register_ap("__nv0"))
aut.new_edge(0,1,ap)

try:
    spot.partitioned_relabel_here(aut)
except RuntimeError as e:
    tc.assertIn("The given prefix for new variables",
                str(e))
else:
    report_missing_exception()

# Relabeling games must not use the
# globally reserved aps
aut = spot.make_twa_graph()
aut.new_states(2)
apin = buddy.bdd_ithvar(aut.register_ap("__AP_IN__"))
apout = buddy.bdd_ithvar(aut.register_ap("__AP_OUT__"))
aut.new_edge(0,1,apin & apout)
aut.new_edge(1,0,buddy.bdd_not(apin & apout))
spot.set_state_players(aut, [False, True])

try:
    spot.partitioned_game_relabel_here(aut, True, True)
except RuntimeError as e:
    tc.assertIn("You can not use __AP_IN__ or __AP_OUT__",
                str(e))
else:
    report_missing_exception()
