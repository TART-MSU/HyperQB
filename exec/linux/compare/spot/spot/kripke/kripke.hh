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

#include <spot/kripke/fairkripke.hh>
#include <spot/twacube/cube.hh>
#include <memory>
#include <type_traits>

namespace spot
{
  /// \ingroup kripke
  /// \brief This class is a template representation of a Kripke
  /// structure. It is composed of two template parameters: State
  /// represents a state of the Kripke structure, SuccIterator is
  /// an iterator over the (possible) successors of a state.
  ///
  /// Do not delete by hand any states and/or iterator that
  /// are provided by this template class. Specialisations
  /// will handle it.
  template<typename State, typename SuccIterator>
  class SPOT_API kripkecube:
    public std::enable_shared_from_this<kripkecube<State, SuccIterator>>
  {
  public:
    /// \brief Returns the initial State of the System. The \a tid parameter
    /// is used internally for sharing this structure among threads.
    State initial(unsigned tid);

    /// \brief Returns the number of threads that are handled by the kripkecube
    unsigned get_threads();

    /// \brief Provides a string representation of the parameter state
    std::string to_string(const State, unsigned tid) const;

    /// \brief Returns an iterator over the successors of the parameter state.
    SuccIterator* succ(const State, unsigned tid);

    /// \brief Allocation and deallocation of iterator is costly. This
    /// method allows to reuse old iterators.
    void recycle(SuccIterator*, unsigned tid);

    /// \brief This method allow to deallocate a given state.
    const std::vector<std::string> ap();
  };

#ifndef SWIG

  /// \ingroup kripke
  /// \brief This class allows to ensure (at compile time) if
  /// a given parameter is of type kripkecube. It also check
  /// if the iterator has the good interface.
  template <typename T, typename State, typename SuccIter>
  class SPOT_API is_a_kripkecube_ptr
  {
  private:
    using yes = std::true_type;
    using no = std::false_type;

    // Hardly waiting C++ concepts...
    template<typename U, typename V> static auto test_kripke(U u, V v)
      -> decltype(
       // Check the kripke
       std::is_same<State,       decltype(u->initial(0))>::value            &&
       std::is_same<unsigned,    decltype(u->get_threads())>::value         &&
       std::is_same<std::string, decltype(u->to_string(State(), 0))>::value &&
       std::is_same<SuccIter*,   decltype(u->succ(State(), 0))>::value      &&
       std::is_same<void,        decltype(u->recycle(nullptr, 0))>::value   &&
       std::is_same<const std::vector<std::string>,
                                 decltype(u->ap())>::value                  &&
       std::is_same<void,        decltype(u->recycle(nullptr, 0))>::value   &&

       // Check the SuccIterator
       std::is_same<void,        decltype(v->next())>::value                &&
       std::is_same<bool,        decltype(v->done())>::value                &&
       std::is_same<State,       decltype(v->state())>::value               &&
       std::is_same<spot::cube,  decltype(v->condition())>::value

      // finally return the type "yes"
      , yes());

    // For all other cases return the type "no"
    template<typename, typename> static no test_kripke(...);

  public:

    /// \brief Checking this value will ensure, at compile time, that the
    /// Kripke specialisation respects the required interface.
    static constexpr bool value =
      std::is_same< decltype(test_kripke<T, SuccIter*>(nullptr, nullptr)), yes
                  >::value;
  };

#endif


  /// \ingroup kripke
  /// \brief Iterator code for Kripke structure
  ///
  /// This iterator can be used to simplify the writing
  /// of an iterator on a Kripke structure (or lookalike).
  ///
  /// If you inherit from this iterator, you should only
  /// redefine
  ///
  ///   - kripke_succ_iterator::first()
  ///   - kripke_succ_iterator::next()
  ///   - kripke_succ_iterator::done()
  ///   - kripke_succ_iterator::dst()
  ///
  /// This class implements kripke_succ_iterator::cond(),
  /// and kripke_succ_iterator::acc().
  class SPOT_API kripke_succ_iterator : public twa_succ_iterator
  {
  public:
    /// \brief Constructor
    ///
    /// The \a cond argument will be the one returned
    /// by kripke_succ_iterator::cond().
    kripke_succ_iterator(const bdd& cond)
      : cond_(cond)
    {
    }

    void recycle(const bdd& cond)
    {
      cond_ = cond;
    }

    virtual ~kripke_succ_iterator();

    virtual bdd cond() const override;
    virtual acc_cond::mark_t acc() const override;
  protected:
    bdd cond_;
  };

  /// \ingroup kripke
  /// \brief Interface for a Kripke structure
  ///
  /// A Kripke structure is a graph in which each node (=state) is
  /// labeled by a conjunction of atomic proposition.
  ///
  /// Such a structure can be seen as spot::tgba without
  /// any acceptance condition.
  ///
  /// A programmer that develops an instance of Kripke structure needs
  /// just provide an implementation for the following methods:
  ///
  ///   - kripke::get_init_state()
  ///   - kripke::succ_iter()
  ///   - kripke::state_condition()
  ///   - kripke::format_state()
  ///
  /// The other methods of the tgba interface (like those dealing with
  /// acceptance conditions) are supplied by this kripke class and
  /// need not be defined.
  ///
  /// See also spot::kripke_succ_iterator.
  class SPOT_API kripke: public fair_kripke
  {
  public:
    kripke(const bdd_dict_ptr& d)
      : fair_kripke(d)
      {
        prop_weak(true);
      }

    virtual ~kripke();

    virtual
      acc_cond::mark_t state_acceptance_mark(const state*) const override;
  };

  typedef std::shared_ptr<kripke> kripke_ptr;
  typedef std::shared_ptr<const kripke> const_kripke_ptr;
}
