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
from itertools import zip_longest
from buddy import bddfalse
from unittest import TestCase
tc = TestCase()

# Tests for the new version of to_parity

# It is no more a no_option as we now have more options (like iar, bscc, …)
no_option = spot.to_parity_options()
no_option.search_ex = False
no_option.use_last = False
no_option.use_last_post_process = False
no_option.partial_degen = False
no_option.acc_clean = False
no_option.parity_equiv = False
no_option.tar = False
no_option.iar = True
no_option.lar_dfs = True
no_option.bscc = True
no_option.parity_prefix = False
no_option.parity_prefix_general = False
no_option.generic_emptiness = False
no_option.rabin_to_buchi = False
no_option.buchi_type_to_buchi = False
no_option.parity_type_to_parity = False
no_option.reduce_col_deg = False
no_option.propagate_col = False
no_option.use_generalized_rabin = False

acc_clean_search_opt = spot.to_parity_options()
acc_clean_search_opt.search_ex = False
acc_clean_search_opt.use_last = False
acc_clean_search_opt.use_last_post_process = False
acc_clean_search_opt.force_order = False
acc_clean_search_opt.partial_degen = False
acc_clean_search_opt.acc_clean = True
acc_clean_search_opt.parity_equiv = False
acc_clean_search_opt.tar = False
acc_clean_search_opt.iar = True
acc_clean_search_opt.lar_dfs = True
acc_clean_search_opt.bscc = True
acc_clean_search_opt.parity_prefix = False
acc_clean_search_opt.parity_prefix_general = False
acc_clean_search_opt.generic_emptiness = False
acc_clean_search_opt.rabin_to_buchi = False
acc_clean_search_opt.buchi_type_to_buchi = False
acc_clean_search_opt.parity_type_to_parity = False
acc_clean_search_opt.reduce_col_deg = False
acc_clean_search_opt.propagate_col = False
acc_clean_search_opt.use_generalized_rabin = False

partial_degen_opt = spot.to_parity_options()
partial_degen_opt.search_ex = False
partial_degen_opt.use_last = False
partial_degen_opt.use_last_post_process = False
partial_degen_opt.force_order = False
partial_degen_opt.partial_degen = True
partial_degen_opt.acc_clean = False
partial_degen_opt.parity_equiv = False
partial_degen_opt.tar = False
partial_degen_opt.iar = True
partial_degen_opt.lar_dfs = True
partial_degen_opt.bscc = True
partial_degen_opt.parity_prefix = False
partial_degen_opt.parity_prefix_general = False
partial_degen_opt.generic_emptiness = False
partial_degen_opt.rabin_to_buchi = False
partial_degen_opt.buchi_type_to_buchi = False
partial_degen_opt.parity_type_to_parity = False
partial_degen_opt.reduce_col_deg = False
partial_degen_opt.propagate_col = False
partial_degen_opt.use_generalized_rabin = False

parity_equiv_opt = spot.to_parity_options()
parity_equiv_opt.search_ex = False
parity_equiv_opt.use_last = False
parity_equiv_opt.use_last_post_process = False
parity_equiv_opt.partial_degen = False
parity_equiv_opt.acc_clean = False
parity_equiv_opt.parity_equiv = True
parity_equiv_opt.tar = False
parity_equiv_opt.iar = True
parity_equiv_opt.lar_dfs = True
parity_equiv_opt.bscc = True
parity_equiv_opt.parity_prefix = False
parity_equiv_opt.parity_prefix_general = False
parity_equiv_opt.generic_emptiness = False
parity_equiv_opt.rabin_to_buchi = False
parity_equiv_opt.buchi_type_to_buchi = False
parity_equiv_opt.parity_type_to_parity = False
parity_equiv_opt.reduce_col_deg = False
parity_equiv_opt.propagate_col = False
parity_equiv_opt.use_generalized_rabin = False

rab_to_buchi_opt = spot.to_parity_options()
rab_to_buchi_opt.search_ex = False
rab_to_buchi_opt.use_last = False
rab_to_buchi_opt.use_last_post_process = False
rab_to_buchi_opt.partial_degen = False
rab_to_buchi_opt.acc_clean = False
rab_to_buchi_opt.parity_equiv = True
rab_to_buchi_opt.tar = False
rab_to_buchi_opt.iar = True
rab_to_buchi_opt.lar_dfs = False
rab_to_buchi_opt.bscc = False
rab_to_buchi_opt.parity_prefix = False
rab_to_buchi_opt.parity_prefix_general = False
rab_to_buchi_opt.generic_emptiness = False
rab_to_buchi_opt.rabin_to_buchi = True
rab_to_buchi_opt.buchi_type_to_buchi = False
rab_to_buchi_opt.parity_type_to_parity = False
rab_to_buchi_opt.reduce_col_deg = False
rab_to_buchi_opt.propagate_col = False
rab_to_buchi_opt.use_generalized_rabin = False

# Force to use CAR, IAR or TAR for each SCC
use_car_opt = spot.to_parity_options()
use_car_opt.search_ex = True
use_car_opt.use_last = True
use_car_opt.use_last_post_process = True
use_car_opt.partial_degen = False
use_car_opt.acc_clean = False
use_car_opt.parity_equiv = False
use_car_opt.tar = True
use_car_opt.iar = True
use_car_opt.lar_dfs = True
use_car_opt.bscc = True
use_car_opt.parity_prefix = False
use_car_opt.parity_prefix_general = False
use_car_opt.generic_emptiness = False
use_car_opt.rabin_to_buchi = False
use_car_opt.buchi_type_to_buchi = False
use_car_opt.parity_type_to_parity = False
use_car_opt.reduce_col_deg = False
use_car_opt.propagate_col = False
use_car_opt.use_generalized_rabin = False

default_opt = spot.to_parity_options()

all_opt = spot.to_parity_options()
all_opt.search_ex = True
all_opt.use_last = True
all_opt.use_last_post_process = True
all_opt.partial_degen = True
all_opt.acc_clean = True
all_opt.parity_equiv = True
all_opt.tar = True
all_opt.iar = True
all_opt.lar_dfs = True
all_opt.bscc = True
all_opt.parity_prefix = True
all_opt.parity_prefix_general = True
all_opt.generic_emptiness = True
all_opt.rabin_to_buchi = True
all_opt.buchi_type_to_buchi = True
all_opt.parity_type_to_parity = True
all_opt.reduce_col_deg = True
all_opt.propagate_col = True
all_opt.use_generalized_rabin = True
all_opt.pretty_print = True

options = [
    no_option,
    acc_clean_search_opt,
    partial_degen_opt,
    parity_equiv_opt,
    rab_to_buchi_opt,
    use_car_opt,
    default_opt,
    all_opt,
    None, # acd_transform
]


def test(aut, expected_num_states=[], full=True):
    for (opt, expected_num) in zip_longest(options, expected_num_states):
        if type(expected_num) is str and expected_num == 'skip':
            continue
        if opt is not None:
            p1 = spot.to_parity(aut,
                                search_ex = opt.search_ex,
                                use_last = opt.use_last,
                                use_last_post_process = \
                                    opt.use_last_post_process,
                                force_order = opt.force_order,
                                partial_degen = opt.partial_degen,
                                acc_clean = opt.acc_clean,
                                parity_equiv = opt.parity_equiv,
                                tar = opt.tar,
                                iar = opt.iar,
                                lar_dfs = opt.lar_dfs,
                                bscc = opt.bscc,
                                parity_prefix = opt.parity_prefix,
                                parity_prefix_general = \
                                    opt.parity_prefix_general,
                                generic_emptiness = opt.generic_emptiness,
                                rabin_to_buchi = opt.rabin_to_buchi,
                                buchi_type_to_buchi = opt.buchi_type_to_buchi,
                                parity_type_to_parity = \
                                    opt.parity_type_to_parity,
                                reduce_col_deg = opt.reduce_col_deg,
                                propagate_col = opt.propagate_col,
                                use_generalized_rabin = \
                                    opt.use_generalized_rabin
                                )
        else:
            p1 = spot.acd_transform(aut)
        p1st, p1ed, p1se = p1.num_states(), p1.num_edges(), p1.num_sets()
        if opt is not None and opt.parity_prefix is False:
            # Reduce the number of colors to help are_equivalent
            spot.reduce_parity_here(p1)
        tc.assertTrue(spot.are_equivalent(aut, p1))
        if expected_num is not None:
            tc.assertEqual(p1.num_states(), expected_num)
        if full and opt is not None:
            # Make sure passing opt is the same as setting
            # each argument individually
            p2 = spot.to_parity(aut, opt)
            tc.assertEqual(p2.num_states(), p1st)
            tc.assertEqual(p2.num_edges(), p1ed)
            tc.assertEqual(p2.num_sets(), p1se)

test(spot.automaton("""HOA: v1
name: "(FGp0 & ((XFp0 & F!p1) | F(Gp1 & XG!p0))) | G(F!p0 & (XFp0 | F!p1) &
F(Gp1 | G!p0))"
States: 14
Start: 0
AP: 2 "p1" "p0"
Acceptance: 6 (Fin(0) & Fin(1)) | ((Fin(4)|Fin(5)) & (Inf(2)&Inf(3)))
properties: trans-labels explicit-labels trans-acc complete
properties: deterministic
--BODY--
State: 0
[!0] 1
[0] 2
State: 1
[!0&!1] 1 {0 1 2 3 5}
[0&!1] 3
[!0&1] 4
[0&1] 5
State: 2
[0&!1] 2 {1}
[!0&1] 4
[!0&!1] 6
[0&1] 7
State: 3
[0&!1] 3 {1 3}
[!0&1] 4
[!0&!1] 6 {0 1 2 3 5}
[0&1] 8
State: 4
[!0&!1] 4 {1 2 3 5}
[!0&1] 4 {2 4 5}
[0&!1] 5 {1 3}
[0&1] 5 {4}
State: 5
[!0&1] 4 {2 4 5}
[0&!1] 5 {1 3}
[0&1] 8 {2 4}
[!0&!1] 9 {1 2 3 5}
State: 6
[0&!1] 3 {1 3}
[!0&1] 4
[0&1] 5
[!0&!1] 10
State: 7
[!0&1] 4
[0&!1] 7 {1 3}
[!0&!1] 11
[0&1] 12 {0 4}
State: 8
[!0&1] 4 {2 4 5}
[0&1] 5 {4}
[0&!1] 8 {1 3}
[!0&!1] 11 {1 3 5}
State: 9
[!0&1] 4 {2 4 5}
[0&!1] 5 {1 3}
[0&1] 5 {4}
[!0&!1] 11 {1 3 5}
State: 10
[!0&1] 4
[0&1] 8
[!0&!1] 10 {0 1 2 3 5}
[0&!1] 13 {1 2 3}
State: 11
[!0&1] 4 {2 4 5}
[0&!1] 8 {1 2 3}
[0&1] 8 {2 4}
[!0&!1] 11 {1 2 3 5}
State: 12
[!0&1] 4
[0&1] 7 {0 2 4}
[!0&!1] 9
[0&!1] 12 {1 3}
State: 13
[!0&1] 4
[0&1] 5
[!0&!1] 10 {0 1 3 5}
[0&!1] 13 {1 3}
--END--"""), [30, 32, 23, 30, 33, 45, 22, 22, 21])

test(spot.automaton("""
HOA: v1
States: 2
Start: 0
AP: 2 "p0" "p1"
Acceptance: 5 (Inf(0)&Inf(1)) | ((Fin(2)|Fin(3)) & Fin(4))
--BODY--
State: 0
[!0 & 1] 0 {2 3}
[!0 & !1] 0 {3}
[0] 1
State: 1
[0&1] 1 {1 2 4}
[0&!1] 1 {4}
[!0&1] 1 {0 1 2 3}
[!0&!1] 1 {0 3}
--END--"""), [5, 6, 3, 5, 5, 26, 3, 3, 3])

test(spot.automaton("""HOA: v1
States: 2
Start: 0
AP: 2 "p0" "p1"
Acceptance: 5 ((Fin(1)|Fin(3)|Fin(4)) | Inf(2) | Inf(0))
               & (Inf(0) | Inf(1)) & (Inf(2) | Inf(1))
properties: trans-labels explicit-labels trans-acc deterministic
--BODY--
State: 0
[0&1] 0 {1 3}
[!0&1] 1 {0}
State: 1
[0&1] 1 {2 3 4}
[!0&!1] 0 {1 2}
--END--"""), [9, 3, 2, 9, 9, 10, 2, 2, 2])

test(spot.automaton("""
HOA: v1
States: 4
Start: 0
AP: 2 "p0" "p1"
Acceptance: 6
((Fin(1) | Inf(2)) & Inf(5)) | (Fin(0) & (Fin(1) | (Fin(3) & Inf(4))))
properties: trans-labels explicit-labels trans-acc complete
properties: deterministic
--BODY--
State: 0
[!0&!1] 0 {2}
[0&1] 1 {0 5}
[0&!1] 1 {0 2 5}
[!0&1] 2 {1}
State: 1
[0&1] 1 {0}
[!0&!1] 1 {2}
[0&!1] 1 {0 2}
[!0&1] 2 {1}
State: 2
[!0&!1] 0 {2 3}
[0&!1] 0 {0 2 3 5}
[!0&1] 2 {1 4}
[0&1] 3 {0 5}
State: 3
[!0&!1] 0 {2 3}
[0&!1] 0 {0 2 3 5}
[!0&1] 2 {1 4}
[0&1] 3 {0}
--END--
"""), [23, 104, 80, 23, 27, 17, "skip", "skip", 5])

test(spot.automaton("""
HOA: v1
States: 5
Start: 0
AP: 2 "p1" "p0"
Acceptance: 5 (Fin(0) & Fin(1)) | (Fin(3) & (Inf(2)&Inf(4)))
properties: trans-labels explicit-labels trans-acc complete
properties: deterministic
--BODY--
State: 0
[!0] 1
[0] 2
State: 1
[!0&!1] 1 {0 1}
[!0&1] 3
[0] 4
State: 2
[!0&1] 1
[0&!1] 2
[0&1] 2 {0 1 2 4}
[!0&!1] 3
State: 3
[!0&1] 3 {1 2 3}
[!0&!1] 3 {4}
[0&!1] 4 {3}
[0&1] 4 {1 2 3}
State: 4
[!0&!1] 3 {3}
[!0&1] 3 {1 2 3}
[0&!1] 4
[0&1] 4 {1 2 4}
--END--
"""), [9, 6, 7, 9, 9, 10, 6, 6, 6])

test(spot.automaton("""
HOA: v1
States: 2
Start: 0
AP: 2 "p1" "p0"
Acceptance: 5 (Fin(0) & (Fin(3)|Fin(4)) & (Inf(1)&Inf(2))) | Inf(3)
properties: trans-labels explicit-labels trans-acc complete
properties: deterministic stutter-invariant
--BODY--
State: 0
[0&!1] 0 {2 3}
[!0&!1] 0 {2 3 4}
[!0&1] 1
[0&1] 1 {2 4}
State: 1
[!0&!1] 0 {0 2 3 4}
[!0&1] 1 {1}
[0&!1] 1 {2 3}
[0&1] 1 {1 2 4}
--END--
"""), [6, 3, 2, 6, 6, 6, 2, 2, 2])


# Tests both the old and new version of to_parity
a = spot.automaton("""HOA: v1
States: 1
Start: 0
AP: 2 "a" "b"
Acceptance: 2 Inf(0)|Inf(1)
--BODY--
State: 0
[0&1] 0 {0 1}
[0&!1] 0 {0}
[!0&1] 0 {1}
[!0&!1] 0
--END--""")
p = spot.to_parity_old(a, True)
tc.assertTrue(spot.are_equivalent(a, p))
test(a)

a = spot.automaton("""
HOA: v1 States: 6 Start: 0 AP: 2 "p0" "p1" Acceptance: 6 Inf(5) |
Fin(2) | Inf(1) | (Inf(0) & Fin(3)) | Inf(4) properties: trans-labels
explicit-labels trans-acc --BODY-- State: 0 [0&1] 2 {4 5} [0&1] 4 {0 4}
[!0&!1] 3 {3 5} State: 1 [0&!1] 3 {1 5} [!0&!1] 5 {0 1} State: 2 [!0&!1]
0 {0 3} [0&!1] 1 {0} State: 3 [!0&1] 4 {1 2 3} [0&1] 3 {3 4 5} State:
4 [!0&!1] 1 {2 4} State: 5 [!0&1] 4 --END--
""")
p = spot.to_parity_old(a, True)
tc.assertEqual(p.num_states(), 22)
tc.assertTrue(spot.are_equivalent(a, p))
test(a, [8, 6, 7, 8, 6, 7, 6, 6, 6])

# Force a few edges to false, to make sure to_parity() is OK with that.
for e in a.out(2):
    if e.dst == 1:
        e.cond = bddfalse
        break
for e in a.out(3):
    if e.dst == 3:
        e.cond = bddfalse
        break
p = spot.to_parity_old(a, True)
tc.assertEqual(p.num_states(), 22)
tc.assertTrue(spot.are_equivalent(a, p))
test(a, [8, 6, 7, 8, 6, 7, 6, 6, 6])

for f in spot.randltl(4, 400):
    d = spot.translate(f, "det", "G")
    p = spot.to_parity_old(d, True)
    tc.assertTrue(spot.are_equivalent(p, d))

for f in spot.randltl(5, 2000):
    n = spot.translate(f)
    p = spot.to_parity_old(n, True)
    tc.assertTrue(spot.are_equivalent(n, p))

for i,f in enumerate(spot.randltl(10, 200)):
    test(spot.translate(f, "det", "G"), full=(i<50))

for f in spot.randltl(5, 500):
    test(spot.translate(f), full=False)

# Issue #390.
a = spot.translate('!(GFa -> (GFb & GF(!b & !Xb)))', 'gen', 'det')
b = spot.to_parity_old(a, True)
tc.assertTrue(a.equivalent_to(b))
test(a, [8, 7, 3, 8, 8, 7, 3, 3, 3])

# owl-21.0 ltl2dra -f '(GFa -> GFb) & GF(c <-> Xc)' | autfilt -Hi | fmt
a = spot.automaton("""
HOA: v1 name: "Automaton for ((((F(G(!a))) | (G(F(b))))) & (G(F(((c) <->
(X(c)))))))" States: 11 Start: 0 AP: 3 "a" "b" "c" acc-name: Rabin 3
Acceptance: 6 (Fin(0) & Inf(1)) | (Fin(2) & Inf(3)) | (Fin(4) & Inf(5))
properties: implicit-labels trans-acc complete deterministic --BODY--
State: 0 0 {3} 0 {2 4} 3 {3} 3 {2 4} 1 0 {2 4} 2 4 {2 4} State: 1 0 0 {2
4} 3 3 {2 4} 1 {5} 0 {2 4} 2 {5} 4 {2 4} State: 2 3 3 {2 4} 3 3 {2 4}
6 {1 5} 5 {1 2 4} 2 {1 5} 4 {1 2 4} State: 3 7 {1 3} 7 {1 2 4} 3 {1 3}
3 {1 2 4} 2 4 {2 4} 2 4 {2 4} State: 4 3 {3} 3 {2 4} 3 {3} 3 {2 4} 6 {1}
5 {1 2 4} 2 {1} 4 {1 2 4} State: 5 8 {3} 8 {2 4} 3 {3} 3 {2 4} 6 5 {2
4} 2 4 {2 4} State: 6 8 8 {2 4} 3 3 {2 4} 6 {5} 5 {2 4} 2 {5} 4 {2 4}
State: 7 7 {3} 7 {2 4} 3 {3} 3 {2 4} 9 10 {2 4} 2 4 {2 4} State: 8 0 {1
3} 0 {1 2 4} 3 {1 3} 3 {1 2 4} 6 5 {2 4} 2 4 {2 4} State: 9 7 7 {2 4}
3 3 {2 4} 1 {1 5} 0 {1 2 4} 2 {1 5} 4 {1 2 4} State: 10 7 {3} 7 {2 4}
3 {3} 3 {2 4} 1 {1} 0 {1 2 4} 2 {1} 4 {1 2 4} --END--
""")
b = spot.iar_maybe(a)
tc.assertEqual(b.num_states(), 11)
tc.assertTrue(a.equivalent_to(b))
test(a, [11, 11, 11, 11, 11, 11, 11, 11, 11])

a = spot.automaton("""
HOA: v1 States: 10 Start: 0 AP: 2 "p0" "p1" acc-name: Rabin 4 Acceptance:
8 (Fin(0) & Inf(1)) | (Fin(2) & Inf(3)) | (Fin(4) & Inf(5)) | (Fin(6)
& Inf(7)) properties: implicit-labels trans-acc complete deterministic
--BODY-- State: 0 2 {7} 7 {3} 2 {7} 3 State: 1 5 {0 3} 9 {3 4} 5 {0 3}
9 {3 4} State: 2 9 {1 6} 9 {1 6} 9 {1 6} 9 {1 6} State: 3 3 {4} 9 {0}
1 {4} 4 {5 6} State: 4 7 8 {1 5 7} 9 {3 7} 8 {1 5 7} State: 5 6 {4} 9
{1 2 6} 6 {4} 9 {1 2 6} State: 6 1 {3 7} 1 {3 7} 1 {3 7} 1 {3 7} State:
7 1 {3 6} 8 {2} 1 {3 6} 8 {2} State: 8 8 {3 4 7} 3 {2} 8 {3 4 7} 3 {2}
State: 9 3 {4} 2 3 {4} 6 --END--
""")
b = spot.iar_maybe(a)
tc.assertEqual(b.num_states(), 87)
tc.assertTrue(a.equivalent_to(b))
test(a, [87, 91, 91, 87, 87, 87, 51, 35, 21])

a = spot.automaton("""HOA: v1
States: 4
Start: 0
AP: 2 "p0" "p1"
Acceptance: 2 Fin(1) & Fin(0)
properties: trans-labels explicit-labels state-acc
--BODY--
State: 0
[!0&!1] 2
[!0&!1] 1
State: 1
[!0&1] 0
[0&1] 3
State: 2
[0&!1] 1
State: 3 {0}
[!0&1] 3
[!0&!1] 1
--END--""")
b = spot.parity_type_to_parity(a)
tc.assertTrue(spot.are_equivalent(a, b))
