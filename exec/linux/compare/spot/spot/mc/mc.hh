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

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>

#include <spot/misc/common.hh>

namespace spot
{
  /// \brief The list of parallel model-checking algorithms available
  enum class SPOT_API mc_algorithm
    {
     BLOEMEN_EC,    ///< \brief Bloemen.16.hvc emptiness check
     BLOEMEN_SCC,   ///< \brief Bloemen.16.ppopp SCC computation
     CNDFS,         ///< \brief Evangelista.12.atva emptiness check
     DEADLOCK,      ///< \brief Check wether there is a deadlock
     REACHABILITY,  ///< \brief Only perform a reachability algorithm
     SWARMING,      ///< \brief Holzmann.11.ieee applied to renault.13.lpar
    };

  enum class SPOT_API mc_rvalue
    {
     DEADLOCK,                  ///< \brief A deadlock has been found
     EMPTY,                     ///< \brief The product is empty
     FAILURE,                    ///< \brief The Algorithm finished abnormally
     NO_DEADLOCK,               ///< \brief No deadlock has been found
     NOT_EMPTY,                 ///< \brief The product is not empty
     SUCCESS,                   ///< \brief The Algorithm finished normally
    };

  /// \brief This structure contains, for each thread, the collected information
  /// during the traversal
  struct SPOT_API ec_stats
  {
   std::vector<std::string> name;     ///< \brief The name of the algorithm used
   std::vector<unsigned> walltime;    ///< \brief Walltime for this thread in ms
   std::vector<unsigned> states;      ///< \brief Number of states visited
   std::vector<unsigned> transitions; ///< \brief Number of transitions visited
   std::vector<int> sccs;             ///< \brief Number of SCCs or -1
   std::vector<mc_rvalue> value;      ///< \brief The return status
   std::vector<bool> finisher;        ///< \brief Is it the finisher thread?
   std::string trace;                 ///< \brief The output trace
  };

  SPOT_API std::ostream& operator<<(std::ostream& os, const mc_algorithm& ma)
  {
    switch (ma)
      {
      case mc_algorithm::BLOEMEN_EC:
        os << "bloemen_ec";  break;
      case mc_algorithm::BLOEMEN_SCC:
        os << "bloemen_scc";  break;
      case mc_algorithm::CNDFS:
        os << "cndfs";  break;
      case mc_algorithm::DEADLOCK:
        os << "deadlock";  break;
      case mc_algorithm::REACHABILITY:
        os << "reachability";  break;
      case mc_algorithm::SWARMING:
        os << "swarming";  break;
      }
    return os;
  }

  SPOT_API std::ostream& operator<<(std::ostream& os, const mc_rvalue& mr)
  {
    switch (mr)
      {
      case mc_rvalue::DEADLOCK:
        os << "deadlock";  break;
      case mc_rvalue::EMPTY:
        os << "empty"; break;
      case mc_rvalue::FAILURE:
        os << "failure";  break;
      case mc_rvalue::NO_DEADLOCK:
        os << "no_deadlock"; break;
      case mc_rvalue::NOT_EMPTY:
        os << "not_empty";  break;
      case mc_rvalue::SUCCESS:
        os << "success"; break;
      }
    return os;
  }

  SPOT_API std::ostream& operator<<(std::ostream& os, const ec_stats& es)
  {
    for (unsigned i = 0; i < es.name.size(); ++i)
      {
        os << "---- Thread number:\t" << i << '\n'
           << "   - Algorithm:\t\t" << es.name[i] << '\n'
           << "   - Walltime (ms):\t" << es.walltime[i] <<'\n'
           << "   - States:\t\t" << es.states[i] << '\n'
           << "   - Transitions:\t" << es.transitions[i] << '\n'
           << "   - Result:\t\t" << es.value[i] << '\n'
           << "   - SCCs:\t\t" << es.sccs[i] << '\n';

        os << "CSV: tid,algorithm,walltime,states,transitions,"
              "sccs,result,finisher\n"
           << "@th_" << i << ',' << es.name[i] << ',' << es.walltime[i] << ','
           << es.states[i] << ',' << es.transitions[i] << ','
           << es.sccs[i] << ',' << es.value[i]
           << ',' << es.finisher[i] << "\n\n";
      }
    return os;
  }

  /// \brief This function helps to find the output value from a set of threads
  /// that may have different values.
  SPOT_API const mc_rvalue operator|(const mc_rvalue& lhs, const mc_rvalue& rhs)
  {
    // Handle Deadlocks
    if (lhs == mc_rvalue::DEADLOCK && rhs == mc_rvalue::DEADLOCK)
      return mc_rvalue::DEADLOCK;
    if (lhs == mc_rvalue::NO_DEADLOCK && rhs == mc_rvalue::NO_DEADLOCK)
      return mc_rvalue::NO_DEADLOCK;
    if ((lhs == mc_rvalue::DEADLOCK && rhs == mc_rvalue::NO_DEADLOCK) ||
        (lhs == mc_rvalue::NO_DEADLOCK && rhs == mc_rvalue::DEADLOCK))
      return mc_rvalue::DEADLOCK;

    // Handle Emptiness
    if (lhs == mc_rvalue::EMPTY && rhs == mc_rvalue::EMPTY)
      return mc_rvalue::EMPTY;
    if (lhs == mc_rvalue::NOT_EMPTY && rhs == mc_rvalue::NOT_EMPTY)
      return mc_rvalue::NOT_EMPTY;
    if ((lhs == mc_rvalue::EMPTY && rhs == mc_rvalue::NOT_EMPTY) ||
        (lhs == mc_rvalue::NOT_EMPTY && rhs == mc_rvalue::EMPTY))
      return mc_rvalue::EMPTY;

    // Handle Failure / Success
    if (lhs == mc_rvalue::FAILURE && rhs == mc_rvalue::FAILURE)
      return mc_rvalue::FAILURE;
    if (lhs == mc_rvalue::SUCCESS && rhs == mc_rvalue::SUCCESS)
      return mc_rvalue::SUCCESS;
    if ((lhs == mc_rvalue::FAILURE && rhs == mc_rvalue::SUCCESS) ||
        (lhs == mc_rvalue::SUCCESS && rhs == mc_rvalue::FAILURE))
      return mc_rvalue::FAILURE;

    throw std::runtime_error("Unable to compare these elements!");
  }
}
