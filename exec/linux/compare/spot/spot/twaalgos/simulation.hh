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

#include <spot/misc/common.hh>
#include <spot/twa/twagraph.hh>

namespace spot
{
  /// \addtogroup twa_reduction
  /// @{

  /// @{
  /// \brief Attempt to reduce the automaton by direct simulation.
  ///
  /// When the suffixes (letter and acceptance conditions) reachable
  /// from one state are included in the suffixes seen by another one,
  /// the former state can be merged into the latter.  The algorithm is
  /// described in \cite babiak.13.spin .
  ///
  /// Our reconstruction of the quotient automaton based on this
  /// suffix-inclusion relation will also improve determinism thanks
  /// to a kind of transition-pruning.
  ///
  /// We recommend to call scc_filter() to first simplify the
  /// automaton that should be reduced by simulation.
  ///
  /// Reducing an automaton by simulation does not change the number
  /// of acceptance conditions.  In some rare cases (1 out of more
  /// than 500 in our benchmark), the reduced automaton will use more
  /// acceptance conditions than necessary, and running scc_filter()
  /// again afterwards will remove these superfluous conditions.
  ///
  ///
  /// The resulting automaton has a named property "simulated-states",
  /// that is a vector mapping each state of the input to a state of
  /// the output. Note that some input states may be mapped to -1, as
  /// a by-product of transition pruning.
  ///
  /// \param automaton the automaton to simulate.
  ///
  /// \param trans_pruning Transition pruning requires a quadratic
  /// number of BDD implication checks between all equivalence
  /// classes, so it can be costly on large automata.  If \a
  /// trans_pruning is set to a non-negative integer, only
  /// (non-deterministic) automata with more states than trans_pruning
  /// will be simplified.
  ///
  /// \return a new automaton which is at worst a copy of the received
  /// one
  SPOT_API twa_graph_ptr
  simulation(const const_twa_graph_ptr& automaton,
             int trans_pruning = -1);
  SPOT_API twa_graph_ptr
  simulation(const const_twa_graph_ptr& automaton,
             std::vector<bdd>* implications,
             int trans_pruning = -1);
  SPOT_API twa_graph_ptr
  simulation_sba(const const_twa_graph_ptr& automaton,
                 int trans_pruning = -1);
  /// @}

  /// @{
  /// \brief Attempt to reduce the automaton by reverse simulation.
  ///
  /// When the prefixes (letter and acceptance conditions) leading to
  /// one state are included in the prefixes leading to one, the former
  /// state can be merged into the latter.  \cite babiak.13.spin .
  ///
  /// Our reconstruction of the quotient automaton based on this
  /// prefix-inclusion relation will also improve codeterminism
  /// thanks to a kind of transition pruning.
  ///
  /// We recommend to call scc_filter() to first simplify the
  /// automaton that should be reduced by cosimulation.
  ///
  /// Reducing an automaton by reverse simulation (1) does not change
  /// the number of acceptance conditions so the resulting automaton
  /// may have superfluous acceptance conditions, and (2) can create
  /// SCCs that are terminal and non-accepting.  For these reasons,
  /// you should call scc_filer() to prune useless SCCs and acceptance
  /// conditions afterwards.
  ///
  /// If you plan to run both simulation() and cosimulation() on the
  /// same automaton, you should start with simulation() so that the
  /// codeterminism improvements achieved by cosimulation() does not
  /// hinder the determinism improvements attempted by simulation().
  /// (This of course assumes that you prefer determinism over
  /// codeterminism.)
  ///
  /// \param automaton the automaton to simulate.
  ///
  /// \param trans_pruning Transition pruning requires a quadratic
  /// number of BDD implication checks between all equivalence
  /// classes, so it can be costly on large automata.  If \a
  /// trans_pruning is set to a non-negative integer, only
  /// (non-deterministic) automata with more states than trans_pruning
  /// will be simplified.
  ///
  /// \return a new automaton which is at worst a copy of the received
  /// one
  SPOT_API twa_graph_ptr
  cosimulation(const const_twa_graph_ptr& automaton,
               int trans_pruning = -1);
  SPOT_API twa_graph_ptr
  cosimulation_sba(const const_twa_graph_ptr& automaton,
                   int trans_pruning = -1);
  /// @}

  /// @{
  /// \brief Iterate simulation() and cosimulation().
  ///
  /// Runs simulation(), cosimulation(), and scc_filter() in a loop,
  /// until the automaton does not change size (states and
  /// transitions).
  ///
  /// We recommend to call scc_filter() to first simplify the
  /// automaton that should be reduced by iterated simulations, since
  /// this algorithm will only call scc_filter() at the end of the
  /// loop.
  ///
  /// \param automaton the automaton to simulate.
  /// \return a new automaton which is at worst a copy of the received
  /// one
  SPOT_API twa_graph_ptr
  iterated_simulations(const const_twa_graph_ptr& automaton,
                       int trans_pruning = -1);
  SPOT_API twa_graph_ptr
  iterated_simulations_sba(const const_twa_graph_ptr& automaton,
                           int trans_pruning = -1);
  /// @}

  /// @{
  /// \brief Attempt to reduce the automaton by direct simulation.
  ///
  /// Compute direct simulation for all states using \cite clemente.2.17.corr,
  /// then reduce the automaton.
  ///
  /// There is no need to call scc_filter() before as it is always applied to
  /// remove dead and unreachable states.
  ///
  /// \param aut the automaton to simulate.
  /// \return a new automaton which is at worst a copy of the received
  /// one
  SPOT_API
  twa_graph_ptr reduce_direct_sim(const const_twa_graph_ptr& aut);
  SPOT_API
  twa_graph_ptr reduce_direct_sim_sba(const const_twa_graph_ptr& aut);
  /// @}

  /// @{
  /// \brief Attempt to reduce the automaton by reverse simulation.
  ///
  /// Reverse the automaton, compute the simulation and reduce it in the same
  /// way as reduce_direct_sim().
  ///
  /// There is no need to call scc_filter() before as it is always applied to
  /// remove dead and unreachable states.
  ///
  /// \param aut the automaton to simulate.
  /// \return a new automaton which is at worst a copy of the received
  /// one
  SPOT_API
  twa_graph_ptr reduce_direct_cosim(const const_twa_graph_ptr& aut);
  SPOT_API
  twa_graph_ptr reduce_direct_cosim_sba(const const_twa_graph_ptr& aut);
  /// @}

  /// @{
  /// \brief Iterate reduce_direct_sim() and reduce_direct_cosim().
  ///
  /// Runs reduce_direct_sim() and reduce_direct_cosim() in a loop,
  /// until the automaton does not change size (states and
  /// transitions).
  ///
  /// There is no need to call scc_filter() before as it is always applied to
  /// remove dead and unreachable states.
  ///
  /// \param aut the automaton to simulate.
  /// \return a new automaton which is at worst a copy of the received
  /// one
  SPOT_API
  twa_graph_ptr reduce_iterated(const const_twa_graph_ptr& aut);
  SPOT_API
  twa_graph_ptr reduce_iterated_sba(const const_twa_graph_ptr& aut);
  /// @}

} // End namespace spot.
