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
#include <spot/misc/bddlt.hh>
#include <spot/twa/fwd.hh>
#include <spot/twa/bdddict.hh>
#include <spot/tl/formula.hh>
#include <spot/tl/apcollect.hh>

#include <unordered_map>
#include <vector>
#include <set>
#include <memory>
#include <algorithm> // std::none_of
#include <sstream>


namespace spot
{
  // Forward for synthesis
  struct mealy_like;

  class aig;

  typedef std::shared_ptr<aig> aig_ptr;
  typedef std::shared_ptr<const aig> const_aig_ptr;

  /// \ingroup synthesis
  /// \brief A class representing AIG circuits
  ///
  /// AIG circuits consist of (named) inputs, (named) outputs, latches which
  /// serve as memory, and gates and negations connecting them.
  /// AIG circuits can be used to represent controllers, which is currently
  /// their sole purpose within spot.
  /// AIGs produce a output sequence based on the following rules:
  /// 1) All latches are initialized to 0
  /// 2) The next input is read.
  /// 3) The output and the state of the latches for the next turn
  ///    are given by the gates as a function of the current latches and inputs
  class SPOT_API aig
  {
  protected:
    const unsigned num_inputs_;
    const unsigned num_outputs_;
    const unsigned num_latches_;
    const std::vector<std::string> input_names_;
    const std::vector<std::string> output_names_;
    unsigned max_var_;

    std::vector<unsigned> next_latches_;
    std::vector<unsigned> outputs_;
    std::vector<std::pair<unsigned, unsigned>> and_gates_;
    bdd_dict_ptr dict_;
    // Cache the function computed by each variable as a bdd.
    // Bidirectional map
    std::unordered_map<unsigned, bdd> var2bdd_;
    std::unordered_map<int, unsigned> bdd2var_; //uses id
    // First anonymous var marking the beginning of variables used
    // as latches
    int l0_;

    bdd all_ins_;
    bdd all_latches_;

    // For simulation
    std::vector<bool> state_;

  public:

    /// \brief Mark the beginning of a test translation
    ///
    /// Sometimes different encodings produces more or less gates.
    /// To improve performances, one can "safe" the current status
    ///  and revert changes afterwards if needed
    using safe_point = std::pair<unsigned, unsigned>;
    using safe_stash =
            std::tuple<std::vector<std::pair<unsigned, unsigned>>,
                       std::vector<std::pair<unsigned, bdd>>,
                       std::vector<bdd>>;

    /// \brief Constructing an "empty" aig, knowing only about the
    ///        necessary inputs, outputs and latches. A bdd_dict can
    ///        be handed to the circuit in order to allow for verification
    ///        against other automata after construction
    aig(const std::vector<std::string>& inputs,
        const std::vector<std::string>& outputs,
        unsigned num_latches,
        bdd_dict_ptr dict = make_bdd_dict());

    /// \brief Constructing the circuit with generic names.
    aig(unsigned num_inputs, unsigned num_outputs,
        unsigned num_latches, bdd_dict_ptr dict = make_bdd_dict());

    ~aig()
    {
      dict_->unregister_all_my_variables(this);
    }

  protected:
    /// \brief Register a new literal in both maps
    void register_new_lit_(unsigned v, const bdd &b);
    void register_latch_(unsigned i, const bdd& b);
    void register_input_(unsigned i, const bdd& b);
    /// \brief Remove a literal from both maps
    void unregister_lit_(unsigned v);

    /// \brief Internal function that split a bdd into a conjunction
    ///        hoping to increase reusage of gates
    void split_cond_(const bdd& b, char so_mode,
                     std::vector<bdd>& cond_parts);

    /// \brief Split-off common sub-expressions as cube
    bdd accum_common_(const bdd& b) const;

    /// Translate a cube into gates, using split-off optionally
    unsigned cube2var_(const bdd& b, const int use_split_off);

  public:

    /// \brief Safe the current state of the circuit
    /// \note This does not make a copy, so rolling back to
    ///       an older safe point invalidates all newer safepoints.
    ///       Also only concerns the gates, output and next_latch variables
    ///       will not change
    ///       This function is semi-public. Make sure you know what you are
    ///       doing when using it.
    safe_point get_safe_point_() const;

    /// \brief roll_back to the saved point.
    ///
    /// \param sp The safe_point to revert back to
    /// \param do_stash Whether or not to save the changes to be possibly
    ///                   reapplied later on
    /// \note This function is semi-public. Make sure you know what you are
    ///        doing when using it.
    safe_stash roll_back_(safe_point sp,
                          bool do_stash = false);
    /// \brief Reapply to stored changes on top of a safe_point
    /// \note \a ss has to be obtained from `roll_back_(sp, true)`
    ///       This function is semi-public. Make sure you know what you are
    ///       doing when using it.
    void reapply_(safe_point sp, const safe_stash& ss);

    /// \brief Get the number of outputs
    unsigned num_outputs() const
    {
      return num_outputs_;
    }
    /// \brief Get the variables associated to the outputs
    /// \note Only available after a call to aig::set_output
    const std::vector<unsigned>& outputs() const
    {
      SPOT_ASSERT(std::none_of(outputs_.begin(), outputs_.end(),
                               [](unsigned o){return o == -1u; }));
      return outputs_;
    }

    /// \brief return the variable associated to output \a num
    ///
    /// This will be equal to -1U if aig::set_output() hasn't been called.
    unsigned output(unsigned num) const
    {
      return outputs_[num];
    }

    /// \brief Get the set of output names
    const std::vector<std::string>& output_names() const
    {
      return output_names_;
    }

    /// \brief Get the number of inputs
    unsigned num_inputs() const
    {
      return num_inputs_;
    }
    /// \brief Get the set of input names
    const std::vector<std::string>& input_names() const
    {
      return input_names_;
    }

    /// \brief Get the number of latches in the circuit
    unsigned num_latches() const
    {
      return num_latches_;
    }
    /// \brief Get the variables associated to the state of the latches
    ///        in the next iteration
    /// \note Only available after call to aig::set_next_latch
    const std::vector<unsigned>& next_latches() const
    {
      SPOT_ASSERT(std::none_of(next_latches_.begin(), next_latches_.end(),
                               [](unsigned o){return o == -1u; }));
      return next_latches_;
    };

    /// \brief Get the total number of and gates
    unsigned num_gates() const
    {
      return and_gates_.size();
    };
    /// \brief Access the underlying container
    const std::vector<std::pair<unsigned, unsigned>>& gates() const
    {
      return and_gates_;
    };

    /// \brief Maximal variable index currently appearing in the circuit
    unsigned max_var() const
    {
      return max_var_;
    };

    /// \brief Get the variable associated to the ith input
    unsigned input_var(unsigned i, bool neg = false) const
    {
      SPOT_ASSERT(i < num_inputs_);
      return (1 + i) * 2 + neg;
    }
    /// \brief Get the bdd associated to the ith input
    bdd input_bdd(unsigned i, bool neg = false) const
    {
      return aigvar2bdd(input_var(i, neg));
    }

    /// \brief Get the variable associated to the ith latch
    unsigned latch_var(unsigned i, bool neg = false) const
    {
      SPOT_ASSERT(i < num_latches_);
      return (1 + num_inputs_ + i) * 2 + neg;
    }
    /// \brief Get the bdd associated to the ith latch
    bdd latch_bdd(unsigned i, bool neg = false) const
    {
      return aigvar2bdd(latch_var(i, neg));
    }

    /// \brief Get the variable associated to the ith gate
    unsigned gate_var(unsigned i, bool neg = false) const
    {
      SPOT_ASSERT(i < num_gates());
      return (1 + num_inputs_ + num_latches_ + i) * 2 + neg;
    }
    /// \brief Get the bdd associated to the ith gate
    bdd gate_bdd(unsigned i, bool neg = false) const
    {
      return aigvar2bdd(gate_var(i, neg));
    }

    /// \brief Get the bdd associated to a variable
    /// \note Throws if non-existent
    bdd aigvar2bdd(unsigned v, bool neg = false) const
    {
      return neg ? bdd_not(var2bdd_.at(v)) : var2bdd_.at(v);
    }

    /// \brief Get the variable associated to a bdd
    /// \note Throws if non-existent
    unsigned bdd2aigvar(const bdd& b) const
    {
      return bdd2var_.at(b.id());
    }

    /// \brief Add a bdd to the circuit using if-then-else normal form
    unsigned bdd2INFvar(const bdd& b);

    /// \brief Add a bdd to the circuit using isop normal form
    unsigned bdd2ISOPvar(const bdd& b, const int use_split_off = 0);

    /// \brief Add a bdd to the circuit
    /// Assumes that all bdd's given in c_alt fulfill the same purpose,
    /// that is, any of these conditions can be encoded and the
    /// corresponding literal returned.
    /// Multiple translation options are available whose main
    /// goal is to minimize the necessary number of gates.
    ///
    /// \param method How to translate the bdd. 0: If-then-else normal form,
    ///               1: isop normal form, 2: try both and retain smaller
    /// \param use_dual Encode the negations of the given bdds and
    ///                 retain the smallest implementation
    /// \param use_split_off 0: Use base algo
    ///                      1: Separate the different types of input signals
    ///                      (like latches, inputs) to increase gate
    ///                      re-usability and retain smallest circuit
    ///                      2: Actively search for subexpressions the
    ///                      within expressions
    unsigned encode_bdd(const std::vector<bdd>& c_alt,
                        char method = 1, bool use_dual = false,
                        int use_split_off = 0);

    /// \brief Just like the vector version but with no alternatives given
    unsigned encode_bdd(const bdd& b,
                        char method = 1, bool use_dual = false,
                        int use_split_off = 0);

    /// \brief Associate the ith output to the variable v
    void set_output(unsigned i, unsigned v);

    /// \brief Associate the ith latch state after update to the variable v
    void set_next_latch(unsigned i, unsigned v);

    static constexpr unsigned aig_true() noexcept
    {
      return 1;
    };

    static constexpr unsigned aig_false() noexcept
    {
      return 0;
    };

    /// \brief Negate a variable
    unsigned aig_not(unsigned v);

    /// \brief Compute AND of v1 and v2
    unsigned aig_and(unsigned v1, unsigned v2);

    /// \brief Computes the AND of all vars
    /// \note This function modifies the given vector to only contain the
    ///       result (at position zero) after the call
    unsigned aig_and(std::vector<unsigned>& vs);

    /// \brief Computes the OR of v1 and v2
    unsigned aig_or(unsigned v1, unsigned v2);

    /// \brief Computes the or of all vars
    /// \note This function modifies the given vector to only contain the
    ///       result after call
    unsigned aig_or(std::vector<unsigned>& vs);

    /// \brief Returns the positive form of the given variable
    unsigned aig_pos(unsigned v);

    /// \brief Instead of successively adding bdds to the circuit,
    ///        one can also pass a vector of all bdds needed to the circuit.
    ///        In this case additional optimization steps are taken to minimize
    ///        the size of the circuit
    /// \note This can be costly and did not bring about any advantages
    ///       in the SYNTCOMP cases
    void encode_all_bdds(const std::vector<bdd>& all_bdd);

    /// \brief Create a circuit from an aag file with restricted syntax.
    ///
    /// \note Additional constraints are:
    ///       - inputs, latches, and gates have to appear in increasing order.
    ///       - Inputs are expected to have variable numbers from 2-2*n_i+1
    ///         with n_i being the number of inputs
    ///       - Latches cannot be named
    static aig_ptr
    parse_aag(const std::string& aig_file,
              bdd_dict_ptr dict = make_bdd_dict());

    static aig_ptr
    parse_aag(const char* data,
              const std::string& filename,
              bdd_dict_ptr dict = make_bdd_dict());

    static aig_ptr
    parse_aag(std::istream& iss,
              const std::string& filename,
              bdd_dict_ptr dict = make_bdd_dict());

    /// \brief Transform the circuit onto an equivalent monitor
    /// \param keepsplit If as_automaton(true) is the same as
    ///                  split_2step(as_automaton(false), outputs)
    /// \note The complexity is exponential in the number of inputs!
    twa_graph_ptr as_automaton(bool keepsplit = false) const;

    /// \brief Gives access to the current state of the circuit.
    ///
    ///        Corresponds to a vector of booleans holding the truth value
    ///        for each literal. Note that within the vector we have the truth
    ///        value of a literal and its negation, so sim_state()[2*i+1] is
    ///        always the negation of sim_state()[2*i].
    ///        The variable index can be obtained using
    ///        input_var, latch_var or outputs
    const std::vector<bool>& circ_state() const
    {
      SPOT_ASSERT(state_.size() == max_var_ + 2
                  && "State vector does not have the correct size.\n"
                     "Forgot to initialize?");
      return state_;
    }

    /// \brief Access to the state of a specific variable
    bool circ_state_of(unsigned var) const
    {
      SPOT_ASSERT(var <= max_var_ + 1
                  && "Variable out of range");
      return circ_state()[var];
    }

    /// \brief (Re)initialize the stepwise evaluation of the circuit.
    ///        This sets all latches to 0 and clears the output
    void circ_init();

    /// \brief Performs the next discrete step of the circuit,
    ///        based on the inputs.
    ///
    ///        Updates the state of the aig such that circ_state holds the
    ///        values of output and latches AFTER reading the given input
    /// \param inputs : Vector of booleans with size num_inputs()
    void circ_step(const std::vector<bool>& inputs);

  };

  /// \ingroup synthesis
  /// \brief Convert a mealy (like) machine into an aig relying on
  /// the transformation described by \a mode.
  /// \param mode This param has to be of the form
  ///             `ite|isop|both [+dc][+ud][+sub0|+sub1|+sub2]`
  ///             Where `ite` means encoded via if-then-else normal form,
  ///             `isop` means encoded via irredundant sum-of-products,
  ///             `both` means trying both encodings to keep the smaller one.
  ///             `+dc` is optional and tries to take advantage of "do not care"
  ///             outputs to minimize the encoding.
  ///             `+dual` is optional and indicates that the algorithm
  ///             should also try to encode the negation of each condition in
  ///             case its encoding is smaller.
  ///             `+subN` indicates that the conditions can be separated into
  ///             blocks with `sub0` being no separation, `sub1` separation into
  ///             input/latches/gates (`isop` only) and `sub2` tries to seek
  ///             common subformulas.
  ///
  /// If \a ins and \a outs are specified, the named-property
  /// synthesis-output is ignored and all properties in \a ins and \a
  /// outs are guaranteed to appear in the aiger circuit.
  ///
  /// If \a rs is given and is not empty, it can be used to specify how
  /// unused output should be encoded by mapping them to some constant.
  ///@{
  SPOT_API aig_ptr
  mealy_machine_to_aig(const const_twa_graph_ptr& m, const char* mode);
  SPOT_API aig_ptr
  mealy_machine_to_aig(const twa_graph_ptr& m, const char *mode,
                       const std::vector<std::string>& ins,
                       const std::vector<std::string>& outs,
                       const realizability_simplifier* rs = nullptr);

  SPOT_API aig_ptr
  mealy_machine_to_aig(const mealy_like& m, const char* mode);
  SPOT_API aig_ptr
  mealy_machine_to_aig(mealy_like& m, const char *mode,
                       const std::vector<std::string>& ins,
                       const std::vector<std::string>& outs,
                       const realizability_simplifier* rs = nullptr);
  ///@}

  /// \ingroup synthesis
  /// \brief Convert multiple mealy machines into an aig relying on
  ///        the transformation described by mode.
  /// \note The states of each machine are represented by a block of latches
  ///       not affected by the others. For this to work in a general setting,
  ///       the outputs must be disjoint.
  ///
  /// Unless \a ins and \a outs are specified, only the propositions
  /// actually used in the strategy appear in the aiger circuit. So it
  /// can happen that, for instance, propositions marked as output
  /// during the call to ltl_to_game() are absent.
  /// If \a ins and \a outs are used, all properties they list are
  /// guaranteed to appear in the aiger circuit.
  ///
  /// If \a rs is given and is not empty, it can be used to specify how
  /// unused output should be encoded by mapping them to some constant.
  /// @{
  SPOT_API aig_ptr
  mealy_machines_to_aig(const std::vector<const_twa_graph_ptr>& m_vec,
                        const char* mode);
  SPOT_API aig_ptr
  mealy_machines_to_aig(const std::vector<mealy_like>& m_vec,
                        const char* mode);
  SPOT_API aig_ptr
  mealy_machines_to_aig(const std::vector<const_twa_graph_ptr>& m_vec,
                        const char* mode,
                        const std::vector<std::string>& ins,
                        const std::vector<std::vector<std::string>>& outs,
                        const realizability_simplifier* rs = nullptr);
  SPOT_API aig_ptr
  mealy_machines_to_aig(const std::vector<mealy_like>& m_vec,
                        const char* mode,
                        const std::vector<std::string>& ins,
                        const std::vector<std::vector<std::string>>& outs,
                        const realizability_simplifier* rs = nullptr);
  /// @}

  /// \ingroup twa_io
  /// \brief Print the aig to stream in AIGER format
  SPOT_API std::ostream&
  print_aiger(std::ostream& os, const_aig_ptr circuit);

  /// \ingroup twa_io
  /// \brief Encode and print an automaton as an AIGER circuit.
  ///
  /// The circuit actually encodes the transition relation of the automaton, not
  /// its acceptance condition. Therefore, this function will reject automata
  /// whose acceptance condition is not trivial (i.e. true).
  /// States are encoded by latches (or registers) in the circuit. Atomic
  /// propositions are encoded as inputs and outputs of the circuit. To know
  /// which AP should be encoded as outputs, print_aiger() relies on the named
  /// property "synthesis-outputs", which is a bdd containing the conjunction of
  /// such output propositions. All other AP are encoded as inputs. If the named
  /// property is not set, all AP are encoded as inputs, and the circuit has no
  /// output.
  ///
  /// \pre  In order to ensure correctness, edge conditions have
  /// to have the form (input cond) & (output cond). The output cond
  /// does not need to be a minterm.
  /// Correct graphs are generated by spot::unsplit_2step
  ///
  /// \param os   The output stream to print on.
  /// \param aut  The automaton to output.
  /// \param mode This param has to be of the form
  ///             `ite|isop|both [+dc][+ud][+sub0|+sub1|+sub2]`
  ///             Where `ite` means encoded via if-then-else normal form,
  ///             `isop` means encoded via irredundant sum-of-products,
  ///             `both` means trying both encodings to keep the smaller one.
  ///             `+dc` is optional and tries to take advantage of "do not care"
  ///             outputs to minimize the encoding.
  ///             `+dual` is optional and indicates that the algorithm
  ///             should also try to encode the negation of each condition in
  ///             case its encoding is smaller.
  ///             `+subN` indicates that the conditions can be separated into
  ///             blocks with `sub0` being no separation, `sub1` separation into
  ///             input/latches/gates (`isop` only) and `sub2` tries to seek
  ///             common subformulas.
  SPOT_API std::ostream&
  print_aiger(std::ostream& os, const const_twa_graph_ptr& aut,
              const char* mode);
}
