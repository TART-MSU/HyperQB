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

#include <spot/twaalgos/emptiness.hh>
#include <spot/twaalgos/sccinfo.hh>
#include <spot/misc/bitvect.hh>

namespace spot
{
  /// \ingroup emptiness_check_algorithms
  /// \brief Emptiness check of an automaton, for any acceptance condition.
  ///
  /// Currently only implemented for twa_graph automata, i.e., not automata
  /// constructed on-the-fly.
  ///
  /// \cite baier.19.atva
  SPOT_API bool
  generic_emptiness_check(const const_twa_graph_ptr& aut);

  /// \ingroup emptiness_check_algorithms
  /// \brief Accepting run search in an automaton, for any acceptance condition.
  /// \return An accepting run over the automaton, or nullptr if the language is
  /// empty.
  ///
  /// Currently only implemented for twa_graph automata, i.e., not automata
  /// constructed on-the-fly.
  SPOT_API twa_run_ptr
  generic_accepting_run(const const_twa_graph_ptr& aut);

  /// \ingroup emptiness_check_algorithms
  /// \brief Emptiness check of one SCC, for any acceptance condition.
  ///
  /// \cite baier.19.atva
  SPOT_API bool
  generic_emptiness_check_for_scc(const scc_info& si, unsigned scc);

  /// \ingroup emptiness_check_algorithms
  /// \brief Emptiness check of one SCC, for any acceptance condition.
  ///
  /// This version makes it possible to ignore the acceptance
  /// condition of the automaton, and use \a forced_acc.
  ///
  /// \cite baier.19.atva
  SPOT_API bool
  generic_emptiness_check_for_scc(const scc_info& si, unsigned scc,
                                  const acc_cond& forced_acc);

#ifndef SWIG
  /// \ingroup emptiness_check_algorithms
  /// \brief Compute set of maximal accepting loops in one SCC,
  /// for any acceptance condition.
  ///
  /// This computes all maximal subsets of the edges of an SCC
  /// that form accepting (sub) SCCs.  For each such subset, the
  /// \a callback function is called with `(si, num)`, such that
  /// `si->inner_edges_of(num)` lists the relevant edges.
  ///
  /// The search is restricted to a set of edges of the given SCC
  /// for which \a keep (a bitvect indexed by edge numbers) is true.
  ///
  /// Returns false iff no accepting loop where found.
  SPOT_API bool
  maximal_accepting_loops_for_scc(const scc_info& si, unsigned scc,
                                  const acc_cond& forced_acc,
                                  const bitvect& keep,
                                  std::function<void(const scc_info&,
                                                     unsigned)> callback);
#endif

  /// \ingroup emptiness_check_algorithms
  ///
  /// Select the version of the generic-emptiness check to use, this
  /// is mainly for benchmarking purpose.
  ///
  /// We currently have three versions:
  /// - "spot28" is similar to the algorithm described our ATVA'19 paper
  ///   \cite baier.19.atva , however it has an implementation bug
  ///   that cause superfluous recursive calls to be performed (the
  ///   result is still correct.
  /// - "atva19" is similar to the algorithm described our ATVA'19 paper
  ///   \cite baier.19.atva , with the above bug fixed.
  /// - "spot29" improves upon the worst case of atva19.  This is
  ///   the default.
  /// - "spot210" improves upon "spot29" in a few cases where a Fin
  ///   is shared by multiple disjuncts.  This improve the worst
  ///   case complexity of EL-automata in the general case, but worsen
  ///   the complexity of Hyper-Rabin in particular.
  /// - "spot211" is another attempt at fixing worst case complexities.
  ///    Compared to atva19, this improves the complexities for Rabin,
  ///    GeneralizedRabin, and EL without worsening the complexity of
  ///    Hyper-Rabin.
  SPOT_API void
  generic_emptiness_check_select_version(const char* emversion = nullptr);

  /// \ingroup emptiness_check_algorithms
  ///
  /// Give the set of transitions contained in
  /// an accepting cycle of the SCC \a scc of \a aut.
  ///
  /// \param si scc_info used to describe the automaton
  /// \param scc SCC to consider
  /// \param aut_acc Acceptance condition used for this SCC
  /// \param removed_colors A set of colors that can't appear on a transition
  /// \param accepting_transitions The result. Must be a vector of size at least
  /// the max index + 1 of a transition of the SCC scc and the value of each
  /// index of a transition of this SCC must be set to false
  /// \param kept A list of booleans that say if a transition is kept even if
  /// it does not have an element of removed_colors
  /// \return True if there is an accepting transition
  SPOT_API bool
  accepting_transitions_scc(const scc_info &si, unsigned scc,
                            const acc_cond aut_acc,
                            acc_cond::mark_t removed_colors,
                            std::vector<bool>& accepting_transitions,
                            const bitvect& kept);

  /// \ingroup emptiness_check_algorithms
  ///
  /// Give the set of transitions contained in an accepting cycle of \a aut.
  /// \param aut Automaton to process
  /// \param cond Acceptance condition associated
  SPOT_API std::vector<bool>
  accepting_transitions(const const_twa_graph_ptr aut, acc_cond cond);

}
