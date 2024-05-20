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

#include <spot/twa/fwd.hh>
#include <spot/misc/common.hh>
#include <vector>

namespace spot
{
  /// Structure used by to_parity to store some information about the
  /// construction
  struct to_parity_data
  {
    /// Total number of states created
    unsigned nb_states_created = 0;
    /// Total number of edges created
    unsigned nb_edges_created = 0;
    /// Name of algorithms used
    std::vector<std::string> algorithms_used;
  };

  /// \ingroup twa_acc_transform
  /// \brief Options to control various optimizations of to_parity().
  struct to_parity_options
  {
    /// If \c search_ex is true, whenever CAR or IAR have to move
    /// several elements in a record, it tries to find an order such
    /// that the new permutation already exists.
    bool search_ex = true;
    /// If \c use_last is true and \a search_ex are true, we use the
    /// most recent state when we find multiple existing state
    /// compatible with the current move.
    bool use_last = true;
    /// If \c use_last_post_process is true, then when LAR ends, it tries to
    /// replace the destination of an edge by the newest compatible state.
    bool use_last_post_process = false;
    /// If \c force_order is true, we force to use an order when CAR or IAR is
    /// applied. Given a state s and a set E ({0}, {0 1}, {2} for example) we
    /// construct an order on colors. With the given example, we ask to have
    /// a permutation that start with [0 …], [0 1 …] or [2 …] in
    ///  that order of preference.
    bool force_order = true;
    /// If \c partial_degen is true, apply a partial
    /// degeneralization to remove occurrences of acceptance
    /// subformulas such as `Fin(x) | Fin(y)` or `Inf(x) & Inf(y)`.
    bool partial_degen = true;
    /// If \c scc_acc_clean is true, to_parity() will ignore colors
    /// not occurring in an SCC while processing this SCC.
    bool acc_clean = true;
    /// If \c parity_equiv is true, to_parity() will check if there
    /// exists a way to see the acceptance condition as a parity max one.
    bool parity_equiv = true;
    /// If \c Car is true, to_parity will try to apply CAR. It is a version of
    /// LAR that tracks colors.
    bool car = true;
    /// If \c tar is true, to_parity will try to apply TAR. It is a version of
    /// LAR that tracks transitions instead of colors.
    bool tar = false;
    /// If \c iar is true, to_parity will try to apply IAR.
    bool iar = true;
    /// if \c lar_dfs is true, then when LAR is used the next state of the
    /// result that will be processed is the last created state.
    bool lar_dfs = true;
    /// If \c bscc is true, to_parity() will only keep the bottommost automaton
    /// when it applies LAR.
    bool bscc = true;
    /// If \c parity_prefix is true, to_parity() will use a special
    /// handling for acceptance conditions of the form `Inf(m0) |
    /// (Fin(m1) & (Inf(m2) | (… β)))` that start as a parity
    /// condition (modulo a renumbering of `m0`, `m1`, `m2`, ...) but where
    /// `β` can be an arbitrary formula.  In this case, the paritization
    /// algorithm is really applied only to `β`, and the marks of the
    /// prefix are appended after a suitable renumbering.
    bool parity_prefix = true;
    /// If \c parity_prefix_general is true, to_parity() will rewrite the
    /// acceptance condition as `Inf(m0) | (Fin(m1) & (Inf(m2) | (… β)))` before
    /// applying the same construction as with the option \c parity_prefix.
    bool parity_prefix_general = false;
    /// If \c generic_emptiness is true, to_parity() will check if the automaton
    /// does not accept any word with an emptiness check algorithm.
    bool generic_emptiness = false;
    /// If \c rabin_to_buchi is true, to_parity() tries to convert a Rabin or
    /// Streett condition to Büchi or co-Büchi with
    /// rabin_to_buchi_if_realizable().
    bool rabin_to_buchi = true;
    /// If \c buchi_type_to_buchi is true, to_parity converts a
    /// (co-)Büchi type automaton to a (co-)Büchi automaton.
    bool buchi_type_to_buchi = false;
    /// If \c parity_type_to_parity is true, to_parity converts a
    /// parity type automaton to a parity automaton.
    bool parity_type_to_parity = false;
    /// Only allow partial degeneralization if it reduces the number of colors
    /// in the acceptance condition or if it implies to apply IAR instead of
    /// CAR.
    bool reduce_col_deg = false;
    /// Use propagate_marks_here to increase the number of marks on transition
    /// in order to move more colors (and increase the number of
    /// compatible states) when we apply LAR.
    bool propagate_col = true;
    /// If \c use_generalized_buchi is true, each SCC will use a generalized
    /// Rabin acceptance in order to avoid CAR.
    bool use_generalized_rabin = false;
    /// If \c pretty_print is true, states of the output automaton are
    /// named to help debugging.
    bool pretty_print = false;
    /// Structure used to store some information about the construction.
    to_parity_data* datas = nullptr;
  };

  /// \ingroup twa_acc_transform
  /// \brief Take an automaton with any acceptance condition and return an
  /// equivalent parity automaton.
  ///
  /// If the input is already a parity automaton of any kind, it is
  /// returned unchanged.  Otherwise a new parity automaton with max
  /// odd or max even condition is created.
  ///
  /// This procedure combines many strategies in an attempt to produce
  /// the smallest possible parity automaton.  Some of the strategies
  /// include CAR (color acceptance record), IAR (index appearance
  /// record), partial degeneralization, conversion from Rabin to Büchi
  /// when possible, etc.
  ///
  /// The \a options argument can be used to selectively disable some of the
  /// optimizations.
  SPOT_API twa_graph_ptr
  to_parity(const const_twa_graph_ptr &aut,
            const to_parity_options options = to_parity_options());

  /// \ingroup twa_acc_transform
  /// \brief Take an automaton with any acceptance condition and return an
  /// equivalent parity automaton.
  ///
  /// The parity condition of the returned automaton is max even.
  ///
  /// This implements a straightforward adaptation of the LAR (latest
  /// appearance record) to automata with transition-based marks.  We
  /// call this adaptation the CAR (color appearance record), as it
  /// tracks colors (i.e., acceptance sets) instead of states.
  ///
  /// It is better to use to_parity() instead, as it will use better
  /// strategies when possible, and has additional optimizations.
  SPOT_API twa_graph_ptr
  to_parity_old(const const_twa_graph_ptr& aut, bool pretty_print = false);

  /// \ingroup twa_acc_transform
  /// \brief Turn a Rabin-like or Streett-like automaton into a parity automaton
  /// based on the index appearence record (IAR)
  ///
  /// This is an implementation of \cite kretinsky.17.tacas .
  /// If the input automaton has n states and k pairs, the output automaton has
  /// at most k!*n states and 2k+1 colors. If the input automaton is
  /// deterministic, the output automaton is deterministic as well, which is the
  /// intended use case for this function. If the input automaton is
  /// non-deterministic, the result is still correct, but way larger than an
  /// equivalent Büchi automaton.
  ///
  /// If the input automaton is Rabin-like (resp. Streett-like), the output
  /// automaton has max odd (resp. min even) acceptance condition.
  ///
  /// Throws an std::runtime_error if the input is neither Rabin-like nor
  /// Street-like.
  ///
  /// It is better to use to_parity() instead, as it will use better
  /// strategies when possible, and has additional optimizations.
  SPOT_DEPRECATED("use to_parity() instead") // deprecated since Spot 2.9
  SPOT_API twa_graph_ptr
  iar(const const_twa_graph_ptr& aut, bool pretty_print = false);

  /// \ingroup twa_acc_transform
  /// \brief Turn a Rabin-like or Streett-like automaton into a parity automaton
  /// based on the index appearance record (IAR)
  ///
  /// Returns nullptr if the input automaton is neither Rabin-like nor
  /// Streett-like, and calls spot::iar() otherwise.
  SPOT_DEPRECATED("use to_parity() and spot::acc_cond::is_rabin_like() instead")
  SPOT_API twa_graph_ptr   // deprecated since Spot 2.9
  iar_maybe(const const_twa_graph_ptr& aut, bool pretty_print = false);

  /// \ingroup twa_acc_transform
  /// \brief Convert an automaton into a parity max automaton preserving
  /// structure when possible.
  ///
  /// Return nullptr if no such automaton is found.
  /// \param aut Automaton that we want to convert
  SPOT_API twa_graph_ptr
  parity_type_to_parity(const twa_graph_ptr &aut);

  /// \ingroup twa_acc_transform
  /// \brief Convert an automaton into a Büchi automaton preserving structure
  /// when possible.
  ///
  /// Return nullptr if no such automaton is found.
  /// \param aut Automaton that we want to convert
  SPOT_API twa_graph_ptr
  buchi_type_to_buchi(const twa_graph_ptr &aut);

  /// \ingroup twa_acc_transform
  /// \brief Convert an automaton into a co-Büchi automaton preserving structure
  /// when possible.
  ///
  /// Return nullptr if no such automaton is found.
  /// \param aut Automaton that we want to convert
  SPOT_API twa_graph_ptr
  co_buchi_type_to_co_buchi(const twa_graph_ptr &aut);
}
