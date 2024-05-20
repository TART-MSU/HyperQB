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

#include <spot/twa/twagraph.hh>

/// \addtogroup mealy Functions related to Mealy machines
/// \ingroup twa_algorithms

namespace spot
{
  // Forward decl
  struct synthesis_info;

  /// \ingroup mealy
  /// \brief Checks whether the automaton is a mealy machine
  ///
  /// A mealy machine is an automaton with the named property
  /// `"synthesis-outputs"` and and that has a "true" as acceptance
  /// condition.
  ///
  /// \param m The automaton to be verified
  /// \see is_separated_mealy
  /// \see is_split_mealy
  /// \see is_input_deterministic_mealy
  SPOT_API bool
  is_mealy(const const_twa_graph_ptr& m);

  /// \ingroup mealy
  /// \brief Checks whether the automaton is a separated mealy machine
  ///
  /// A separated mealy machine is a mealy machine with
  /// all transitions having the form `(in)&(out)` where `in` and
  /// `out` are BDDs over the input and output propositions.
  ///
  /// \param m The automaton to be verified
  ///
  /// \see is_mealy
  /// \see is_split_mealy
  SPOT_API bool
  is_separated_mealy(const const_twa_graph_ptr& m);

  /// \ingroup mealy
  /// \brief Checks whether or not the automaton is a split mealy machine
  ///
  /// A split mealy machine is a mealy machine machine that has
  /// be converted into a game.  It should have the named property
  /// `"state-player"`, moreover the game should be alternating
  /// between the two players.  Transitions leaving states owned by
  /// player 0 (the environment) should use only input propositions,
  /// while transitions leaving states owned by player 1 (the
  /// controller) should use only output propositions.
  ///
  /// \param m The automaton to be verified
  /// \see is_mealy
  /// \see is_separated_mealy
  SPOT_API bool
  is_split_mealy(const const_twa_graph_ptr& m);

  /// \brief Checks whether a mealy machine is input deterministic
  ///
  /// A machine is input deterministic if none of the states has two
  /// outgoing transitions that can agree on a common assignment of
  /// the input propositions.  In case the mealy machine is split, the
  /// previous condition is tested only on states owned by player 0
  /// (the environment).
  ///
  /// \param m The automaton to be verified
  /// \see is_mealy
  SPOT_API bool
  is_input_deterministic_mealy(const const_twa_graph_ptr& m);


  /// \ingroup mealy
  /// \brief split a separated mealy machine
  ///
  /// In a separated mealy machine, every transitions as a label of
  /// the form `(in)&(out)`.  This function will turn each transition
  /// into a pair of consecutive transitions labeled by `in` and
  /// `out`, and turn the mealy machine into a game (what we call a
  /// split mealy machine)
  ///
  /// \param m separated mealy machine to be split
  /// \see is_split_mealy
  /// @{
  SPOT_API twa_graph_ptr
  split_separated_mealy(const const_twa_graph_ptr& m);

  SPOT_API void
  split_separated_mealy_here(const twa_graph_ptr& m);
  /// @}

  /// \ingroup mealy
  /// \brief the inverse of split_separated_mealy
  ///
  /// Take a split mealy machine \a m, and build a separated mealy machine.
  ///
  /// \see split_separated_mealy
  /// \see is_split_mealy
  /// \see is_separated_mealy
  SPOT_API twa_graph_ptr
  unsplit_mealy(const const_twa_graph_ptr& m);

  /// \ingroup mealy
  /// \brief reduce an (in)completely specified mealy machine
  ///
  /// This is a bisimulation based reduction, that optionally use
  /// inclusion between signatures to force some output when there is
  /// a choice in order to favor more reductions.  Only infinite
  /// traces are considered.  See \cite renkin.22.forte for details.
  ///
  /// \param mm The mealy machine to be minimized, has to be unsplit.
  /// \param output_assignment Whether or not to use output assignment
  /// \return A specialization of \c mm.
  ///
  /// \note If mm is separated, the returned machine is separated as
  /// well.
  /// @{
  SPOT_API twa_graph_ptr
  reduce_mealy(const const_twa_graph_ptr& mm,
               bool output_assignment = true);

  SPOT_API void
  reduce_mealy_here(twa_graph_ptr& mm,
                    bool output_assignment = true);
  /// @}

  /// \ingroup mealy
  /// \brief Minimizes an (in)completely specified mealy machine
  ///
  /// The approach is described in \cite renkin.22.forte.
  ///
  /// \param premin Whether to use reduce_mealy as a preprocessing:
  ///   - -1: Do not use;
  ///   - 0: Use without output assignment;
  ///   - 1: Use with output assignment.
  /// \return A split mealy machines which is a minimal
  /// specialization of the original machine.
  ///
  /// \note Enabling \a premin will remove finite traces.
  /// \see is_split_mealy_specialization
  SPOT_API twa_graph_ptr
  minimize_mealy(const const_twa_graph_ptr& mm, int premin = -1);

  /// \ingroup mealy
  /// \brief Minimizes an (in)completely specified mealy machine
  ///
  /// The approach is described in \cite renkin.22.forte.
  ///
  /// \param si synthesis_info structure used to store data for benchmarking
  /// and indicates which premin level to use
  ///
  /// \return A split mealy machines which is a minimal
  /// specialization of the original machine.
  ///
  /// \note Enabling \a premin will remove finite traces.
  /// \note If si.opt contains an option "satlogcsv" detailed results will be
  /// stored in this file. If it contains "satlogdimacs" all sat problems will
  /// stored.
  /// \see is_split_mealy_specialization

  SPOT_API twa_graph_ptr
  minimize_mealy(const const_twa_graph_ptr& mm,
                 synthesis_info& si);


  /// \ingroup mealy
  /// \brief Test if the split mealy machine \a right is a specialization of
  /// the split mealy machine \a left.
  ///
  /// That is, all input sequences valid for left must be applicable
  /// for right and the induced sequence of output signals of right
  /// must imply the ones of left
  SPOT_API bool
  is_split_mealy_specialization(const_twa_graph_ptr left,
                                const_twa_graph_ptr right,
                                bool verbose = false);

  /// \ingroup mealy
  /// \brief Product between two mealy machines \a left and \a right.
  /// \pre The machines have to be both either split or unsplit,
  /// input complete and compatible. All of this is checked by assertion.
  /// \result A mealy machine representing the shared behaviour,
  /// with the same type (mealy/separated/split) as the input machines
  SPOT_API twa_graph_ptr
  mealy_product(const const_twa_graph_ptr& left,
                const const_twa_graph_ptr& right);

  /// \ingroup mealy
  /// \brief Convenience function to call minimize_mealy or reduce_mealy.
  /// Uses the same convention as ltlsynt for \a minimize_lvl (or the
  /// field `minimize_lvl` of \a si):
  ///   - 0: no reduction
  ///   - 1: bisimulation based reduction
  ///   - 2: bisimulation with output assignment
  ///   - 3: SAT minimization
  ///   - 4: 1 then 3
  ///   - 5: 2 then 3
  ///
  /// Minimizes the given machine \a m inplace, the parameter
  /// \a split_out specifies if the result should be split.
  /// @{
  SPOT_API void
  simplify_mealy_here(twa_graph_ptr& m, int minimize_lvl,
                      bool split_out);

  SPOT_API void
  simplify_mealy_here(twa_graph_ptr& m, synthesis_info& si,
                      bool split_out);
  /// @}
}
