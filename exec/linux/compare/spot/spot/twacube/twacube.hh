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

#include <vector>
#include <iosfwd>
#include <spot/graph/graph.hh>
#include <spot/misc/hash.hh>
#include <spot/twa/acc.hh>
#include <spot/twacube/cube.hh>
#include <spot/twacube/fwd.hh>

namespace spot
{
  /// \brief Class for thread-safe states.
  class SPOT_API cstate
  {
  public:
    cstate() = default;
    cstate(const cstate& s) = delete;
    cstate(cstate&& s) noexcept;
    ~cstate() = default;
  };

  /// \brief Class for representing a transition.
  class SPOT_API transition
  {
  public:
    transition() = default;
    transition(const transition& t) = delete;
    transition(transition&& t) noexcept;
    transition(const cube& cube, acc_cond::mark_t acc);
    ~transition() = default;

    cube cube_;
    acc_cond::mark_t acc_;
  };

  /// \brief Class for iterators over transitions
  class SPOT_API trans_index final:
    public std::enable_shared_from_this<trans_index>
  {
  public:
    typedef digraph<cstate, transition> graph_t;
    typedef graph_t::edge_storage_t edge_storage_t;

    trans_index(trans_index& ci) = delete;
    trans_index(unsigned state, const graph_t& g):
      st_(g.state_storage(state))
    {
      reset();
    }

    trans_index(trans_index&& ci):
      idx_(ci.idx_),
      st_(ci.st_)
    {
    }

    /// Reset the iterator on the first element.
    inline void reset()
    {
      idx_ = st_.succ;
    }

    /// \brief Iterate over the next transition.
    inline void next()
    {
      ++idx_;
    }

    /// \brief Returns a boolean indicating wether all the transitions
    /// have been iterated.
    inline bool done() const
    {
      return !idx_ || idx_ > st_.succ_tail;
    }

    /// \brief Returns the current transition according to a specific
    /// \a seed. The \a  seed is traditionally the thread identifier.
    inline unsigned current(unsigned seed = 0) const
    {
      // no-swarming : since twacube are dedicated for parallelism, i.e.
      // swarming, we expect swarming is activated.
      if (SPOT_UNLIKELY(!seed))
        return idx_;
      // Here swarming performs a technique called "primitive
      // root modulo n", i. e.  for i in [1..n]: i*seed (mod n). We
      // also must have seed prime with n: to solve this, we use
      // precomputed primes and seed access one of this primes. Note
      // that the chosen prime must be greater than n.
      SPOT_ASSERT(primes[seed] > (st_.succ_tail-st_.succ+1));
      unsigned long long c = (idx_-st_.succ) + 1;
      unsigned long long p = primes[seed];
      unsigned long long s = (st_.succ_tail-st_.succ+1);
      return (unsigned)  (((c*p) % s)+st_.succ);
    }

  private:
    unsigned idx_;                   ///< The current transition
    const graph_t::state_storage_t& st_; ///< The underlying states
  };

  /// \brief Class for representing a thread-safe twa.
  class SPOT_API twacube final: public std::enable_shared_from_this<twacube>
  {
  public:
    twacube() = delete;

    /// \brief Build a new automaton from a list of atomic propositions.
    twacube(const std::vector<std::string> aps);

    virtual ~twacube();

    /// \brief Returns the acceptance condition associated to the automaton.
    acc_cond& acc();

    /// \brief Returns the names of the atomic properties.
    std::vector<std::string> ap() const;

    /// \brief This method creates a new state.
    unsigned new_state();

    /// \brief Updates the initial state to \a init
    void set_initial(unsigned init);

    /// \brief Returns the id of the initial state in the automaton.
    unsigned get_initial() const;

    /// \brief Accessor for a state from its id.
    cstate* state_from_int(unsigned i);

    /// \brief create a transition between state \a src and state \a dst,
    /// using \a cube as the labelling cube and \a mark as the acceptance mark.
    void create_transition(unsigned src,
                           const cube& cube,
                           const acc_cond::mark_t& mark,
                           unsigned dst);

    /// \brief Accessor the cube's manipulator.
    const cubeset& get_cubeset() const;

    /// \brief Check if all the successors of a state are located contiguously
    /// in memory. This is mandatory for swarming techniques.
    bool succ_contiguous() const;

    unsigned num_states() const
    {
      return theg_.num_states();
    }

    unsigned num_edges() const
    {
      return theg_.num_edges();
    }

    typedef digraph<cstate, transition> graph_t;

    /// \brief Returns the underlying graph for this automaton.
    const graph_t& get_graph()
    {
      return theg_;
    }
    typedef graph_t::edge_storage_t edge_storage_t;

    /// \brief Returns the storage associated to a transition.
    const graph_t::edge_storage_t&
    trans_storage(std::shared_ptr<trans_index> ci,
                  unsigned seed = 0) const
    {
      return theg_.edge_storage(ci->current(seed));
    }

    /// \brief Returns the data associated to a transition.
    const transition& trans_data(std::shared_ptr<trans_index> ci,
                                 unsigned seed = 0) const
    {
      return theg_.edge_data(ci->current(seed));
    }

    ///< \brief Returns the successor of state \a i.
    std::shared_ptr<trans_index> succ(unsigned i) const
    {
      return std::make_shared<trans_index>(i, theg_);
    }

    friend SPOT_API std::ostream& operator<<(std::ostream& os,
                                             const twacube& twa);
  private:
    unsigned init_;                  ///< The Id of the initial state
    acc_cond acc_;                       ///< The acceptance condition
    const std::vector<std::string> aps_; ///< The name of atomic propositions
    graph_t theg_;                       ///< The underlying graph
    cubeset cubeset_;                    ///< Ease the cube manipulation
  };

  inline twacube_ptr make_twacube(const std::vector<std::string> aps)
  {
    return std::make_shared<twacube>(aps);
  }
}
