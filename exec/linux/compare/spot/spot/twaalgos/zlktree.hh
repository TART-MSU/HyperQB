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
#include <deque>
#include <memory>
#include <spot/misc/bitvect.hh>
#include <spot/twa/twagraph.hh>
#include <spot/twaalgos/sccinfo.hh>

namespace spot
{
  /// \ingroup twa_acc_transform
  /// \brief Options to alter the behavior of acd
  enum class zielonka_tree_options
  {
    /// Build the ZlkTree, without checking its shape.
    NONE = 0,
    /// Check if the ZlkTree has Rabin shape.
    /// This actually has no effect unless ABORT_WRONG_SHAPE is set,
    /// because zielonka_tree always check the shape.
    CHECK_RABIN = 1,
    /// Check if the ZlkTree has Streett shape.
    /// This actually has no effect unless ABORT_WRONG_SHAPE is set,
    /// because zielonka_tree always check the shape.
    CHECK_STREETT = 2,
    /// Check if the ZlkTree has Parity shape
    /// This actually has no effect unless ABORT_WRONG_SHAPE is set,
    /// because zielonka_tree always check the shape.
    CHECK_PARITY = CHECK_RABIN | CHECK_STREETT,
    /// Abort the construction of the ZlkTree if it does not have the
    /// shape that is tested.  When that happens, num_branches() is set
    /// to 0.
    ABORT_WRONG_SHAPE = 4,
    /// Fuse identical substree.  This cannot be used with
    /// zielonka_tree_transform().  However it saves memory if the
    /// only use of the zielonka_tree to check the shape.
    MERGE_SUBTREES = 8,
  };

#ifndef SWIG
  inline
  bool operator!(zielonka_tree_options me)
  {
    return me == zielonka_tree_options::NONE;
  }

  inline
  zielonka_tree_options operator&(zielonka_tree_options left,
                                  zielonka_tree_options right)
  {
    typedef std::underlying_type_t<zielonka_tree_options> ut;
    return static_cast<zielonka_tree_options>(static_cast<ut>(left)
                                              & static_cast<ut>(right));
  }

  inline
  zielonka_tree_options operator|(zielonka_tree_options left,
                                  zielonka_tree_options right)
  {
    typedef std::underlying_type_t<zielonka_tree_options> ut;
    return static_cast<zielonka_tree_options>(static_cast<ut>(left)
                                              | static_cast<ut>(right));
  }

  inline
  zielonka_tree_options operator-(zielonka_tree_options left,
                                  zielonka_tree_options right)
  {
    typedef std::underlying_type_t<zielonka_tree_options> ut;
    return static_cast<zielonka_tree_options>(static_cast<ut>(left)
                                              & ~static_cast<ut>(right));
  }
#endif
  /// \ingroup twa_acc_transform
  /// \brief Zielonka Tree implementation
  ///
  /// This class implements a Zielonka Tree, using
  /// conventions similar to those in \cite casares.21.icalp
  ///
  /// The differences is that this tree is built from Emerson-Lei
  /// acceptance conditions, and can be "walked through" with multiple
  /// colors at once.
  class SPOT_API zielonka_tree
  {
  public:
    /// \brief Build a Zielonka tree from the acceptance condition.
    zielonka_tree(const acc_cond& cond,
                  zielonka_tree_options opt = zielonka_tree_options::NONE);

    /// \brief The number of branches in the Zielonka tree.
    ///
    /// Branch are designated by the node number of their
    /// leaves.
    unsigned num_branches() const
    {
      return num_branches_;
    }

    /// \brief The number of one branch in the tree.
    ///
    /// This returns the branch whose leave is the smallest one.
    unsigned first_branch() const
    {
      return one_branch_;
    }

    /// \brief Walk through the Zielonka tree.
    ///
    /// Given a \a branch number, and a set of \a colors, this returns
    /// a pair (new branch, level), as needed in definition 3.3 of
    /// \cite casares.21.icalp (or definition 3.7 in the full version).
    ///
    /// The level correspond to the priority of a minimum parity acceptance
    /// condition, with the parity odd/even as specified by is_even().
    ///
    /// This implementation is slightly different from the original
    /// definition since it allows a set of colors to be processed,
    /// and not exactly one color.  When multiple colors are given,
    /// the minimum corresponding level is returned.  When no color is
    /// given, the branch is not changed and the level returned might
    /// be one more than the depth of that branch (as if the tree had
    /// another layer of leaves labeled by the empty sets, that we
    /// do not store).  Whether the depth for no color is the depth
    /// of the node or one more depend on whether the absence of
    /// color had the same parity has the current node.
    std::pair<unsigned, unsigned>
    step(unsigned branch, acc_cond::mark_t colors) const;

    /// \brief Whether the tree corresponds to a min even or min odd
    /// parity acceptance.
    bool is_even() const
    {
      return is_even_;
    }

    /// \brief Whether the Zielonka tree has Rabin shape.
    ///
    /// The tree has Rabin shape of all accepting (round) nodes have
    /// at most one child.
    bool has_rabin_shape() const
    {
      return has_rabin_shape_;
    }

    /// \brief Whether the Zielonka tree has Streett shape.
    ///
    /// The tree has Streett shape of all rejecting (square) nodes have
    /// at most one child.
    bool has_streett_shape() const
    {
      return has_streett_shape_;
    }

    /// \brief Whether the Zielonka tree has parity shape.
    ///
    /// The tree has parity shape of all nodes have at most one child.
    bool has_parity_shape() const
    {
      return has_streett_shape_ && has_rabin_shape_;
    }

    /// \brief Render the tree as in GraphViz format.
    void dot(std::ostream&) const;

    struct zielonka_node
    {
      unsigned parent;
      unsigned next_sibling = 0;
      unsigned first_child = 0;
      unsigned level;
      acc_cond::mark_t colors;
    };
    std::vector<zielonka_node> nodes_;
  private:
    unsigned one_branch_ = 0;
    unsigned num_branches_ = 0;
    bool is_even_;
    bool empty_is_even_;
    bool has_rabin_shape_ = true;
    bool has_streett_shape_ = true;
  };

  /// \ingroup twa_acc_transform
  /// \brief Paritize an automaton using Zielonka tree.
  ///
  /// This corresponds to the application of Section 3 of
  /// \cite casares.21.icalp
  ///
  /// The resulting automaton has a parity acceptance that is either
  /// "min odd" or "min even", depending on the original acceptance.
  /// It may uses up to n+1 colors if the input automaton has n
  /// colors.  Finally, it is colored, i.e., each output transition
  /// has exactly one color.
  SPOT_API
  twa_graph_ptr zielonka_tree_transform(const const_twa_graph_ptr& aut);


  /// \ingroup twa_acc_transform
  /// \brief Options to alter the behavior of acd
  enum class acd_options
  {
    /// Build the ACD, without checking its shape.
    NONE = 0,
    /// Check if the ACD has Rabin shape.
    CHECK_RABIN = 1,
    /// Check if the ACD has Streett shape.
    CHECK_STREETT = 2,
    /// Check if the ACD has Parity shape
    CHECK_PARITY = CHECK_RABIN | CHECK_STREETT,
    /// Abort the construction of the ACD if it does not have the
    /// shape that is tested.  When that happens, node_count() is set
    /// to 0.
    ABORT_WRONG_SHAPE = 4,
    /// Order the children of a node by decreasing size of the number
    /// of states they would introduce if that child appears as the
    /// last child of an "ACD" round in the state-based version of the
    /// ACD output.
    ORDER_HEURISTIC = 8,
  };

#ifndef SWIG
  inline
  bool operator!(acd_options me)
  {
    return me == acd_options::NONE;
  }

  inline
  acd_options operator&(acd_options left, acd_options right)
  {
    typedef std::underlying_type_t<acd_options> ut;
    return static_cast<acd_options>(static_cast<ut>(left)
                                    & static_cast<ut>(right));
  }

  inline
  acd_options operator|(acd_options left, acd_options right)
  {
    typedef std::underlying_type_t<acd_options> ut;
    return static_cast<acd_options>(static_cast<ut>(left)
                                    | static_cast<ut>(right));
  }

  inline
  acd_options operator-(acd_options left, acd_options right)
  {
    typedef std::underlying_type_t<acd_options> ut;
    return static_cast<acd_options>(static_cast<ut>(left)
                                    & ~static_cast<ut>(right));
  }

#endif

  /// \ingroup twa_acc_transform
  /// \brief Alternating Cycle Decomposition implementation
  ///
  /// This class implements an Alternating Cycle Decomposition
  /// similar to what is described in \cite casares.21.icalp
  ///
  /// The differences is that this ACD is built from Emerson-Lei
  /// acceptance conditions, and can be "walked through" with multiple
  /// colors at once.
  class SPOT_API acd
  {
  public:
    /// \brief Build a Alternating Cycle Decomposition an SCC decomposition
    acd(const scc_info& si, acd_options opt = acd_options::NONE);
    acd(const const_twa_graph_ptr& aut, acd_options opt = acd_options::NONE);

    ~acd();

    /// \brief Step through the ACD.
    ///
    /// Given a \a branch number, and an edge, this returns
    /// a pair (new branch, level), as needed in definition 4.6 of
    /// \cite casares.21.icalp (or definition 4.20 in the full version).
    ///
    /// The level correspond to the priority of a minimum parity acceptance
    /// condition, with the parity odd/even as specified by is_even().
    std::pair<unsigned, unsigned>
    step(unsigned branch, unsigned edge) const;

    /// \brief Step through the ACD, with rules for state-based output.
    ///
    /// Given a \a node number, and an edge, this returns
    /// the new node to associate to the destination state.  This
    /// node is not necessarily a leave, and its level should be
    /// the level for the output state.
    unsigned state_step(unsigned node, unsigned edge) const;

    /// \brief Return the list of edges covered by node n of the ACD.
    ///
    /// This is mostly used for interactive display.
    std::vector<unsigned> edges_of_node(unsigned n) const;

    /// \brief Return the number of nodes in the the ACD forest.
    unsigned node_count() const
    {
      return nodes_.size();
    }

    /// Tell whether a node store accepting or rejecting cycles.
    ///
    /// This is mostly used for interactive display.
    bool node_acceptance(unsigned n) const;

    /// Return the level of a node.
    unsigned node_level(unsigned n) const;

    /// Return the colors of a node.
    const acc_cond::mark_t& node_colors(unsigned n) const;

    /// \brief Whether the ACD corresponds to a min even or min odd
    /// parity acceptance in SCC \a scc.
    bool is_even(unsigned scc) const
    {
      if (scc >= scc_count_)
        report_invalid_scc_number(scc, "is_even");
      return trees_[scc].is_even;
    }

    /// \brief Whether the ACD globally corresponds to a min even or
    /// min odd parity acceptance.
    ///
    /// The choice between even or odd is determined by the parity
    /// of the tallest tree of the ACD.  In case two tree of opposite
    /// parity share the tallest height, then even parity is favored.
    bool is_even() const
    {
      return is_even_;
    }

    /// \brief Return the first branch for \a state
    unsigned first_branch(unsigned state) const;

    unsigned scc_max_level(unsigned scc) const
    {
      if (scc >= scc_count_)
        report_invalid_scc_number(scc, "scc_max_level");
      return trees_[scc].max_level;
    }

    /// \brief Whether the ACD has Rabin shape.
    ///
    /// The ACD has Rabin shape if all accepting (round) nodes have no
    /// children with a state in common.  The acd should be built with
    /// option CHECK_RABIN or CHECK_PARITY for this function to work.
    bool has_rabin_shape() const;

    /// \brief Whether the ACD has Streett shape.
    ///
    /// The ACD has Streett shape if all rejecting (square) nodes have no
    /// children with a state in common.  The acd should be built with
    /// option CHECK_STREETT or CHECK_PARITY for this function to work.
    bool has_streett_shape() const;

    /// \brief Whether the ACD has Streett shape.
    ///
    /// The ACD has Streett shape if all nodes have no
    /// children with a state in common.  The acd should be built with
    /// option CHECK_PARITY for this function to work.
    bool has_parity_shape() const;

    /// \brief Return the automaton on which the ACD is defined.
    const const_twa_graph_ptr get_aut() const
    {
      return aut_;
    }

    /// \brief Render the ACD as in GraphViz format.
    ///
    /// If \a id is given, it is used to give the graph an id, and,
    /// all nodes will get ids as well.
    void dot(std::ostream&, const char* id = nullptr) const;

  private:
    const scc_info* si_;
    bool own_si_ = false;
    acd_options opt_;

    // This structure is used to represent one node in the ACD forest.
    // The tree use a left-child / right-sibling representation
    // (called here first_child, next_sibling).  Each node
    // additionally store a level (depth in the ACD, adjusted at the
    // end of the construction so that all node on the same level have
    // the same parity), the SCC (which is also it's tree number), and
    // some bit vectors representing the edges and states of that
    // node.  Those bit vectors are as large as the original
    // automaton, and they are shared among nodes from the different
    // trees of the ACD forest (since each tree correspond to a
    // different SCC, they cannot share state or edges).
    struct acd_node
    {
      unsigned parent;
      unsigned next_sibling = 0;
      unsigned first_child = 0;
      unsigned level;
      unsigned scc;
      acc_cond::mark_t colors;
      unsigned minstate;
      bitvect& edges;
      bitvect& states;
      acd_node(bitvect& e, bitvect& s) noexcept
        : edges(e), states(s)
      {
      }
    };
    // We store the nodes in a deque so that their addresses do not
    // change.
    std::deque<acd_node> nodes_;
    // Likewise for bitvectors: this is the support for all edge vectors
    // and state vectors used in acd_node.
    std::deque<std::unique_ptr<bitvect>> bitvectors;
    // Information about a tree of the ACD.  Each treinserte correspond
    // to an SCC.
    struct scc_data
    {
      bool trivial;             // whether the SCC is trivial we do
                                // not store any node for trivial
                                // SCCs.
      unsigned root = 0;        // root node of a non-trivial SCC.
      bool is_even;             // parity of the tree, used at the end
                                // of the construction to adjust
                                // levels.
      unsigned max_level = 0;   // Maximum level for this SCC.
      unsigned num_nodes = 0;   // Number of node in this tree.  This
                                // is only used to share bitvectors
                                // between SCC: node with the same
                                // "rank" in each tree share the same
                                // bitvectors.
    };
    std::vector<scc_data> trees_;
    unsigned scc_count_;
    const_twa_graph_ptr aut_;
    // Information about the overall ACD.
    bool is_even_;
    bool has_rabin_shape_ = true;
    bool has_streett_shape_ = true;

    // Build the ACD structure.  Called by the constructors.
    void build_();

    // leftmost branch of \a node that contains \a state
    unsigned leftmost_branch_(unsigned node, unsigned state) const;

#ifndef SWIG
    [[noreturn]] static
    void report_invalid_scc_number(unsigned num, const char* fn);
    [[noreturn]] static void report_need_opt(const char* opt);
    [[noreturn]] static void report_empty_acd(const char* fn);
#endif
  };

  /// \ingroup twa_acc_transform
  /// \brief Paritize an automaton using ACD.
  ///
  /// This corresponds to the application of Section 4 of
  /// \cite casares.21.icalp
  ///
  /// The resulting automaton has a parity acceptance that is either
  /// "min odd" or "min even", depending on the original acceptance.
  ///
  /// If \a colored is set, each output transition will have exactly
  /// one color, and the output automaton will use at most n+1 colors
  /// if the input has n colors. If \a colored is unset (the default),
  /// output transitions will use at most one color, and output
  /// automaton will use at most n colors.
  ///
  /// The acd_transform() is the original function producing
  /// optimal transition-based output (optimal in the sense of least
  /// number of duplicated states), while the acd_transform_sbacc() variant
  /// produces state-based output from transition-based input and without
  /// any optimality claim.  The \a order_heuristics argument, enabled
  /// by default activates the ORDER_HEURISTICS option of the ACD.
  /// @{
  SPOT_API
  twa_graph_ptr acd_transform(const const_twa_graph_ptr& aut,
                              bool colored = false);
  SPOT_API
  twa_graph_ptr acd_transform_sbacc(const const_twa_graph_ptr& aut,
                                    bool colored = false,
                                    bool order_heuristic = true);
  /// @}
}
