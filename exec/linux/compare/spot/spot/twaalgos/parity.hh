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
#include <spot/twa/fwd.hh>
#include <vector>

namespace spot
{
  /// \addtogroup parity_algorithms Algorithms for parity acceptance
  /// \ingroup twa_acc_transform

  /// \ingroup parity_algorithms
  /// @{

  /// \brief Parity kind type
  enum parity_kind
  {
    /// The new acceptance will be a parity max
    parity_kind_max,
    /// The new acceptance will be a parity min
    parity_kind_min,
    /// The new acceptance will keep the kind
    parity_kind_same,
    /// The new acceptance may change the kind
    parity_kind_any
  };

  /// \brief Parity  style type
  enum parity_style
  {
    /// The new acceptance will be a parity odd
    parity_style_odd,
    /// The new acceptance will be a parity even
    parity_style_even,
    /// The new acceptance will keep the style
    parity_style_same,
    /// The new acceptance may change the style
    parity_style_any
  };

  /// \brief Change the parity acceptance of an automaton
  ///
  /// The parity acceptance condition of an automaton is characterized by
  ///    - The kind of the acceptance (min or max).
  ///    - The parity style, i.e., parity of the sets seen infinitely often
  ///      (odd or even).
  ///    - The number of acceptance sets.
  ///
  /// The output will be an equivalent automaton with the new parity acceptance.
  /// The number of acceptance sets may be increased by one. Every transition
  /// will belong to at most one acceptance set. The automaton must have a
  /// parity acceptance, otherwise an invalid_argument exception is thrown.
  ///
  /// The parity kind is defined only if the number of acceptance sets
  /// is strictly greater than 1. The parity_style is defined only if the number
  /// of acceptance sets is non-zero. Some values of kind and style may result
  /// in equivalent outputs if the number of acceptance sets of the input
  /// automaton is not great enough.
  ///
  /// \param aut the input automaton
  ///
  /// \param kind the parity kind of the output automaton
  ///
  /// \param style the parity style of the output automaton
  ///
  /// \return the automaton with the new acceptance
  /// @{
  SPOT_API twa_graph_ptr
  change_parity(const const_twa_graph_ptr& aut,
                parity_kind kind, parity_style style);

  SPOT_API twa_graph_ptr
  change_parity_here(twa_graph_ptr aut, parity_kind kind, parity_style style);
  /// @}

  /// \brief Remove useless acceptance sets of an automaton with parity
  /// acceptance
  ///
  /// If two sets with the same parity are separated by unused sets, then these
  /// two sets are merged. The input must be an automaton with a parity
  /// acceptance, otherwise an invalid_argument exception is thrown.
  ///
  /// \param aut the input automaton
  ///
  /// \param keep_style whether the style of the parity acc is kept.
  ///
  /// \return the automaton without useless acceptance sets.
  /// @{
  SPOT_API twa_graph_ptr
  cleanup_parity(const const_twa_graph_ptr& aut,
                            bool keep_style = false);

  SPOT_API twa_graph_ptr
  cleanup_parity_here(twa_graph_ptr aut, bool keep_style = false);
  /// @}

  /// \brief Colorize an automaton with parity acceptance
  ///
  /// An automaton is said colored iff all the transitions belong to exactly one
  /// acceptance set. The algorithm achieves that by removing superfluous
  /// acceptance marks. It may introduce a new set to mark the transitions with
  /// no acceptance sets and a second set may be introduced to keep the style.
  /// The input must be an automaton with a parity acceptance, otherwise an
  /// invalid_argument exception is thrown.
  ///
  /// \param aut the input automaton
  ///
  /// \param keep_style whether the style of the parity acc is kept.
  ///
  /// \return the colorized automaton
  /// @{
  SPOT_API twa_graph_ptr
  colorize_parity(const const_twa_graph_ptr& aut, bool keep_style = false);

  SPOT_API twa_graph_ptr
  colorize_parity_here(twa_graph_ptr aut, bool keep_style = false);
  /// @}

  /// \brief Reduce the parity acceptance condition to use a minimal
  /// number of colors.
  ///
  /// This implements an algorithm derived from the following article,
  /// but generalized to all types of parity acceptance.
  /// \cite carton.99.ita
  ///
  /// The kind of parity (min/max) is preserved, but the style
  /// (odd/even) may be altered to reduce the number of colors used.
  ///
  /// If \a colored is true, colored automata are output (this is what
  /// the above paper assumes).  Otherwise, the smallest or highest
  /// colors (depending on the parity kind) is removed to simplify the
  /// acceptance condition.
  ///
  /// If the input uses state-based acceptance, the output will use
  /// state-based acceptance as well.
  ///
  /// A parity automaton, sometimes called a chain automaton, can be
  /// seen as a stack of layers that are alternatively rejecting and
  /// accepting.  For instance imagine a parity max automaton that is
  /// strongly connected.  Removing the transitions with the maximal
  /// color might leave a few transitions that were not labeled by
  /// this maximal color, but that are part of any cycle anymore:
  /// those transition could have been colored with the maximal color,
  /// since any cycle going through them would have seen the maximal
  /// color.  (Once your remove this maximal layer,
  /// your can define the next layer similarly.)
  ///
  /// When \a layered is true all transition that belong to the same
  /// layer receive the same color.  When layer is `false`, only the
  /// transition that where used initially to define the layers (i.e,
  /// the transition with the maximal color in the previous example),
  /// get their color adjusted.  The other will receive either no
  /// color (if \a colored is false), or a useless color (if \a colored
  /// is true).  Here "useless color" means the smallest color
  /// for parity max, and the largest color for parity min.
  ///
  /// When \a layered is true, the output of this function is
  /// comparable to what acd_transform() would produce.  The
  /// difference is that this function preserve the kind (min/max) of
  /// parity input, while acd_transform() always output a parity min
  /// automaton.  Additionally, this function needs fewer resources
  /// than acd_transform() because it is already known that the input
  /// is a parity automaton.  In some (historically inaccurate) way,
  /// reduce_parity() can be seen as a specialized version of
  /// acd_transform().
  ///
  /// The reason layered is false by default, is that not introducing
  /// colors in place where there where none occasionally help with
  /// simulation-based reductions.
  ///
  /// @{
  SPOT_API twa_graph_ptr
  reduce_parity(const const_twa_graph_ptr& aut,
                bool colored = false, bool layered = false);

  SPOT_API twa_graph_ptr
  reduce_parity_here(twa_graph_ptr aut,
                     bool colored = false, bool layered = false);

  /// @}

  /// \brief Internal data computed by the reduce_parity function
  ///
  /// `piprime1` and `piprime2` have the size of `aut`'s edge vector,
  /// represent two possible colorations of the edges.  piprime1 assumes
  /// that terminal cases of the recursion are odd, and piprime2 assumes
  /// they are even.
  ///
  /// reduce_parity() actually compare the range of values in these
  /// two vectors to limit the number of colors.
  struct SPOT_API reduce_parity_data
  {
    bool parity_max;       ///< Whether the input automaton is parity max
    bool parity_odd;       ///< Whether the input automaton is parity odd
    std::vector<int> piprime1;
    std::vector<int> piprime2;

    reduce_parity_data(const const_twa_graph_ptr& aut, bool layered = false);
  };

  /// @}
}
