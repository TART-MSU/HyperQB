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

AUTOMAKE_OPTIONS = subdir-objects

nobase_pkginclude_HEADERS = bricks/brick-assert bricks/brick-bitlevel \
	bricks/brick-hash bricks/brick-hashset  bricks/brick-shmem \
	bricks/brick-types


# List directories in the order they must be built.  Keep tests at the
# end, after building '.' (since the current directory contains
# libspot.la needed by the tests)
SUBDIRS = misc priv tl graph twa twacube twaalgos ta taalgos kripke \
          twacube_algos mc parseaut parsetl . ltsmin gen

lib_LTLIBRARIES = libspot.la
libspot_la_SOURCES =
libspot_la_LDFLAGS = $(BUDDY_LDFLAGS) -no-undefined @LIBSPOT_PTHREAD@ $(SYMBOLIC_LDFLAGS)
libspot_la_LIBADD =  \
  kripke/libkripke.la \
  misc/libmisc.la \
  parseaut/libparseaut.la \
  parsetl/libparsetl.la \
  priv/libpriv.la \
  taalgos/libtaalgos.la \
  ta/libta.la \
  tl/libtl.la \
  twaalgos/libtwaalgos.la \
  twa/libtwa.la \
  twacube_algos/libtwacube_algos.la \
  twacube/libtwacube.la \
  mc/libmc.la \
  ../lib/libgnu.la \
  ../picosat/libpico.la

# Dummy C++ source to cause C++ linking.
nodist_EXTRA_libspot_la_SOURCES = _.cc
_.cc:; touch $@


EXTRA_DIST = $(srcdir)/libspot.pc.in
pkgconfigdir = $(libdir)/pkgconfig
nodist_pkgconfig_DATA = libspot.pc

libspot.pc: $(srcdir)/libspot.pc.in Makefile
	sed -e 's![@]prefix[@]!$(prefix)!g' \
	    -e 's![@]exec_prefix[@]!$(exec_prefix)!g' \
	    -e 's![@]includedir[@]!$(includedir)!g' \
	    -e 's![@]libdir[@]!$(libdir)!g' \
	    -e 's![@]PACKAGE_VERSION[@]!$(PACKAGE_VERSION)!g' \
	    -e 's![@]LIBSPOT_PTHREAD[@]!$(LIBSPOT_PTHREAD)!g' \
	$(srcdir)/libspot.pc.in > $@.tmp && mv $@.tmp $@

CLEANFILES = libspot.pc
