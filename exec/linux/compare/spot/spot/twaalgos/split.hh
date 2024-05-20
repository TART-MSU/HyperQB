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
#include <vector>

namespace spot
{
  /// \ingroup twa_misc
  /// \brief transform edges into transitions
  ///
  /// Create a new version of the automaton where all edges are split
  /// so that they are all labeled by a conjunction of all atomic
  /// propositions.
  ///
  /// So if an edge is labeled by "true", it will be split into
  /// $2^{AP}$ distinct edges.
  ///
  /// After this we can consider that each edge of the automaton is a
  /// transition labeled by one of $2^{AP}$ letters.
  ///
  /// \see separate_edges
  SPOT_API twa_graph_ptr split_edges(const const_twa_graph_ptr& aut);

#ifndef SWIG
  // pseudo container that we use to iterate over
  // the items of LABELS that are compatible with COND.
  template<bool subsumed>
  struct SPOT_API edge_separator_filter
    {
      edge_separator_filter(const std::vector<bdd>& labels, bdd cond)
      : labels_(labels), cond_(cond)
      {
      }

      class iterator
      {
        std::vector<bdd>::const_iterator pos_;
        std::vector<bdd> const& labels_;
        bdd cond_;

      public:
        iterator(const std::vector<bdd>& labels, bdd cond)
          : labels_(labels), cond_(cond)
        {
          pos_ = labels_.begin();
          next();
        }

        iterator& operator++()
        {
          ++pos_;
          next();
          return *this;
        }

        void next()
        {
          // If subsumed is true, we want to match the labels
          // that imply the current condition.  Otherwise we
          // want to match the labels that are compatible.
          while (pos_ != labels_.end() &&
                 ((subsumed && !bdd_implies(*pos_, cond_))
                  || (!subsumed && (*pos_ & cond_) == bddfalse)))
            ++pos_;
        }

        bdd operator*() const
        {
          if (subsumed)
            return *pos_;
          else
            return *pos_ & cond_;
        }

        bool operator==(const iterator& other) const
        {
          return pos_ == other.pos_;
        }

        bool operator!=(const iterator& other) const
        {
          return pos_ != other.pos_;
        }

        bool operator==(std::vector<bdd>::const_iterator pos) const
        {
          return pos_ == pos;
        }

        bool operator!=(std::vector<bdd>::const_iterator pos) const
        {
          return pos_ != pos;
        }
      };

      iterator begin() const
      {
        return iterator(labels_, cond_);
      }

      std::vector<bdd>::const_iterator end() const
      {
        return labels_.end();
      }

    private:
      const std::vector<bdd>& labels_;
      bdd cond_;
    };
#endif


  /// \ingroup twa_misc
  /// \brief separate edges so that their labels are disjoint
  ///
  /// To use this class, first call add_to_basis() for each label that
  /// you want to separate.  Then call separate() to get a new
  /// automaton.
  ///
  /// Note that all labels seen by separate() should have been
  /// previously declared using add_to_basis(), but more can be
  /// declared.
  ///
  /// For instance an automaton has labels in {a,b,!a&!b&c} and those
  /// are used as basis, the separated automaton should have its
  /// labels among {a&!b,a&b,!a&b,!a&!b&c}.
  class SPOT_API edge_separator
  {
  public:
    /// \brief add label(s) to a basis
    ///
    /// Add a single label, or all the labels of an automaton.
    ///
    /// The version that takes an automaton will also record the
    /// atomic propositions used by the automaton.  Those atomic
    /// propositions will be registered by separate_implying() or
    /// separate_compat().  If you call the BDD version of
    /// add_to_basis() and add a new atomic proposition, you should
    /// remember to register it in the result of separate_implying()
    /// or separate_compat() yourself.
    ///
    /// If \a max_label is given, at most \a max_label unique labels
    /// are added to the basis.  False is returned iff the automaton
    /// used more labels.
    /// @{
    void add_to_basis(bdd label);
    void add_to_basis(const const_twa_graph_ptr& aut);
    bool add_to_basis(const const_twa_graph_ptr& aut,
                      unsigned long max_label);
    /// @}
    /// \brief Separate an automaton
    ///
    /// This variant replaces each edge labeled by L by an edge
    /// for each label of the basis that is implies L.  This
    /// faster than separate_compat when all edges of aut have
    /// been declared in the basis.
    twa_graph_ptr separate_implying(const const_twa_graph_ptr& aut);
    /// \brief Separate an automaton
    ///
    /// This variant replaces each edge labeled by L by an edge for
    /// each label of the basis that compatible implies L.  This
    /// faster than separate_compat when all edges of aut have been
    /// declared in the basis.
    twa_graph_ptr separate_compat(const const_twa_graph_ptr& aut);
#ifndef SWIG
    /// \brief Separate a label
    ///
    /// This returns a pseudo-container that can be used to iterate
    /// over the elements of the basis that imply the current label.
    ///
    /// For instance if the basis was created from {a,b} (i.e., the
    /// basis is actually {!a&!b,a&!b,!a&b,a&b}), and the label is
    /// {a}, the result will be {a&!b,a&b}.
    edge_separator_filter<true> separate_implying(bdd label)
    {
      return {basis_, label};
    }
    /// \brief Separate a label
    ///
    /// This returns a pseudo-container that can be used to iterate
    /// over the elements of the basis compatible with the current
    /// label.
    ///
    /// For instance if the basis was created from {a,b} (i.e., the
    /// basis is actually {!a&!b,a&!b,!a&b,a&b}), and the label is
    /// {c&a}, the result will be {a&!b&c,a&b&c}.
    edge_separator_filter<false> separate_compat(bdd label)
    {
      return {basis_, label};
    }
#endif

    unsigned basis_size() const
    {
      return basis_.size();
    }

    const std::vector<bdd>& basis() const
    {
      return basis_;
    }

  private:
    std::vector<bdd> basis_{bddtrue};
    std::set<formula> aps_;
  };

  /// \ingroup twa_misc
  /// \brief Make edge labels disjoints
  ///
  /// Create a new version of the automaton where all edges are split
  /// in such a way that two labels are either equal or disjoint.
  ///
  /// For instance if the automaton uses only {a,b,!a&!b&c} as labels,
  /// the result should have label among {a&!b,a&b,!a&b,!a&!b&c}.
  ///
  /// Using split_edges() also creates an automaton with separated labels,
  /// but the separation will be much finer since it will result in a much
  /// involves all atomic proposition.
  SPOT_API twa_graph_ptr separate_edges(const const_twa_graph_ptr& aut);
}
