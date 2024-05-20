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

#include "common_file.hh"
#include <error.h>
#include <iostream>

output_file::output_file(const char* name, bool force_append)
{
  std::ios_base::openmode mode = std::ios_base::trunc;
  if (name[0] == '>' && name[1] == '>')
    {
      append_ = true;
      name += 2;
    }
  if (force_append)
    append_ = true;
  if (append_)
    mode = std::ios_base::app;
  if (name[0] == '-' && name[1] == 0)
    {
      os_ = &std::cout;
      return;
    }
  of_ = std::make_unique<std::ofstream>(name, mode);
  if (!*of_)
    error(2, errno, "cannot open '%s'", name);
  os_ = of_.get();
}

void
output_file::reopen_for_append(const std::string& name)
{
  if (os_ == &std::cout || of_->is_open())     // nothing to do
    return;
  const char* cname = name.c_str();
  if (cname[0] == '>' && cname[1] == '>')
    cname += 2;
  // the name cannot be "-" at this point, otherwise os_ would be
  // equal to std::cout.
  of_->open(cname, std::ios_base::app);
  if (!*of_)
    error(2, errno, "cannot reopen '%s'", cname);
}

void output_file::close(const std::string& name)
{
  // We close of_, not os_, so that we never close std::cout.
  if (os_)
    os_->flush();
  if (of_ && of_->is_open())
    of_->close();
  if (os_ && !*os_)
    error(2, 0, "error writing to %s",
          (name == "-") ? "standard output" : name.c_str());
}
