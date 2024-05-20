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

#include <memory>
#include <spot/misc/common.hh>

namespace spot
{
  ////////////////////////////////////////////////////////////////////////
  // spins interface

  typedef struct transition_info
  {
    int* labels; // edge labels, NULL, or pointer to the edge label(s)
    int  group;  // holds transition group or -1 if unknown
  } transition_info_t;

  typedef void (*TransitionCB)(void *ctx,
                               transition_info_t *transition_info,
                               int *dst);

  /// \brief Implementation of the PINS interface. This class
  /// is a wrapper that, given a file, will compile it w.r.t
  /// the PINS interface. The class can then be manipulated
  /// transparently whatever the input format considered.
  class SPOT_API spins_interface
  {
  public:
    spins_interface() = default;
    spins_interface(const std::string& file_arg);
    ~spins_interface();

    // The various functions that can be called once the object
    // has been instanciated.
    void (*get_initial_state)(void *to);
    int (*have_property)();
    int (*get_successors)(void* m, int *in, TransitionCB, void *arg);
    int (*get_state_size)();
    const char* (*get_state_variable_name)(int var);
    int (*get_state_variable_type)(int var);
    int (*get_type_count)();
    const char* (*get_type_name)(int type);
    int (*get_type_value_count)(int type);
    const char* (*get_type_value_name)(int type, int value);

  private:
    // handle to the dynamic library. The variable is of type lt_dlhandle, but
    // we need this trick since we cannot put ltdl.h in public headers
    void* handle;
  };

  using spins_interface_ptr = std::shared_ptr<const spins_interface>;
}
