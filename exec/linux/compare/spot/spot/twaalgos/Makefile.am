## -*- coding: utf-8 -*-
## Copyright (C) by the Spot authors, see the AUTHORS file for details.
##
## This file is part of Spot, a model checking library.
##
## Spot is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or
## (at your option) any later version.
##
## Spot is distributed in the hope that it will be useful, but WITHOUT
## ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
## or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
## License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.

SUBDIRS = gtec

AM_CPPFLAGS = -I$(top_builddir) -I$(top_srcdir) $(BUDDY_CPPFLAGS) \
              -I$(top_builddir)/lib -I$(top_srcdir)/lib
AM_CXXFLAGS = $(WARNING_CXXFLAGS)

twaalgosdir = $(pkgincludedir)/twaalgos

twaalgos_HEADERS = \
  aiger.hh \
  alternation.hh \
  are_isomorphic.hh \
  bfssteps.hh \
  canonicalize.hh \
  cleanacc.hh \
  cobuchi.hh \
  complete.hh \
  complement.hh \
  compsusp.hh \
  contains.hh \
  copy.hh \
  cycles.hh \
  dbranch.hh \
  degen.hh \
  determinize.hh \
  dot.hh \
  dtbasat.hh \
  dtwasat.hh \
  dualize.hh \
  emptiness.hh \
  emptiness_stats.hh \
  forq_contains.hh \
  game.hh \
  genem.hh \
  gfguarantee.hh \
  gv04.hh \
  hoa.hh \
  iscolored.hh \
  isdet.hh \
  isunamb.hh \
  isweakscc.hh \
  langmap.hh \
  lbtt.hh \
  ltl2taa.hh \
  ltl2tgba_fm.hh \
  magic.hh \
  mask.hh \
  minimize.hh \
  mealy_machine.hh \
  couvreurnew.hh \
  neverclaim.hh \
  parity.hh \
  postproc.hh \
  powerset.hh \
  product.hh \
  randomgraph.hh \
  randomize.hh \
  reachiter.hh \
  relabel.hh \
  remfin.hh \
  remprop.hh \
  split.hh \
  strength.hh \
  sbacc.hh \
  sccfilter.hh \
  sccinfo.hh \
  se05.hh \
  sepsets.hh \
  simulation.hh \
  stats.hh \
  stripacc.hh \
  stutter.hh \
  sum.hh \
  synthesis.hh \
  tau03.hh \
  tau03opt.hh \
  toparity.hh \
  totgba.hh \
  toweak.hh \
  translate.hh \
  word.hh \
  zlktree.hh

noinst_LTLIBRARIES = libtwaalgos.la
libtwaalgos_la_SOURCES = \
  aiger.cc \
  alternation.cc \
  are_isomorphic.cc \
  bfssteps.cc \
  canonicalize.cc \
  cleanacc.cc \
  cobuchi.cc \
  complete.cc \
  complement.cc \
  compsusp.cc \
  contains.cc \
  cycles.cc \
  dbranch.cc \
  degen.cc \
  determinize.cc \
  dot.cc \
  dtbasat.cc \
  dtwasat.cc \
  dualize.cc \
  emptiness.cc \
  forq_contains.cc \
  genem.cc \
  gfguarantee.cc \
  gv04.cc \
  hoa.cc \
  iscolored.cc \
  isdet.cc \
  isunamb.cc \
  isweakscc.cc \
  game.cc \
  langmap.cc \
  lbtt.cc \
  ltl2taa.cc \
  ltl2tgba_fm.cc \
  magic.cc \
  mask.cc \
  minimize.cc \
  mealy_machine.cc \
  couvreurnew.cc \
  ndfs_result.hxx \
  neverclaim.cc \
  parity.cc \
  postproc.cc \
  powerset.cc \
  product.cc \
  randomgraph.cc \
  randomize.cc \
  reachiter.cc \
  remfin.cc \
  remprop.cc \
  relabel.cc \
  split.cc \
  strength.cc \
  sbacc.cc \
  sccinfo.cc \
  sccfilter.cc \
  se05.cc \
  sepsets.cc \
  simulation.cc \
  stats.cc \
  stripacc.cc \
  stutter.cc \
  synthesis.cc \
  sum.cc \
  tau03.cc \
  tau03opt.cc \
  toparity.cc \
  totgba.cc \
  toweak.cc \
  translate.cc \
  word.cc \
  zlktree.cc

libtwaalgos_la_LIBADD = gtec/libgtec.la
