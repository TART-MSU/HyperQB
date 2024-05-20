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

#include "config.h"
#include <spot/ltsmin/spins_interface.hh>
#include <ltdl.h>
#include <cstring>
#include <cstdlib>
#include <string.h>
#include <spot/mc/utils.hh>
#include <sys/stat.h>

// MinGW does not define this.
#ifndef WEXITSTATUS
# define WEXITSTATUS(x) ((x) & 0xff)
#endif

namespace spot
{
  namespace
  {
    using namespace std::string_literals;

    //////////////////////////////////////////////////////////////////////////
    // LOADER

    // Call spins to compile "foo.prom" as "foo.prom.spins" if the latter
    // does not exist already or is older.
    static void
    compile_model(std::string& filename, const std::string& ext)
    {
      const char* cmd;
      std::string command;
      std::string compiled_ext;

      if (ext == ".gal")
        {
          cmd = "gal2c ";
          command = cmd + filename;
          compiled_ext = "2C";
        }
      else if (ext == ".prom" || ext == ".pm" || ext == ".pml")
        {
          cmd = "spins ";
          command = cmd + filename;
          compiled_ext = ".spins";
        }
      else if (ext == ".dve")
        {
          cmd = "divine ";
          command = cmd + "compile --ltsmin "s + filename;
          command += " 2> /dev/null"; // FIXME needed for Clang on MacOSX
          compiled_ext = "2C";
        }
      else
        {
          throw std::runtime_error("Unknown extension '"s + ext +
                                   "'.  Use '.prom', '.pm', '.pml', "
                                   "'.dve', '.dve2C', '.gal', '.gal2C' or "
                                   "'.prom.spins'.");
        }

      struct stat s;
      if (stat(filename.c_str(), &s) != 0)
        throw std::runtime_error("Cannot open "s + filename);

      filename += compiled_ext;

      // Remove any directory, because the new file will
      // be compiled in the current directory.
      size_t pos = filename.find_last_of("/\\");
      if (pos != std::string::npos)
        filename = "./" + filename.substr(pos + 1);

      struct stat d;
      if (stat(filename.c_str(), &d) == 0)
        if (s.st_mtime < d.st_mtime)
          // The .spins or .dve2C or .gal2C is up-to-date, no need to compile.
          return;

      int res = system(command.c_str());
      if (res)
        {
          std::ostringstream os;
          int status = WEXITSTATUS(res);
          os << "Execution of '" << command << "' returned exit code "
             << status << '.';
          if (status == 127)
            os << "\nIs " << cmd << "installed and on your $PATH?\n"
              "Read README.ltsmin in Spot's sources for "
              "installation instructions.";
          throw std::runtime_error(os.str());
        }
    }
  }

  spins_interface::spins_interface(const std::string& file_arg)
  {
    std::string file;
    if (file_arg.find_first_of("/\\") != std::string::npos)
      file = file_arg;
    else
      file = "./" + file_arg;

    std::string ext = file.substr(file.find_last_of("."));
    if (ext != ".spins" && ext != ".dve2C" && ext != ".gal2C")
      {
        compile_model(file, ext);
        ext = file.substr(file.find_last_of("."));
      }

    if (lt_dlinit())
      throw std::runtime_error("Failed to initialize libltldl.");

    lt_dlhandle h = lt_dlopen(file.c_str());
    if (!h)
      {
        std::string lt_error = lt_dlerror();
        lt_dlexit();
        throw std::runtime_error("Failed to load '"s
                                 + file + "'.\n" + lt_error);
      }

    handle = h;

    auto sym = [&](auto* dst, const char* name)
      {
        // Work around -Wpendantic complaining that pointer-to-objects
        // should not be converted to pointer-to-functions (we have to
        // assume they can for POSIX).
        *reinterpret_cast<void**>(dst) = lt_dlsym(h, name);
        if (*dst == nullptr)
          throw std::runtime_error("Failed to resolve symbol '"s
                                   + name + "' in '" + file + "'.");
      };

    // SpinS interface.
    if (ext == ".spins")
      {
        sym(&get_initial_state, "spins_get_initial_state");
        have_property = nullptr;
        sym(&get_successors, "spins_get_successor_all");
        sym(&get_state_size, "spins_get_state_size");
        sym(&get_state_variable_name, "spins_get_state_variable_name");
        sym(&get_state_variable_type, "spins_get_state_variable_type");
        sym(&get_type_count, "spins_get_type_count");
        sym(&get_type_name, "spins_get_type_name");
        sym(&get_type_value_count, "spins_get_type_value_count");
        sym(&get_type_value_name, "spins_get_type_value_name");
      }
    // dve2 and gal2C interfaces.
    else
      {
        sym(&get_initial_state, "get_initial_state");
        *reinterpret_cast<void**>(&have_property) =
          lt_dlsym(h, "have_property");
        sym(&get_successors, "get_successors");
        sym(&get_state_size, "get_state_variable_count");
        sym(&get_state_variable_name, "get_state_variable_name");
        sym(&get_state_variable_type, "get_state_variable_type");
        sym(&get_type_count, "get_state_variable_type_count");
        sym(&get_type_name, "get_state_variable_type_name");
        sym(&get_type_value_count, "get_state_variable_type_value_count");
        sym(&get_type_value_name, "get_state_variable_type_value");
      }

    if (have_property && have_property())
      throw std::runtime_error("Models with embedded properties "
                               "are not supported.");
  }

  spins_interface::~spins_interface()
  {
    lt_dlhandle h = (lt_dlhandle) handle;
    if (h)
      lt_dlclose(h);
    lt_dlexit();
  }
}
