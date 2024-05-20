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
#include <spot/twaalgos/game.hh>
#include <spot/twaalgos/relabel.hh>
#include <bddx.h>

namespace spot
{
  /// \addtogroup synthesis Reactive Synthesis
  /// \ingroup twa_algorithms

  /// \ingroup synthesis
  /// \brief make each transition a 2-step transition, transforming
  ///        the graph into an alternating arena
  ///
  /// Given a set of atomic propositions I, split each transition
  ///     p -- cond --> q                cond in 2^2^AP
  /// into a set of transitions of the form
  ///     p -- {a} --> (p,a) -- o --> q
  /// for each a in cond ∪ 2^2^I
  /// and where o = (cond & a) ∪ 2^2^O.
  ///
  /// By definition, the states p are deterministic,
  /// only the states of the form
  /// (p,a) may be non-deterministic.
  /// This function is used to transform an automaton into a turn-based game in
  /// the context of LTL reactive synthesis.
  /// The player of inputs (aka environment) plays first.
  ///
  /// \param aut          automaton to be transformed
  /// \param output_bdd   conjunction of all output AP, all APs not present
  ///                     are treated as inputs
  /// \param complete_env Whether the automaton should be complete for the
  ///                     environment, i.e. the player of inputs
  /// \note This function also computes the state players
  /// \note If the automaton is to be completed, sink states will
  ///       be added for both env and player if necessary
  SPOT_API twa_graph_ptr
  split_2step(const const_twa_graph_ptr& aut,
              const bdd& output_bdd, bool complete_env = true);

  /// \ingroup synthesis
  /// \brief Like split_2step but relying on the named property
  /// 'synthesis-outputs'
  SPOT_API twa_graph_ptr
  split_2step(const const_twa_graph_ptr& aut, bool complete_env = true);

  /// \ingroup synthesis
  /// \brief the inverse of split_2step
  ///
  /// \pre aut is an alternating arena
  /// \post All edge conditions in the returned automaton are of the form
  ///       ins&outs, with ins/outs being a condition over the input/output
  ///       propositions
  /// \note This function relies on the named property "state-player"
  SPOT_API twa_graph_ptr
  unsplit_2step(const const_twa_graph_ptr& aut);

  /// \ingroup synthesis
  /// \brief Benchmarking data and options for synthesis
  struct SPOT_API synthesis_info
  {
    enum class algo
    {
      DET_SPLIT=0,
      SPLIT_DET,
      DPA_SPLIT,
      LAR,
      LAR_OLD,
      ACD,
    };

    struct bench_var
    {
      double total_time = 0.0;
      double trans_time = 0.0;
      double split_time = 0.0;
      double paritize_time = 0.0;
      double solve_time = 0.0;
      double strat2aut_time = 0.0;
      double simplify_strat_time = 0.0;
      double aig_time = 0.0;
      unsigned nb_states_arena = 0;
      unsigned nb_states_arena_env = 0;
      unsigned nb_strat_states = 0;
      unsigned nb_strat_edges = 0;
      unsigned nb_simpl_strat_states = 0;
      unsigned nb_simpl_strat_edges = 0;
      unsigned nb_latches = 0;
      unsigned nb_gates = 0;
      bool realizable = false;
    };

    synthesis_info()
    : force_sbacc{false},
      s{algo::LAR},
      minimize_lvl{2},
      bv{},
      verbose_stream{nullptr},
      dict(make_bdd_dict())
    {
    }

    bool force_sbacc;
    algo s;
    int minimize_lvl;
    std::optional<bench_var> bv;
    std::ostream* verbose_stream;
    option_map opt;
    bdd_dict_ptr dict;
  };

  /// \ingroup synthesis
  /// \brief Stream algo
  SPOT_API std::ostream&
  operator<<(std::ostream& os, synthesis_info::algo s);

  /// \ingroup synthesis
  /// \brief Stream benchmarks and options
  SPOT_API std::ostream &
  operator<<(std::ostream &os, const synthesis_info &gi);


  /// \ingroup synthesis
  /// \brief Creates a game from a specification and a set of
  /// output propositions
  ///
  /// \param f The specification given as an LTL/PSL formula, or
  ///          as a string.
  /// \param all_outs The names of all output propositions
  /// \param gi synthesis_info structure
  /// \note All propositions in the formula that do not appear in all_outs
  /// are treated as input variables.
  /// @{
  SPOT_API twa_graph_ptr
  ltl_to_game(const formula& f,
              const std::vector<std::string>& all_outs,
              synthesis_info& gi);
  SPOT_API twa_graph_ptr
  ltl_to_game(const formula& f,
              const std::vector<std::string>& all_outs);
  SPOT_API twa_graph_ptr
  ltl_to_game(const std::string& f,
              const std::vector<std::string>& all_outs,
              synthesis_info& gi);
  SPOT_API twa_graph_ptr
  ltl_to_game(const std::string& f,
              const std::vector<std::string>& all_outs);
  /// @}

  /// \ingroup synthesis
  /// \brief creates a mealy machine from a solved game \a arena
  /// taking into account the options given in \a gi.
  /// This concerns in particular whether or not the machine is to be reduced
  /// and how.
  /// solved_game_to_mealy can return any type of mealy machine. In fact it
  /// will return the type that necessitates no additional operations
  /// @{
  SPOT_API twa_graph_ptr
  solved_game_to_mealy(twa_graph_ptr arena, synthesis_info& gi);
  SPOT_API twa_graph_ptr
  solved_game_to_mealy(twa_graph_ptr arena);
  SPOT_API twa_graph_ptr
  solved_game_to_separated_mealy(twa_graph_ptr arena, synthesis_info& gi);
  SPOT_API twa_graph_ptr
  solved_game_to_separated_mealy(twa_graph_ptr arena);
  SPOT_API twa_graph_ptr
  solved_game_to_split_mealy(twa_graph_ptr arena, synthesis_info& gi);
  SPOT_API twa_graph_ptr
  solved_game_to_split_mealy(twa_graph_ptr arena);
  /// @}

  /// \ingroup synthesis
  /// \brief A struct that represents different types of mealy like
  ///        objects
  struct SPOT_API mealy_like
  {
    enum class realizability_code
    {
      UNREALIZABLE,
      UNKNOWN,
      REALIZABLE_REGULAR,
      // strat is DTGBA and a glob_cond
      REALIZABLE_DTGBA
    };

    realizability_code success;
    twa_graph_ptr mealy_like;
    bdd glob_cond;
  };

  /// \ingroup synthesis
  /// \brief Seeks to decompose a formula into independently synthetizable
  /// sub-parts. The conjunction of all sub-parts then
  /// satisfies the specification
  ///
  /// The algorithm is based on work by Finkbeiner et al.
  /// \cite finkbeiner.21.nfm, \cite finkbeiner.21.arxiv.
  ///
  /// \param f the formula to split
  /// \param outs vector with the names of all output propositions
  /// \return A vector of pairs holding a subformula and the used output
  ///  propositions each.
  /// @{
  SPOT_API std::pair<std::vector<formula>, std::vector<std::set<formula>>>
  split_independent_formulas(formula f, const std::vector<std::string>& outs);

  SPOT_API std::pair<std::vector<formula>, std::vector<std::set<formula>>>
  split_independent_formulas(const std::string& f,
                             const std::vector<std::string>& outs);
  /// @}

  /// \ingroup synthesis
  /// \brief Creates a strategy for the formula given by calling all
  ///        intermediate steps
  ///
  /// For certain formulas, we can ''bypass'' the traditional way
  /// and find directly a strategy or some other representation of a
  /// winning condition without translating the formula as such.
  /// If no such simplifications can be made, it executes the usual way.
  ///
  /// \param f The formula to synthesize a strategy for
  /// \param output_aps A vector with the name of all output properties.
  ///                   All APs not named in this vector are treated as inputs
  /// \param want_strategy Set to false if we don't want to construct the
  /// strategy but only test realizability.
  SPOT_API mealy_like
  try_create_direct_strategy(formula f,
                             const std::vector<std::string>& output_aps,
                             synthesis_info& gi, bool want_strategy = false);

  /// \ingroup synthesis
  /// \brief Solve a game, and update synthesis_info
  ///
  /// This is just a wrapper around the solve_game() function with a
  /// single argument.  This one measure the runtime and update \a gi.
  SPOT_API bool
  solve_game(twa_graph_ptr arena, synthesis_info& gi);

  struct SPOT_API game_relabeling_map
  {
    relabeling_map env_map;
    relabeling_map player_map;
  };

  /// \ingroup synthesis
  /// \brief Tries to relabel a SPLIT game \a arena using fresh propositions.
  /// Can be applied to env or player depending on \a relabel_env
  /// and \a relabel_play. The arguments \a split_env and \a split_play
  /// determine whether or not env and player edges are to
  /// be split into several transitions labelled by letters not conditions.
  ///
  /// \return pair of relabeling_map, first is for env, second is for player.
  /// The maps are empty if no relabeling was performed
  /// \note Can also be applied to split mealy machine.
  /// \note partitioned_relabel_here can not be used directly if there are
  /// T (true conditions)
  SPOT_API game_relabeling_map
  partitioned_game_relabel_here(twa_graph_ptr& arena,
                                bool relabel_env,
                                bool relabel_play,
                                bool split_env = false,
                                bool split_play = false,
                                unsigned max_letter = -1u,
                                unsigned max_letter_mult = -1u);

  /// \ingroup synthesis
  /// \brief Undoes a relabeling done by partitioned_game_relabel_here.
  /// A dedicated function is necessary in order to remove the
  /// variables tagging env and player conditions
  SPOT_API void
  relabel_game_here(twa_graph_ptr& arena,
                    game_relabeling_map& rel_maps);

}
