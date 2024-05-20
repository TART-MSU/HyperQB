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

#include "common_sys.hh"

#include <argp.h>
#include <vector>
#include <spot/tl/parse.hh>

enum class job_type : char { LTL_STRING,
                             LTL_FILENAME,
                             AUT_FILENAME,
                             TLSF_FILENAME };

struct job
{
  const char* str;
  job_type type;

  job(const char* str, job_type type) noexcept
    : str(str), type(type)
  {
  }
};

typedef std::vector<job> jobs_t;
extern jobs_t jobs;
extern bool lbt_input;

extern const struct argp finput_argp;
extern const struct argp finput_argp_headless;

int parse_opt_finput(int key, char* arg, struct argp_state* state);

spot::parsed_formula parse_formula(const std::string& s);


class job_processor
{
protected:
  bool abort_run = false; // Set to true in process_formula() to abort run().
public:
  job_processor() = default;
  job_processor(const job_processor&) = delete;
  job_processor& operator=(const job_processor&) = delete;

  virtual ~job_processor();

  virtual int
  process_formula(spot::formula f,
                  const char* filename = nullptr, int linenum = 0) = 0;

  virtual int
  process_string(const std::string& str,
                 const char* filename = nullptr, int linenum = 0);
  virtual int
  process_stream(std::istream& is, const char* filename);

  virtual int
  process_ltl_file(const char* filename);

  virtual int
  process_aut_file(const char* filename);

  virtual int
  process_tlsf_file(const char* filename);

  virtual int
  run();

  char* real_filename = nullptr;
  long int col_to_read = 0;
  char* prefix = nullptr;
  char* suffix = nullptr;
};

// Report and error message or add a default job depending on whether
// the input is a tty.
void check_no_formula();
void check_no_automaton();
