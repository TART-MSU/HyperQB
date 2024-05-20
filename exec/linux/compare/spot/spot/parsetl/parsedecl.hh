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

#include <spot/parsetl/parsetl.hh>
#include <spot/misc/location.hh>

# define YY_DECL \
  int tlyylex (tlyy::parser::semantic_type *yylval, \
               spot::location *yylloc,                                \
               spot::parse_error_list& error_list)
YY_DECL;

void flex_set_buffer(const std::string& buf, int start_tok, bool lenient);
void flex_unset_buffer();
