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

#include <spot/tl/formula.hh>
#include <set>
#include <vector>
#include <bddx.h>
#include <spot/twa/fwd.hh>

namespace spot
{
  /// \addtogroup tl_misc
  /// @{

  /// Set of atomic propositions.
  typedef std::set<formula> atomic_prop_set;

  /// \brief construct an atomic_prop_set with n propositions
  SPOT_API
  atomic_prop_set create_atomic_prop_set(unsigned n);

  /// \brief Return the set of atomic propositions occurring in a formula.
  ///
  /// \param f the formula to inspect
  /// \param s an existing set to fill with atomic_propositions discovered,
  ///        or 0 if the set should be allocated by the function.
  /// \return A pointer to the supplied set, \c s, augmented with
  ///        atomic propositions occurring in \c f; or a newly allocated
  ///        set containing all these atomic propositions if \c s is 0.
  SPOT_API atomic_prop_set*
  atomic_prop_collect(formula f, atomic_prop_set* s = nullptr);

  /// \brief Return the set of atomic propositions occurring in a
  /// formula, as a BDD.
  ///
  /// \param f the formula to inspect
  /// \param a that automaton that should register the BDD variables used.
  /// \return A conjunction the atomic propositions.
  SPOT_API bdd
  atomic_prop_collect_as_bdd(formula f, const twa_ptr& a);


  /// \brief Collect the literals occurring in f
  ///
  /// This function records each atomic proposition occurring in f
  /// along with the polarity of its occurrence.  For instance if the
  /// formula is `G(a -> b) & X(!b & c)`, then this will output `{!a,
  /// b, !b, c}`.
  SPOT_API
  atomic_prop_set collect_literals(formula f);

  /// \brief Collect equivalent APs
  ///
  /// Looks for patterns like `...&G(...&(x->y)&...)&...` or
  /// other forms of constant implications, then build a graph
  /// of implications to compute equivalence classes of literals.
  SPOT_API
  std::vector<std::vector<spot::formula>>
  collect_equivalent_literals(formula f);



  /// \brief Simplify a reactive specification, preserving realizability
  class SPOT_API realizability_simplifier final
  {
  public:
    enum realizability_simplifier_option {
      /// \brief remove APs with single polarity
      polarity = 1,
      /// \brief remove equivalent APs
      global_equiv = 2,
      /// \brief likewise, but don't consider equivalent input and output
      global_equiv_output_only = 6,
    };

    realizability_simplifier(formula f,
                             const std::vector<std::string>& inputs,
                             unsigned options = polarity | global_equiv,
                             std::ostream* verbose = nullptr);

    /// \brief Return the simplified formula.
    formula simplified_formula() const
    {
      return f_;
    }

    /// \brief Returns a vector of (from,from_is_input,to)
    const std::vector<std::tuple<formula, bool, formula>>& get_mapping() const
    {
      return mapping_;
    }

    /// \brief Augment the current mapping with output variable renaming from
    /// another realizability_simplifier.
    void merge_mapping(const realizability_simplifier& other);

    /// \brief Patch a Mealy machine to add the missing APs.
    void patch_mealy(twa_graph_ptr mealy) const;

    /// \brief Patch a game to add the missing APs.
    void patch_game(twa_graph_ptr mealy) const;

  private:
    void add_to_mapping(formula from, bool from_is_input, formula to);
    std::vector<std::tuple<formula, bool, formula>> mapping_;
    formula f_;
    bool global_equiv_output_only_;
  };

  /// @}
}
