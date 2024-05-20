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

#include <iosfwd>
#include <spot/misc/common.hh>
#include <spot/twa/fwd.hh>
#include <vector>
#include <unordered_map>
#include <utility>
#include <functional>
#include <bddx.h>

namespace spot
{
  /// \ingroup twa_io
  /// \brief Print reachable states in Hanoi Omega Automata format.
  ///
  /// \param os The output stream to print on.
  /// \param g The automaton to output.
  /// \param opt a set of characters each corresponding to a possible
  ///        option: (b) create an alias basis if more >=2 AP
  ///        are used, (i) implicit labels for complete and
  ///        deterministic automata, (k) state labels when possible,
  ///        (s) state-based acceptance when possible, (t)
  ///        transition-based acceptance, (m) mixed acceptance, (l)
  ///        single-line output, (v) verbose properties, (1.1) use
  ///        version 1.1 of the HOA format.
  SPOT_API std::ostream&
  print_hoa(std::ostream& os,
            const const_twa_ptr& g,
            const char* opt = nullptr);

  /// \ingroup twa_io
  /// \brief Obtain aliases used in the HOA format.
  ///
  /// Aliases are stored as a vector of pairs (name, bdd).
  /// The bdd is expected to use only variable registered by \a g.
  SPOT_API std::vector<std::pair<std::string, bdd>>*
  get_aliases(const const_twa_ptr& g);

  /// \ingroup twa_io
  /// \brief Define all aliases used in the HOA format.
  ///
  /// Aliases are stored as a vector of pairs (name, bdd).
  /// The bdd is expected to use only variable registered by \a g.
  ///
  /// Pass an empty vector to remove existing aliases.
  SPOT_API void
  set_aliases(twa_ptr g,
              const std::vector<std::pair<std::string, bdd>>& aliases);

  /// \ingroup twa_io
  /// \brief Help printing BDDs as text, using aliases.
  class SPOT_API hoa_alias_formater final
  {
  public:
    typedef std::vector<std::pair<std::string, bdd>> aliases_t;
  private:
    aliases_t* aliases_;
    std::unordered_map<int, unsigned> aliases_map_;
    std::vector<std::pair<bdd, unsigned>> alias_cubes_;
    const char* false_str_;
    const char* true_str_;
    const char* or_str_;
    const char* and_str_;
    const char* not_str_;
    const char* lpar_str_;
    const char* rpar_str_;
    std::function<std::string(int)> ap_printer_;
  public:
    /// \brief Initialize this class from the aliases defined
    /// for an automaton.
    ///
    /// The aliases are taken from the named-property "aliases",
    /// defined in the automaton.  If no such named-property exist,
    /// the aliases() method will return a null pointer.
    ///
    /// Note that any alias that uses an atomic proposition not
    /// registered in the automaton is not only ignored, but also
    /// removed from the alias list stored in the automaton.
    ///
    /// The \a or_str, \a and_str, and \a ap_printer arguments are
    /// used to print operators OR, AND, and to print atomic propositions
    /// that are not aliases.  \a lpar_str and \a rpar_str are used
    /// to group conjuncts that appear in a disjunction.
    hoa_alias_formater(const const_twa_graph_ptr& aut,
                       const char* false_str,
                       const char* true_str,
                       const char* or_str,
                       const char* and_str,
                       const char* not_str,
                       const char* lpar_str,
                       const char* rpar_str,
                       std::function<std::string(int)> ap_printer);

    /// \brief Attempt to format a BDD label using aliases.
    ///
    /// The returned pair should be interpreted as a disjunction
    /// between an expression involving aliases, and a BDD that could
    /// not be expressed using aliases.
    ///
    /// The conversion works in several attempts, in the following order:
    ///
    /// - If an alias A exists for \a label, `"@A"` is returned.
    ///
    /// - If an alias A exists for the negation of \a label, `"!@A"`
    ///   is returned.
    ///
    /// - If \a label is true or false, `true_str` or `false_str`
    ///   are returned as specified at construction.
    ///
    /// - If \a label can be built as a disjunction of positive
    ///   aliases, such a disjunction is returned as `"@A | @B | @C"`.
    ///
    /// - If \a label can be built from a conjunction of
    ///   aliases or negated aliases, it is returned as
    ///   `"@A&!@B&@C"`.  Note that this check first
    ///   attempt to use aliases that represent cubes before trying
    ///   other aliases.
    ///
    /// - Otherwise \a label is split in irredundant-sum-of-products
    ///   and each clause is encoded as a conjunctions of (possibly
    ///   negated) aliases using only those that are cubes.  Any
    ///   remaining literal is encoded with the ap_printer() function
    ///   passed to the constructor.
    ///
    /// The aliases to be used are those returned by the aliases()
    /// method.  The vector returned by aliases() is scanned from
    /// position \a aliases_start.  As this vector is stored in the
    /// opposite order of how aliases should be stored in the HOA
    /// output, adjusting \a aliases_start is helpful to encode an
    /// aliases using only previously defined aliases (i.e., aliases
    /// that appear later in the sequence returned by aliases()).
    std::string
    encode_label(bdd label, unsigned aliases_start = 0);

    /// \brief Retrieve the list of aliases.
    ///
    /// This points to the same list that the automaton's "aliases"
    /// named properties points to.  Will return `nullptr` if no
    /// aliases are defined.
    ///
    /// \note Aliases involving atomic propositions that are not
    /// declared in the automaton have been removed by this class'
    /// constructor.
    aliases_t* aliases()
    {
      return aliases_;
    }
  };

  /// \ingroup twa_io
  /// \brief Create an alias basis
  ///
  /// This use spot::edge_separator to build a set of alias that can
  /// be used as a basis for all labels of the automaton.
  ///
  /// Such a basis can be used to shorten the size of an output file
  /// when printing in HOA format (actually, calling print_hoa() with
  /// option 'b' will call this function).  Such a basis may also be
  /// useful to help visualize an automaton (using spot::print_dot's
  /// `@` option) when its labels are too large.
  SPOT_API void
  create_alias_basis(const twa_graph_ptr& aut);

}
