// -*- coding: utf-8 -*-
// Copyright (C) by the Spot authors, see the AUTHORS file for details.
//
// This file is part of Spot, a model checking library.
//
// Spot is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// Spot is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <iosfwd>
#include <spot/twa/fwd.hh>
#include <spot/misc/common.hh>
#include <spot/twaalgos/aiger.hh>

namespace spot
{
  /// \ingroup twa_io
  /// \brief Print reachable states in dot format.
  ///
  /// If \a assume_sba is set, this assumes that the automaton
  /// is an SBA and use double ellipse to mark accepting states.
  ///
  /// \param options an optional string of letters, each indicating a
  /// different option.  Presently the following options are
  /// supported: 'v' for vertical output, 'h' for horizontal output,
  /// 't' force transition-based acceptance, 'N' hide the name of the
  /// automaton, 'n' shows the name, 'c' uses circle-shaped states,
  /// 'a' shows the acceptance, 'k' uses state-based labels if possible.
  SPOT_API std::ostream&
  print_dot(std::ostream& os,
                  const const_twa_ptr& g,
                  const char* options = nullptr);


  SPOT_API std::ostream &
  print_dot(std::ostream &os, aig_ptr circuit, const char * = nullptr);
}
