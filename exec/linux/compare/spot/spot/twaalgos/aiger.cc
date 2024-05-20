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

#include "config.h"
#include <spot/twaalgos/aiger.hh>

#include <cmath>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cstring>
#include <tuple>
#include <utility>
#include <fstream>
#include <string>
#include <sstream>

#include <spot/twa/twagraph.hh>
#include <spot/misc/bddlt.hh>
#include <spot/misc/minato.hh>
#include <spot/twaalgos/mealy_machine.hh>
#include <spot/twaalgos/synthesis.hh>

#define STR(x) #x
#define STR_(x) STR(x)
#define STR_LINE STR_(__LINE__)

//#define TRACE
#ifdef TRACE
#  define trace std::cerr
#else
#  define trace while (0) std::cerr
#endif

namespace
{
  using namespace spot;

  static std::vector<std::string>
  name_vector(unsigned n, const std::string& prefix)
  {
    std::vector<std::string> res;
    for (unsigned i = 0; i != n; ++i)
      res.push_back(prefix + std::to_string(i));
    return res;
  }

  void check_double_names(std::vector<std::string> sv, std::string msg)
  {
    if (sv.size() < 2)
      return;
    std::sort(sv.begin(), sv.end());
    const unsigned svs = sv.size() - 1;
    for (unsigned i = 0; i < svs; ++i)
      if (sv[i] == sv[i+1])
        throw std::runtime_error(msg + sv[i]);
  }

  std::tuple<std::vector<std::string>, std::vector<std::string>,
             std::vector<unsigned>,
             std::vector<unsigned>,
             std::vector<std::pair<unsigned, unsigned>>
             >
  parse_aag_impl_(std::istream& iss, const std::string& filename)
  {

    auto error_aig = [filename](const std::string_view& msg,
                                unsigned line_start = 0,
                                unsigned line_end = 0)
      {
        std::ostringstream out;
        out << filename;
        if (line_start)
          out << ':' << line_start;
        if (line_end && line_end > line_start)
          out << '-' << line_end;
        out << ": " << msg;
        throw parse_error(out.str());
      };

    //results
    std::vector<std::string> input_names;
    std::vector<std::string> output_names;
    std::vector<unsigned> latches;
    std::vector<unsigned> outputs;
    std::vector<std::pair<unsigned, unsigned>> gates;

    unsigned max_index, nb_inputs, nb_latches, nb_outputs, nb_and;

    std::string line;
    unsigned line_number = 0;
    std::unordered_set<std::string> names;

    auto nextline = [&line, &line_number, &iss]() {
      line.clear();
      getline(iss, line);
      ++line_number;
    };

    nextline();
    if (sscanf(line.c_str(), "aag %u %u %u %u %u", &max_index, &nb_inputs,
               &nb_latches, &nb_outputs, &nb_and) != 5)
      error_aig("invalid header line", line_number);
    if (max_index < nb_inputs + nb_latches + nb_and)
      error_aig("more variables than indicated by max var", line_number);

    latches.reserve(nb_latches);
    outputs.reserve(nb_outputs);
    gates.reserve(nb_and);

    for (unsigned i = 0; i < nb_inputs; ++i)
      {
        nextline();
        unsigned expected = 2 * (i + 1);
        unsigned num = 0;
        int end = 0;
        const char* buf = line.c_str();
        if (!line.empty()
            && (sscanf(buf, "%u %n", &num, &end) != 1 || buf[end]))
          error_aig("invalid format for an input", line_number);
        if (num != expected)
          error_aig("expecting input number " + std::to_string(expected),
                    line_number);
      }
    for (unsigned i = 0; i < nb_latches; ++i)
      {
        nextline();
        unsigned expected = 2 * (1 + nb_inputs + i);
        unsigned latch_var = 0, next_state;
        int end = 0;
        const char* buf = line.c_str();
        if (!line.empty()
            && (sscanf(buf, "%u %u %n", &latch_var, &next_state, &end) != 2
                || buf[end]))
          error_aig("invalid format for a latch", line_number);
        if (latch_var != expected)
          error_aig("expecting latch number " + std::to_string(expected),
                    line_number);
        latches.push_back(next_state);
      }
    for (unsigned i = 0; i < nb_outputs; ++i)
      {
        nextline();
        if (line.empty())
          error_aig("expecting an output", line_number);
        unsigned num_out;
        const char* buf = line.c_str();
        int end = 0;
        if (sscanf(buf, "%u %n", &num_out, &end) != 1 || buf[end])
          error_aig("invalid format for an output", line_number);
        outputs.push_back(num_out);
      }
    for (unsigned i = 0; i < nb_and; ++i)
      {
        nextline();
        unsigned and_gate = 0, lhs, rhs;
        if (!line.empty())
          {
            const char* buf = line.c_str();
            int end = 0;
            if (sscanf(buf, "%u %u %u %n", &and_gate, &lhs, &rhs, &end) != 3
                || buf[end])
              error_aig("invalid format for an AND gate", line_number);
          }
        unsigned expected = 2 * (1 + nb_inputs + nb_latches + i);
        if (and_gate != expected)
          error_aig("expecting AND gate number " + std::to_string(expected),
                    line_number);
        gates.emplace_back(lhs, rhs);
      }
    nextline();
    bool comment_sec = false;
    unsigned first_input_line = 0;
    unsigned last_input_line = 0;
    unsigned first_output_line = 0;
    unsigned last_output_line = 0;
    while (iss && !comment_sec)
      {
        unsigned pos_var_name;
        char var_name[256];
        int end;
        const char* buf = line.c_str();
        switch (buf[0])
          {
          case 'l':               // latches names non supported
            {
              nextline();
              continue;
            }
          case 'i':
            {
              if (sscanf(buf, "i%u %255[^\n]%n",
                         &pos_var_name, var_name, &end) != 2
                  || !var_name[0] || buf[end])
                error_aig("could not parse as input name", line_number);
              unsigned expected = input_names.size();
              if (pos_var_name >= nb_inputs)
                error_aig("value " + std::to_string(pos_var_name)
                          + " exceeds input count", line_number);
              if (pos_var_name != expected)
                error_aig("expecting name for input "
                          + std::to_string(expected), line_number);
              if (!names.insert(var_name).second)
                error_aig(std::string("name '") + var_name
                          + "' already used", line_number);
              input_names.push_back(var_name);
              if (!first_input_line)
                first_input_line = line_number;
              else
                last_input_line = line_number;
              nextline();
              break;
            }
          case 'o':
            {
              if (sscanf(buf, "o%u %255[^\n]%n",
                         &pos_var_name, var_name, &end) != 2
                  || !var_name[0] || buf[end])
                error_aig("could not parse as output name", line_number);
              unsigned expected = output_names.size();
              if (pos_var_name >= nb_outputs)
                error_aig("value " + std::to_string(pos_var_name)
                          + " exceeds output count", line_number);
              if (pos_var_name != expected)
                error_aig("expecting name for output "
                          + std::to_string(expected), line_number);
              if (!names.insert(var_name).second)
                error_aig(std::string("name '") + var_name
                          + "' already used", line_number);
              output_names.push_back(var_name);
              if (!first_output_line)
                first_output_line = line_number;
              else
                last_output_line = line_number;
              nextline();
              break;
            }
          case 'c':
            {
              comment_sec = true;
              break;
            }
          default:
            {
              error_aig("unsupported line type", line_number);
            }
          }
      }
    if (!input_names.empty())
      {
        if (input_names.size() != nb_inputs)
          error_aig("either all or none of the inputs should be named",
                    first_input_line, last_input_line);
      }
    else
      {
        input_names = name_vector(nb_inputs, "i");
      }
    if (!output_names.empty())
      {
        if (output_names.size() != nb_outputs)
          error_aig("either all or none of the outputs should be named",
                    first_output_line, last_output_line);
      }
    else
      {
        output_names = name_vector(nb_outputs, "o");
      }
    return { input_names, output_names, latches, outputs, gates };
  }
}

namespace spot
{
  aig::aig(const std::vector<std::string>& inputs,
           const std::vector<std::string>& outputs,
           unsigned num_latches,
           bdd_dict_ptr dict)
           : num_inputs_(inputs.size()),
             num_outputs_(outputs.size()),
             num_latches_(num_latches),
             input_names_(inputs),
             output_names_(outputs),
             max_var_((inputs.size() + num_latches) * 2),
             next_latches_(num_latches, -1u),
             outputs_(outputs.size(), -1u),
             dict_{dict}
  {
    // check that in/out names are unique
    check_double_names(inputs, "spot/twaalgos/aiger.cc:" STR_LINE ": input"
                               " name appears multiple times: ");
    check_double_names(outputs, "spot/twaalgos/aiger.cc:" STR_LINE ": output"
                               " name appears multiple times: ");

    bdd2var_[bddtrue.id()] = aig_true();
    var2bdd_[aig_true()] = bddtrue;
    bdd2var_[bddfalse.id()] = aig_false();
    var2bdd_[aig_false()] = bddfalse;

    all_ins_ = bddtrue;
    all_latches_ = bddtrue;

    l0_ = dict_->register_anonymous_variables(num_latches_, this);
    for (unsigned i = 0; i < num_latches_; ++i)
      {
        bdd b = bdd_ithvar(l0_+i);
        register_latch_(i, b);
        all_latches_ &= b;
      }

    unsigned i = 0;
    for (auto& in : input_names_)
    {
      bdd b = bdd_ithvar(dict_->register_proposition(formula::ap(in), this));
      register_input_(i, b);
      all_ins_ &= b;
      ++i;
    }
    for (auto& out : output_names_)
      dict_->register_proposition(formula::ap(out), this);

    bdd2var_.reserve(4 * (num_inputs_ + num_outputs_ + num_latches_));
    var2bdd_.reserve(4 * (num_inputs_ + num_outputs_ + num_latches_));
  }

  /// \brief Constructing the circuit with generic names.
  aig::aig(unsigned num_inputs, unsigned num_outputs,
           unsigned num_latches, bdd_dict_ptr dict)
           : aig(name_vector(num_inputs, "in"),
             name_vector(num_outputs, "out"), num_latches, dict)
  {
  }

  // register the bdd corresponding the an
  // aig literal
  void aig::register_new_lit_(unsigned v, const bdd& b)
  {
    assert(!var2bdd_.count(v) || var2bdd_.at(v) == b);
    assert(!bdd2var_.count(b.id()) || bdd2var_.at(b.id()) == v);
    var2bdd_[v] = b;
    bdd2var_[b.id()] = v;
    // Also store negation
    // Do not use aig_not as tests will fail
    var2bdd_[v ^ 1] = !b;
    bdd2var_[(!b).id()] = v ^ 1;
  }

  void aig::register_latch_(unsigned i, const bdd& b)
  {
    register_new_lit_(latch_var(i), b);
  }

  void aig::register_input_(unsigned i, const bdd& b)
  {
    register_new_lit_(input_var(i), b);
  }

  // Unregisters positive and negative form of a literal
  // which has to be given in positive form
  void aig::unregister_lit_(unsigned v)
  {
    assert(((v&1) == 0) && "Expected positive form");
    unsigned n_del = bdd2var_.erase(var2bdd_[v].id());
    assert(n_del);
    (void) n_del;
    n_del = var2bdd_.erase(v);
    assert(n_del);
    (void) n_del;
    v = v ^ 1;
    n_del = bdd2var_.erase(var2bdd_[v].id());
    assert(n_del);
    (void) n_del;
    n_del = var2bdd_.erase(v);
    assert(n_del);
    (void) n_del;
  }

  // Get propositions that are commun to all
  // possible products so that they can be anded at the end
  bdd aig::accum_common_(const bdd& b) const
  {
    bdd commun_bdd = bddtrue;
    for (unsigned i = 0; i < num_inputs(); ++i)
      {
        if (bdd_implies(b, input_bdd(i)))
          commun_bdd &= input_bdd(i);
        else if (bdd_implies(b, input_bdd(i, true)))
          commun_bdd &= input_bdd(i, true);
      }
    for (unsigned i = 0; i < num_latches(); ++i)
      {
        if (bdd_implies(b, latch_bdd(i)))
          commun_bdd &= latch_bdd(i);
        else if (bdd_implies(b, latch_bdd(i, true)))
          commun_bdd &= latch_bdd(i, true);
      }
    assert(commun_bdd != bddfalse);
    return commun_bdd;
  }

  void aig::split_cond_(const bdd& b, char so_mode,
                        std::vector<bdd>& cond_parts)
  {
    cond_parts.clear();
    switch (so_mode)
      {
      case 0:
        {
          cond_parts.push_back(b);
          return;
        }
      case 1:
        {
          assert(bdd_is_cube(b));
          auto push = [&](const bdd& bb)
            {
              assert(bb != bddfalse);
              if (bb != bddtrue)
                cond_parts.push_back(bb);
            };
          // Break the bdd into latches/inputs/gates
          push(bdd_existcomp(b, all_latches_)); // Only latches
          push(bdd_existcomp(b, all_ins_)); // Only inputs
          push(bdd_exist(bdd_exist(b, all_ins_), all_latches_)); // Only gates
          return; //Done
        }
      case 2:
        {
          bdd common = accum_common_(b);
          if (common != bddtrue)
            {
              cond_parts.push_back(common);
              cond_parts.push_back(bdd_exist(b, common));
            }
          else
            cond_parts.push_back(b);
          return; //Done
        }
      default:
        throw std::runtime_error("Unrecognised option for encode_bdd.");
      }
  }

  aig::safe_point aig::get_safe_point_() const
  {
    return {max_var_, and_gates_.size()};
  }

  aig::safe_stash
  aig::roll_back_(safe_point sf, bool do_stash)
  {
    // todo specialise for safe_all?
    trace << "Roll back to sf: " << sf.first << "; " << sf.second << '\n';
    safe_stash ss;
    auto& [gates, vardict, negs] = ss;
    if (do_stash)
      {
        unsigned dn = max_var_ - sf.first;
        assert((dn&1) == 0);
        dn /= 2;
        assert((int) dn == std::distance(and_gates_.begin()+sf.second,
                                         and_gates_.end()));
        gates.resize(dn);
        vardict.reserve(dn);
        negs.reserve(dn);
        //Copy and erase the lits
        for (unsigned v = sf.first+2; v <= max_var_; v += 2)
          {
            vardict.emplace_back(v, var2bdd_[v]);
            negs.push_back(var2bdd_[v+1]);
          }
        // Copy the gates
        std::copy(and_gates_.begin()+sf.second, and_gates_.end(),
                  gates.begin());
        trace << "Safed " << gates.size() << '\n';
      }
    // 1. Delete all literals
    // max_var_old was used before
    // max_var_ is currently used
    for (unsigned v = sf.first + 2; v <= max_var_; v += 2)
      unregister_lit_(v);
    // 2. Set back the gates
    and_gates_.erase(and_gates_.begin() + sf.second, and_gates_.end());
    max_var_ = sf.first;
    trace << "After rollback: \n" << and_gates_.size() << " gates and\n"
          << max_var_ << " variables\n\n";
    return ss;
  }

  void
  aig::reapply_(safe_point sf, const safe_stash& ss)
  {
    // Do some check_ups
    auto& [gates, vardict, _] = ss;
    (void)_;
    trace << "Reapplying sf: " << sf.first << "; " << sf.second
          << "\nwith " << gates.size() << " additional gates.\n\n";
    assert(gates.size() == vardict.size());
    assert(sf.first == max_var_);
    assert(sf.second == and_gates_.size());
    (void)sf;
    unsigned new_max_var_ = max_var_ + gates.size()*2;
    for (auto& p : vardict)
      {
        assert(max_var_ + 1 < p.first);
        assert(p.first <= new_max_var_+1);
        register_new_lit_(p.first, p.second);
      }
    and_gates_.insert(and_gates_.end(),
                      gates.begin(), gates.end());
    max_var_ = new_max_var_;
    trace << "New Ngates: " << num_gates() << '\n';
  }

  void aig::set_output(unsigned i, unsigned v)
  {
    assert(v <= max_var() + 1);
    outputs_[i] = v;
  }

  void aig::set_next_latch(unsigned i, unsigned v)
  {
    assert(v <= max_var() + 1);
    next_latches_[i] = v;
  }

  unsigned aig::aig_not(unsigned v)
  {
    unsigned not_v = v ^ 1;
    assert(var2bdd_.count(v) && var2bdd_.count(not_v));
    return not_v;
  }

  unsigned aig::aig_and(unsigned v1, unsigned v2)
  {
    assert(var2bdd_.count(v1));
    assert(var2bdd_.count(v2));

    if (SPOT_UNLIKELY(v1 > v2))
      std::swap(v1, v2);
    if (SPOT_UNLIKELY(v1 == v2))
      return v1;

    bdd b = var2bdd_[v1] & var2bdd_[v2];
    auto [it, inserted] = bdd2var_.try_emplace(b.id(), 0);
    if (!inserted)
      return it->second;
    max_var_ += 2;
    it->second = max_var_;
    and_gates_.emplace_back(v1, v2);
    register_new_lit_(max_var_, b);
    return max_var_;
  }

  unsigned aig::aig_and(std::vector<unsigned>& vs)
  {
    // Note: we could check if some if the variables
    // correspond to true or false.
    // However this never happens during automatic construction,
    // so it would be useless overhead in most cases
    std::sort(vs.begin(), vs.end());
    auto new_end = std::unique(vs.begin(), vs.end());
    vs.erase(new_end, vs.end());

    if (vs.empty())
      return aig_true();
    if (vs.size() == 1)
      return vs[0];
    if (vs.size() == 2)
      return aig_and(vs[0], vs[1]);

    unsigned add_elem = -1u;
    do
      {
        if (vs.size() & 1)
          {
            // Odd size -> make even
            add_elem = vs.back();
            vs.pop_back();
          }

        // Reduce two by two inplace
        const unsigned idx_end = vs.size() / 2;
        for (unsigned i = 0; i < idx_end; ++i)
          vs[i] = aig_and(vs[2 * i], vs[2 * i + 1]);
        vs.resize(vs.size() / 2);
        // Add the odd elem if necessary
        if (add_elem != -1u)
          {
            vs.push_back(add_elem);
            add_elem = -1u;
          }
        // Sort to increase reusage gates
        std::sort(vs.begin(), vs.end());
      } while (vs.size() > 1);
    return vs[0];
  }

  unsigned aig::aig_or(unsigned v1, unsigned v2)
  {
    unsigned n1 = aig_not(v1);
    unsigned n2 = aig_not(v2);
    return aig_not(aig_and(n1, n2));
  }

  unsigned aig::aig_or(std::vector<unsigned>& vs)
  {
    std::transform(vs.begin(), vs.end(), vs.begin(),
                   [&](auto v){return aig_not(v); });
    vs[0] = aig_not(aig_and(vs));
    return vs[0];
  }

  unsigned aig::aig_pos(unsigned v)
  {
    return v & ~1;
  }
  unsigned aig::bdd2INFvar(const bdd& b)
  {
    // F = !v&low | v&high
    // De-morgan
    // !(!v&low | v&high) = !(!v&low) & !(v&high)
    // !v&low | v&high = !(!(!v&low) & !(v&high))
    // note that if low or high are T
    // we can simplify the formula
    // given that low / high is T
    // then !(!v&low) & !(v&high) can be simplified to
    // !v&low | v&high = !v | high / low | v
    // = !(v & !high) /  !(!low & !v)
    // The case when low / high is ⊥ is automatically treated
    auto b_it = bdd2var_.find(b.id());
    if (b_it != bdd2var_.end())
      return b_it->second;

    unsigned v_var = bdd2var_.at(bdd_ithvar(bdd_var(b)).id());

    unsigned b_branch_var[2] = {bdd2INFvar(bdd_low(b)),
                                bdd2INFvar(bdd_high(b))};

    unsigned l;
    unsigned r;

    if (b_branch_var[0] == aig_true())
      {
        // low == T
        l = v_var;
        r = aig_not(b_branch_var[1]);
      }
    else if (b_branch_var[1] == aig_true())
      {
        // high == T
        l = aig_not(b_branch_var[0]);
        r = aig_not(v_var);
      }
    else
      {
        // General case
        r = aig_not(aig_and(v_var, b_branch_var[1]));
        l = aig_not(aig_and(aig_not(v_var), b_branch_var[0]));
      }

    return aig_not(aig_and(l, r));
  }

  unsigned aig::cube2var_(const bdd& b, const int use_split_off)
  {
    assert(bdd_is_cube(b) && "bdd is not a cube");
    static std::vector<bdd> parts_;
    static std::vector<unsigned> prod_vars_;
    static std::vector<unsigned> prod_parts_;

    parts_.clear();
    prod_vars_.clear();

    split_cond_(b, use_split_off, parts_);

    decltype(bdd2var_)::const_iterator it;
    for (bdd b : parts_)
      {
        prod_parts_.clear();
        while (b != bddtrue)
          {
            //Check if we know the sub-bdd
            if ((it = bdd2var_.find(b.id())) != bdd2var_.end())
              {
                // We already constructed prod
                prod_parts_.push_back(it->second);
                break;
              }
            // if the next lines failes,
            // it is probably due to unregistered latches or ins
            // todo
//            unsigned v = bdd2var_.at(bdd_var(b));
            unsigned v = bdd2var_.at(bdd_ithvar(bdd_var(b)).id());
            if (bdd_high(b) == bddfalse)
            {
              v = aig_not(v);
              b = bdd_low(b);
            }
            else
              b = bdd_high(b);
            prod_parts_.push_back(v);
          }
        prod_vars_.push_back(aig_and(prod_parts_));
      }
    return aig_and(prod_vars_);
  }

  unsigned aig::bdd2ISOPvar(const bdd& b, const int use_split_off)
  {
    static std::vector<unsigned> plus_vars_;

    auto it = bdd2var_.find(b.id());
    if (it != bdd2var_.end())
      return it->second;

    // Is it worth checking if it is a cube?
    bdd prod;
    minato_isop cond(b);
    plus_vars_.clear();

    while ((prod = cond.next()) != bddfalse)
      plus_vars_.push_back(cube2var_(prod,
                                     use_split_off == 2 ? 0 : use_split_off));
    // Done building -> sum them
    return aig_or(plus_vars_);
  }

  unsigned aig::encode_bdd(const std::vector<bdd>& c_alt,
                           char method, bool use_dual,
                           int use_split_off)
  {
    // Before doing anything else, let us check if one the variables
    // already exists in which case we are done
#ifdef TRACE
    trace << "encoding one of \n";
    for (const auto& c: c_alt)
      trace << c << '\n';
#endif

    for (const bdd& cond : c_alt)
      {
        auto it = bdd2var_.find(cond.id());
        if (it != bdd2var_.end())
          {
            trace << "Condition already encoded -> Direct return\n\n";
            return it->second;
          }
      }

    safe_point sf = get_safe_point_();

    unsigned ngates_min = -1u;
    safe_stash ss_min;

    unsigned res_var = -1u;

    std::vector<char> used_m;
    if (method == 0 || method == 2)
      used_m.push_back(0);
    if (method == 1 || method == 2)
      used_m.push_back(1);
    assert(used_m.size()
           && "Cannot convert the given method. "
              "Only 0,1 and 2 are currently supported");

    auto enc_1 = [&](const bdd& b,
                     const char m)
      {
        return (m == 0) ? bdd2INFvar(b)
                        : bdd2ISOPvar(b, use_split_off != 2
                                         ? use_split_off : 0);
      };

    // we are given a list of "equivalent" bdds (we have to encode one of them
    // but we can chose which one)
    // We have different options:
    // method, use_dual and use_split_off
    // which can be arbitrarily combined

    std::vector<bdd> cond_parts;
    std::vector<unsigned> cond_vars;
    //for (bool do_negate : (use_dual ? std::initializer_list<bool>{false, true}
    //                                : std::initializer_list<bool>{false}))
    for (unsigned neg_counter = 0; neg_counter <= 0 + use_dual; ++neg_counter)
      {
        bool do_negate = neg_counter;
        for (const bdd& b : c_alt)
          {
            bdd b_used = do_negate ? bdd_not(b) : b;
            cond_parts.clear();
            split_cond_(b_used,
                        use_split_off != 1 ?  use_split_off : 0, cond_parts);

            for (auto m : used_m)
              {
                cond_vars.clear();
                for (const bdd& cpart : cond_parts)
                  {
                    cond_vars.push_back(enc_1(cpart, m));
                    if (num_gates() >= ngates_min)
                      break; // Cannot be optimal
                  }
                // Compute the and if there is still hope
                auto this_res = -1u;
                if (num_gates() < ngates_min)
                  this_res = aig_and(cond_vars);
                // Check if after adding these gates
                // the circuit is still smaller
                if (num_gates() < ngates_min)
                  {
                    // This is the new best
                    assert(this_res != -1u);
                    res_var = do_negate ? aig_not(this_res) : this_res;
                    ngates_min = num_gates();
                    trace << "Found new best encoding with\nneg: "
                          << do_negate << "\nmethod: " << (m == 0 ? "INF"
                                                                  : "ISOP")
                          << "\nalt: " << b
                          << "\nNgates: " << num_gates() << "\n\n";
                    ss_min = roll_back_(sf, true);
                  }
                else
                  // Reset the computations
                  {
                    trace << "Method \nneg: "
                          << do_negate << "\nmethod: " << (m == 0 ? "INF"
                                                                  : "ISOP")
                          << "\nalt: " << b
                          << "\nNgates: " << num_gates()
                          << " discarded.\n\n";
                    roll_back_(sf, false);
                  }
              } // Encoding styles
          } // alternatives
      } // end do_negate

    // Reapply the best result
    reapply_(sf, ss_min);
    return res_var;
  }


  unsigned aig::encode_bdd(const bdd& b,
                           char method, bool use_dual,
                           int use_split_off)
  {
    return encode_bdd(std::vector<bdd>{b}, method, use_dual, use_split_off);
  }

  void aig::encode_all_bdds(const std::vector<bdd>& all_bdd)
  {

    // Build the set of all bdds needed in a "smart" way
    // We only need the bdd or its negation, never both
    std::set<bdd, bdd_less_than> needed_bdd;
    for (const auto& b : all_bdd)
      {
        if (needed_bdd.count(bdd_not(b)))
          continue;
        needed_bdd.insert(b);
      }

    std::vector<std::vector<bdd>> sum_terms;
    sum_terms.reserve(all_bdd.size());
    std::set<bdd, bdd_less_than> needed_prods;
    // Do sth smart here
    std::vector<bdd> sum_terms_pos;
    std::vector<bdd> sum_terms_neg;
    std::vector<bdd> intersect;
    for (const auto& b : needed_bdd)
      {
        sum_terms_pos.clear();
        sum_terms_neg.clear();
        // Compute the ISOP of the primal and dual
        // Use the repr that adds less terms

        bdd prod;

        minato_isop cond_isop(b);
        while ((prod = cond_isop.next()) != bddfalse)
          sum_terms_pos.push_back(prod);

        cond_isop = minato_isop(bdd_not(b));
        while ((prod = cond_isop.next()) != bddfalse)
          sum_terms_neg.push_back(prod);

        std::sort(sum_terms_pos.begin(), sum_terms_pos.end(),
                  bdd_less_than());
        std::sort(sum_terms_neg.begin(), sum_terms_neg.end(),
                  bdd_less_than());

        intersect.clear();
        std::set_intersection(needed_prods.cbegin(), needed_prods.end(),
                              sum_terms_pos.cbegin(), sum_terms_pos.cend(),
                              std::back_inserter(intersect), bdd_less_than());
        unsigned n_add_pos = 0;
        std::for_each(sum_terms_pos.begin(), sum_terms_pos.end(),
                      [&n_add_pos, &intersect](const auto& b)
                      {
                        if (std::find(intersect.cbegin(), intersect.cend(),
                                      b) == intersect.cend())
                          n_add_pos += bdd_nodecount(b);
                      });

        intersect.clear();
        std::set_intersection(needed_prods.cbegin(), needed_prods.end(),
                              sum_terms_neg.cbegin(), sum_terms_neg.cend(),
                              std::back_inserter(intersect), bdd_less_than());
        unsigned n_add_neg = 0;
        std::for_each(sum_terms_neg.begin(), sum_terms_neg.end(),
                      [&n_add_neg, &intersect](const auto& b)
                      {
                        if (std::find(intersect.cbegin(), intersect.cend(),
                                      b) == intersect.cend())
                          n_add_neg += bdd_nodecount(b);
                      });

        if (n_add_pos <= n_add_neg)
          {
            needed_prods.insert(sum_terms_pos.begin(), sum_terms_pos.end());
            sum_terms.emplace_back(std::move(sum_terms_pos));
          }
        else
          {
            needed_prods.insert(sum_terms_neg.begin(), sum_terms_neg.end());
            sum_terms.emplace_back(std::move(sum_terms_neg));
          }
      }

    // Now we need to compute the prod_terms
    // todo switch to using id() to avoid ref count
    // and use a map
    std::vector<std::vector<bdd>> prod_terms;
    prod_terms.reserve(needed_prods.size());
    for (bdd aprod : needed_prods) //apord : i1&ni2...
      {
        prod_terms.emplace_back();
        auto& ptv = prod_terms.back();

        while (aprod != bddtrue)
        {
          bdd topvar = bdd_ithvar(bdd_var(aprod));
          bdd aprod_low = bdd_low(aprod);
          if (aprod_low == bddfalse)
            {
              ptv.push_back(topvar);
              aprod = bdd_high(aprod);
            }
          else
            {
              ptv.push_back(bdd_not(topvar));
              aprod = aprod_low;
            }
        }
        std::sort(ptv.begin(), ptv.end(),
                  bdd_less_than());
      }

      // Now we need to count and then create the stack
      // We start with the products
    auto bdd_pair_hash = [](const auto& p) noexcept
      {
        return pair_hash()(std::make_pair(
                            (unsigned) p.first.id(),
                            (unsigned) p.second.id()));
      };

    std::unordered_map<std::pair<bdd, bdd>, unsigned,
                       decltype(bdd_pair_hash)> occur_map(prod_terms.size(),
                                                           bdd_pair_hash);
    auto count_occ = [&occur_map](const auto& term)
      {
        unsigned l = term.size();
        for (unsigned i = 0; i < l; ++i)
          for (unsigned j = i + 1; j < l; ++j)
            {
              auto [it, ins] =
                occur_map.try_emplace({term[i], term[j]} , 0);
              (void)ins;
              it->second += 1;
            }
      };
    auto uncount_occ = [&occur_map](const auto& term)
      {
        unsigned l = term.size();
        for (unsigned i = 0; i < l; ++i)
          for (unsigned j = i + 1; j < l; ++j)
            {
              assert(occur_map.at({term[i], term[j]}) >= 1);
              occur_map[{term[i], term[j]}] -= 1;
            }
      };
    for (const auto& pterm : prod_terms)
      count_occ(pterm);

    auto and_ = [this](const auto& mi)
      {
        assert(bdd2var_.count(mi.first.first.id()));
        assert(bdd2var_.count(mi.first.second.id()));
        // Internal creation
        aig_and(bdd2var_[mi.first.first.id()], bdd2var_[mi.first.second.id()]);
        return mi.first.first & mi.first.second;
      };

    auto get_max = [&occur_map]
      {
        assert(occur_map.cbegin() != occur_map.cend());
        auto itm =
                std::max_element(occur_map.cbegin(), occur_map.cend(),
                     [](const auto& it1, const auto& it2)
                       { return std::make_tuple(it1.second,
                                                it1.first.first.id(),
                                                it1.first.second.id())
                                  < std::make_tuple(it2.second,
                                                    it2.first.first.id(),
                                                    it2.first.second.id()); });
        if (itm == occur_map.cend())
          throw std::runtime_error("Empty occurrence map");
        return *itm;
      };

    while (!occur_map.empty())
      {
        auto max_elem = get_max();
        unsigned n_occur_old = max_elem.second;
        if (max_elem.second == 0)
          break;

        // Create the gate
        bdd andcond = and_(max_elem);
        // Update
        // Check in all prods if this exists and update
        unsigned n_occur = 0;

        for (auto& pterm : prod_terms)
          {
            // todo, too costly right now
            // Find left and right
            // Note, left is always to the left of right
            auto itl = std::find(pterm.begin(), pterm.end(),
                                 max_elem.first.first);
            auto itr =
                itl == pterm.end() ? pterm.end()
                                   : std::find(itl+1, pterm.end(),
                                               max_elem.first.second);

            if ((itl != pterm.end()) && (itr != pterm.end()))
              {
                ++n_occur;
                // uncount -> modifiy -> count
                uncount_occ(pterm);
                pterm.erase(itr); //Order matters
                pterm.erase(itl);
                pterm.push_back(andcond);
                std::sort(pterm.begin(), pterm.end(),
                          bdd_less_than());
                count_occ(pterm);
              }
          }
        assert(n_occur_old == n_occur);
        (void) n_occur_old;
      }
    // All products should now be created
    assert(std::all_of(needed_prods.cbegin(), needed_prods.cend(),
                       [this](const bdd& aprod)
                       { return bdd2aigvar(aprod) > 0; }));

    //  We have created all products, now we need the sums
    //  We basically do the same but with or
    occur_map.clear();
    for (const auto& sterm : sum_terms)
      // a & b = b & a only count once
      count_occ(sterm);

    auto or_ = [this](const auto& mi)
      {
        assert(bdd2var_.count(mi.first.first.id()));
        assert(bdd2var_.count(mi.first.second.id()));
        // Internal creation
        aig_or(bdd2var_[mi.first.first.id()], bdd2var_[mi.first.second.id()]);
        return mi.first.first | mi.first.second;
      };

    while (!occur_map.empty())
      {
        auto max_elem = get_max();
        unsigned n_occur_old = max_elem.second;
        if (max_elem.second == 0)
          break;
        // Create the gate
        bdd orcond = or_(max_elem);
        // Update
        // Check in all prods if this exists and update
        unsigned n_occur = 0;

        for (auto& sterm : sum_terms)
          {
            // todo, too costly right now
            // Find left and right
            auto itl = std::find(sterm.begin(), sterm.end(),
                                 max_elem.first.first);
            auto itr =
                itl == sterm.end() ? sterm.end()
                    : std::find(itl+1, sterm.end(), max_elem.first.second);

            if ((itl != sterm.end()) && (itr != sterm.end()))
            {
              ++n_occur;
              uncount_occ(sterm);
              sterm.erase(itr); //Order matters
              sterm.erase(itl);
              sterm.push_back(orcond);
              std::sort(sterm.begin(), sterm.end(),
                        bdd_less_than());
              count_occ(sterm);
            }
          }
        assert(n_occur_old == n_occur);
        (void) n_occur_old;
      }
  }

  aig_ptr
  aig::parse_aag(const std::string& aig_file,
                 bdd_dict_ptr dict)
  {
    std::ifstream iss(aig_file, std::ios::in);
    if (iss)
      return parse_aag(iss, aig_file, dict);
    else
      throw std::runtime_error("Unable to open " + aig_file + '\n');
  }

  aig_ptr
  aig::parse_aag(const char* data,
                 const std::string& filename,
                 bdd_dict_ptr dict)
  {
    std::string data_s(data);
    std::istringstream iss(data_s);
    return parse_aag(iss, filename, dict);
  }

  aig_ptr
  aig::parse_aag(std::istream& iss,
                 const std::string& filename,
                 bdd_dict_ptr dict)
  {
    //result
    auto [in_names__, out_names__, next_latches__, outputs__, gates__] =
        parse_aag_impl_(iss, filename);

    aig_ptr res_ptr =
        std::make_shared<aig>(in_names__, out_names__,
                              next_latches__.size(), dict);
    // For some reason g++-7 thinks res_ptr could be null.
    SPOT_ASSUME(res_ptr);
    aig& circ = *res_ptr;
    circ.max_var_ =
        (in_names__.size() + next_latches__.size() + gates__.size())*2;
    std::swap(circ.next_latches_, next_latches__);
    std::swap(circ.outputs_, outputs__);
    std::swap(circ.and_gates_, gates__);

    // Create all the bdds/vars
    // true/false/latches/inputs already exist
    // Get the gate number corresponding to an output
    auto v2g = [&](unsigned v)->unsigned
      {
        v = circ.aig_pos(v);
        v /= 2;
        assert(v >= 1 + circ.num_inputs_ + circ.num_latches_
               && "Variable does not correspond to a gate");
        return v - (1 + circ.num_inputs_ + circ.num_latches_);
      };
    auto& var2bdd = circ.var2bdd_;
#ifndef NDEBUG
    auto& bdd2var = circ.bdd2var_;
#endif
    auto get_gate_bdd = [&](unsigned g, auto self)->bdd
      {
        unsigned v = circ.gate_var(g);
        auto it = var2bdd.find(v);
        if (it != var2bdd.end())
        {
          assert(bdd2var.at(var2bdd.at(v).id()) == v
                 && "Inconsistent bdd.");
          return it->second;
        }
        //get the vars of the input to the gates
        bdd gsbdd[2];
        unsigned gsv[2] = {circ.and_gates_.at(g).first,
                           circ.and_gates_.at(g).second};
        // Check if the exist
        for (unsigned i : {0, 1})
        {
          it = var2bdd.find(gsv[i]);
          if (it != var2bdd.end())
            gsbdd[i] = it->second;
          else
          {
            // Construct it
            gsbdd[i] = self(v2g(circ.aig_pos(gsv[i])), self);
            // Odd idx -> negate
            if (gsv[i]&1)
              gsbdd[i] = bdd_not(gsbdd[i]);
          }
        }
        bdd gbdd = bdd_and(gsbdd[0], gsbdd[1]);
        circ.register_new_lit_(v, gbdd);
        return gbdd;
      };
    // Do this for each gate then everything should exist
    // Also it should be consistent
    const unsigned n_gates = res_ptr->num_gates();
    for (unsigned g = 0; g < n_gates; ++g)
      get_gate_bdd(g, get_gate_bdd);
    //Check that all outputs and next_latches exist
    auto check = [&var2bdd](unsigned v)
      {
        if (not (var2bdd.count(v)))
          throw std::runtime_error("variable " + std::to_string(v)
                                   + " has no bdd associated.");
      };
    std::for_each(circ.next_latches_.begin(), circ.next_latches_.end(),
                  check);
    std::for_each(circ.outputs_.begin(), circ.outputs_.end(),
                  check);
    return res_ptr;
  }

  twa_graph_ptr aig::as_automaton(bool keepsplit) const
  {
    auto aut = make_twa_graph(dict_);

    assert(num_latches_ < sizeof(unsigned)*CHAR_BIT);

    const unsigned n_max_states = 1 << num_latches_;
    aut->new_states(n_max_states);

    auto s2bdd = [&](unsigned s)
      {
        bdd b = bddtrue;
        for (unsigned j = 0; j < num_latches_; ++j)
          {
            // Get the j-th latch in this partial strategy
            // If high -> not negated
            b &= latch_bdd(j, !(s & 1));
            s >>= 1;
          }
        return b;
      };

    std::vector<bdd> outbddvec;
    outbddvec.reserve(output_names_.size());
    for (auto& ao : output_names_)
      outbddvec.push_back(bdd_ithvar(aut->register_ap(ao)));
    // Also register the ins
    for (auto& ai : input_names_)
      aut->register_ap(ai);

    // Set the named prop
    set_synthesis_outputs(aut,
                    std::accumulate(outbddvec.begin(), outbddvec.end(),
                                    (bdd) bddtrue,
                                    [](const bdd& b1, const bdd& b2) -> bdd
                                        {return b1 & b2; }));

    std::vector<bdd> outcondbddvec;
    for (auto ov : outputs_)
      outcondbddvec.push_back(aigvar2bdd(ov));

    auto get_out = [&](const bdd& sbdd, const bdd& insbdd)
      {
        bdd out = bddtrue;
        for (unsigned i = 0; i < num_outputs_; ++i)
          {
            if ((outcondbddvec[i] & sbdd & insbdd) != bddfalse)
              out &= outbddvec[i];
            else
              out -= outbddvec[i];
          }
        return out;
      };


    //Nextlatch is a function of latch and input
    std::vector<bdd> nxtlbddvec(num_latches_);
    for (unsigned i = 0; i < num_latches_; ++i)
      nxtlbddvec[i] = aigvar2bdd(next_latches_[i]);

    auto get_dst = [&](const bdd& sbdd, const bdd& insbdd)
      {
        // the first latch corresponds to the most significant digit
        unsigned dst = 0;
        unsigned off = 1;
        for (unsigned i = 0; i < num_latches_; ++i)
          {
            bdd ilatch = nxtlbddvec[i];
            // evaluate
            ilatch = (ilatch & sbdd & insbdd);
            dst += (ilatch != bddfalse)*off;
            off *= 2;
          }
        return dst;
      };

    bdd alli = bddtrue;
    std::vector<bdd> ibddvec(num_inputs_);
    for (unsigned i = 0; i < num_inputs_; ++i)
      {
        ibddvec[i] = input_bdd(i);
        alli &= ibddvec[i];
      }

    std::deque<unsigned> todo;
    todo.push_back(0);
    assert(n_max_states > 0);
    std::vector<bool> seen(n_max_states, false);
    seen[0] = true;

    std::unordered_map<unsigned long long, unsigned> splayer_map;
    //dst + cond -> state
    auto get_id = [](const bdd& ocond, unsigned dst)
      {
        constexpr unsigned shift = (sizeof(size_t) / 2) * 8;
        size_t u = dst;
        u <<= shift;
        u += (unsigned) ocond.id();
        return u;
      };

    while (!todo.empty())
      {
        unsigned s = todo.front();
        todo.pop_front();

        // bdd of source state
        bdd srcbdd = s2bdd(s);
        // All possible inputs
        // Note that for unspecified input sequences the
        // result is unspecified as well

        //todo change to new format
        for (auto inbdd : minterms_of(bddtrue, alli))
          {
            // Get the target state
            unsigned sprime = get_dst(srcbdd, inbdd);
            // Get the associated cout cond
            bdd outbdd = get_out(srcbdd, inbdd);

            if (keepsplit)
              {
                auto id = get_id(outbdd, sprime);
                auto it = splayer_map.find(id);
                if (it == splayer_map.end())
                  {
                    unsigned ntarg = aut->new_state();
                    splayer_map[id] = ntarg;
                    aut->new_edge(s, ntarg, inbdd);
                    aut->new_edge(ntarg, sprime, outbdd);
                  }
                else
                  aut->new_edge(s, it->second, inbdd);
              }
            else
              aut->new_edge(s, sprime, inbdd & outbdd);
            if (!seen[sprime])
              {
                seen[sprime] = true;
                todo.push_back(sprime);
              }
          }
      }
    aut->purge_unreachable_states();
    aut->merge_edges();
    if (keepsplit)
      // Mealy machines by definition start with env trans
      alternate_players(aut, false, false);
    return aut;
  }

  void aig::circ_init()
  {
    state_.resize(max_var_ + 2);
    std::fill(state_.begin(), state_.end(), false);
    // Set "true"
    state_[1] = 1;
  }

  void aig::circ_step(const std::vector<bool>& inputs)
  {
    assert(inputs.size() == num_inputs()
           && "Input length does not match");
    assert(state_.size() == max_var_ + 2
           && "State vector does not have the correct size. "
              "Forgot to initialize?");
    // Set the inputs
    for (unsigned i = 0; i < num_inputs(); ++i)
      {
        state_[input_var(i)] = inputs[i];
        state_[input_var(i, true)] = !inputs[i];
      }
    // Latches already have correct state either from
    // init or from last iteration

    // Loop through all gates
    const unsigned ng = num_gates();
    for (unsigned i = 0; i < ng; ++i)
      {
        unsigned var_g = gate_var(i);
        state_[var_g] = state_[and_gates_[i].first]
                        && state_[and_gates_[i].second];
        state_[aig_not(var_g)] = !state_[var_g];
      }
    // Update latches
    const auto& nl = next_latches();
    for (unsigned i = 0; i < num_latches(); ++i)
      {
        unsigned lv = latch_var(i);
        state_[lv] = state_[nl[i]];
        state_[aig_not(lv)] = !state_[lv];
      }

  }
}

namespace
{
  using namespace spot;

  // We have to decide which actual output to use:
  // Heuristic : Use the assignment having the smallest cost
  // according to satoneshortest
  static std::vector<std::vector<bdd>>
  choose_outc(const std::vector<std::pair<const_twa_graph_ptr, bdd>>&
                  strat_vec,
              const bdd& all_inputs)
  {
    std::vector<std::vector<bdd>> used_outc;

    for (const auto& astrat : strat_vec)
      {
        used_outc.emplace_back(astrat.first->num_edges()+1);
        auto& this_outc = used_outc.back();

        // Check if split or separated
        if (auto sp_ptr =
              astrat.first->get_named_prop<region_t>("state-player"))
          {
            // Split -> We only need outs for env edges
            auto sp = *sp_ptr;
            const unsigned N = astrat.first->num_states();
            for (unsigned s = 0; s < N; ++s)
              {
                if (sp[s])
                  continue; //Player state -> nothing to do
                for (const auto& e : astrat.first->out(s))
                  {
                    assert(e.cond != bddfalse);
                    bdd bout = astrat.first->out(e.dst).begin()->cond;
                    assert(bout != bddfalse);
                    // low is good, dc is ok, high is bad
                    this_outc[astrat.first->edge_number(e)] =
                        bdd_satoneshortest(bout, 0, 1, 5);
                  }
              }
          }
        else
          {
            // separated
            for (const auto& e: astrat.first->edges())
              {
                assert(e.cond != bddfalse);
                bdd bout = bdd_exist(e.cond, all_inputs);
                // low is good, dc is ok, high is bad
                this_outc[astrat.first->edge_number(e)] =
                    bdd_satoneshortest(bout, 0, 1, 5);
              }
          }
      }
    //Done
    return used_outc;
  }

  static void
  state_to_vec(std::vector<unsigned>& v, unsigned s,
               unsigned offset)
  {
    assert(s != -1u && "-1u is not a valid sstate");
    v.clear();
    unsigned i = offset;
    while (s > 0)
      {
        if (s & 1)
          v.push_back(i);
        s >>= 1;
        ++i;
      }
  }

  static void
  output_to_vec(std::vector<unsigned>& v,
                std::map<unsigned, bool>& out_dc_vec, bdd b,
                const std::unordered_map<unsigned, unsigned>&
                bddvar_to_outputnum)
  {
    // We do not care about an output if it does not appear in the bdd
    for (auto& [_, dc_v] : out_dc_vec)
      {
        (void) _;
        dc_v = true;
      }
    v.clear();
    while (b != bddtrue)
      {
        unsigned i = bddvar_to_outputnum.at(bdd_var(b));
        out_dc_vec.at(i) = false;
        assert((bdd_low(b) == bddfalse) || (bdd_high(b) == bddfalse));
        if (bdd_low(b) == bddfalse)
          {
            v.push_back(i);
            b = bdd_high(b);
          }
        else
          b = bdd_low(b);
      }
  }

  // Transforms a vector of strategies and their respective
  // outputs into an Aig
  static aig_ptr
  auts_to_aiger(const std::vector<std::pair<const_twa_graph_ptr, bdd>>&
                strat_vec,
                const char* mode,
                const std::vector<std::string>& unused_ins = {},
                const std::vector<std::string>& unused_outs = {},
                const realizability_simplifier* rs = nullptr)
  {
    // The aiger circuit can currently noly encode separated mealy machines

    for (const auto& astrat : strat_vec)
      if (!astrat.first->acc().is_t())
        {
          std::cerr << "Acc cond must be true not " << astrat.first->acc()
                    << std::endl;
          throw std::runtime_error("Cannot turn automaton into "
                                   "aiger circuit");
        }
    // This checks the acc again, but does more and is to
    // costly for non-debug mode
    // We can also handle split machines
    assert(std::all_of(strat_vec.begin(), strat_vec.end(),
           [](const auto& p){ return is_separated_mealy(p.first)
                                     || is_split_mealy(p.first); }));

    // get the propositions
    std::vector<std::string> input_names;
    std::vector<std::string> output_names;
    bdd all_inputs = bddtrue;
    bdd all_outputs = bddtrue;

    // Join all the outputs
    for (auto& astrat : strat_vec)
      all_outputs &= astrat.second;

    std::vector<bdd> all_inputs_vec;
    std::unordered_map<unsigned, unsigned> bddvar_to_num;
    {
      unsigned i_in = 0;
      unsigned i_out = 0;
      for (auto& astrat : strat_vec)
        {
          for (const auto& ap : astrat.first->ap())
            {
              int bddvar =
                astrat.first->get_dict()->
                  has_registered_proposition(ap, astrat.first);
              assert(bddvar >= 0);
              bdd b = bdd_ithvar(bddvar);
              if (bdd_implies(all_outputs, b)) // ap is an output AP
                {
                  output_names.push_back(ap.ap_name());
                  bool inserted = bddvar_to_num.try_emplace(bddvar,
                                                            i_out++).second;
                  if (SPOT_UNLIKELY(!inserted))
                    throw std::runtime_error("Intersecting outputs");
                }
              else // ap is an input AP -> check if already registered
                {
                  if (bdd_implies(all_inputs, b))
                    continue;
                  // New input
                  input_names.push_back(ap.ap_name());
                  all_inputs &= b;
                  bddvar_to_num[bddvar] = i_in++;
                }
            }
        }
    }

    // Create two new vector holding used and unused inputs/outputs
    // Used propositions appear first
    std::vector<std::string> input_names_all = input_names;
    input_names_all.insert(input_names_all.end(),
                           unused_ins.cbegin(),
                           unused_ins.cend());
    std::vector<std::string> output_names_all = output_names;
    output_names_all.insert(output_names_all.end(),
                            unused_outs.cbegin(),
                            unused_outs.cend());

    if (rs)
      // If we have removed some APs from the original formula, they
      // might have dropped out of the output_names list (depending on
      // how we split the formula), but they should not have dropped
      // from the input_names list.  So let's fix the output_names
      // lists by adding anything that's not an input and not already
      // there.
      for (auto [k, k_is_input, v]: rs->get_mapping())
        {
          (void) v;
          if (k_is_input)
            continue;
          const std::string s = k.ap_name();
          if (std::find(output_names_all.begin(), output_names_all.end(), s)
              == output_names_all.end())
            output_names_all.push_back(s);
        }

    // Decide on which outcond to use
    // The edges of the automaton all have the form in&out
    // due to the unsplit
    // however we need the edge out cond to be encoded by the aiger
    // so we have to decide which minterm to use

    std::vector<std::vector<bdd>> used_outc;
    // Heuristic: Use the assignment with the shortest path
    // according to a weight function depending on the number
    // of highs, lows and do-not-cares
    used_outc = choose_outc(strat_vec, all_inputs);

    // Encode state in log2(num_states) latches.
    // The latches of each strategy have to be separated
    // as the strategies advance synchronously
    // so we get latches = [latches_0, latches_1, ....]

    // latches per strat
    // If the states in the automaton are named,
    // it is assumed that they are named by integers
    // and these values will be used for encoding
    // This coding has to ensure that the initial state
    // is zero
    // attention : least significant bit -> left / idx 0
    std::vector<std::vector<unsigned>> state_numbers;
    std::vector<unsigned> log2n;
    log2n.reserve(strat_vec.size());
    // cumulative sum of latches across strats
    std::vector<unsigned> latch_offset;
    latch_offset.reserve(strat_vec.size()+1);
    unsigned n_latches = 0;
    for (const auto& astrat : strat_vec)
      {
        const unsigned N = astrat.first->num_states();
        state_numbers.emplace_back(N, -1u);
        auto& sn = state_numbers.back();
        unsigned max_index = 0;
        // Check if named
        auto sp_ptr = astrat.first->get_named_prop<region_t>("state-player");
        if (const auto* s_names =
                astrat.first->
                  get_named_prop<std::vector<std::string>>("state-names"))
          {
            for (unsigned n = 0; n < N; ++n)
              {
                if (sp_ptr && (*sp_ptr)[n])
                  continue;
                // Remains -1u
                unsigned su = std::stoul((*s_names)[n]);
                max_index = std::max(max_index, su);
                sn[n] = su;
              }
            ++max_index;
          }
        else
          {
            if (sp_ptr)
              {
                auto sp = *sp_ptr;
                // Split
                unsigned n_next = 0;
                // Player -1u
                // Env: Succesively numbered according to appearance
                for (unsigned n = 0; n < N; ++n)
                  if (!sp[n])
                    sn[n] = n_next++;
                max_index = n_next;
              }
            else
              {
                std::iota(state_numbers.back().begin(),
                          state_numbers.back().end(), 0);
                max_index = N;
              }

            // Ensure 0 <-> init state
            std::swap(state_numbers.back()[0],
                      state_numbers.back()[astrat.first->
                                             get_init_state_number()]);
          }
        // Largest index to encode -> num_states()-1
        log2n.push_back(std::ceil(std::log2(max_index)));
        latch_offset.push_back(n_latches);
        n_latches += log2n.back();
      }
    latch_offset.push_back(n_latches);

    assert(output_names.size() == (unsigned) bdd_nodecount(all_outputs));
    aig_ptr circuit_ptr =
        std::make_shared<aig>(input_names_all, output_names_all,
                              n_latches, strat_vec[0].first->get_dict());
    aig& circuit = *circuit_ptr;

    // Latches and outputs are expressed as a bdd
    // The bdd are then translated into aiger circuits
    // relying on different strategies
    const unsigned n_outs = output_names.size();
    std::vector<bdd> latch(n_latches, bddfalse);
    std::vector<bdd> out(n_outs, bddfalse);
    std::vector<bdd> out_dc(n_outs, bddfalse);

    std::vector<unsigned> out_vec;
    out_vec.reserve(n_outs);
    std::map<unsigned, bool> out_dc_vec; //Bdd where out can be high

    std::vector<unsigned> next_state_vec;
    next_state_vec.reserve(n_latches);

    // Loop over the different strategies
    for (unsigned i = 0; i < strat_vec.size(); ++i)
      {
        auto&& [astrat, aouts] = strat_vec[i];
        const auto& sn = state_numbers.at(i);

        auto sp_ptr = astrat->get_named_prop<region_t>("state-player");

        auto latchoff = latch_offset[i];
#ifndef NDEBUG
        auto latchoff_next = latch_offset.at(i+1);
#endif
        auto alog2n = log2n[i];
        auto state2bdd = [&](auto s)
          {
            assert((!sp_ptr || !(*sp_ptr)[s])
                    && "Only env states need to be encoded.");
            auto s2 = sn[s];
            bdd b = bddtrue;
            for (unsigned j = 0; j < alog2n; ++j)
              {
                // Get the j-th latch in this partial strategy
                // If high -> not negated
                b &= circuit.latch_bdd(latchoff + j, !(s2 & 1));
                s2 >>= 1;
              }
            assert(s2 <= 1);
            return b;
          };

        //set the possible do not cares for this strat
        {
          out_dc_vec.clear();
          bdd r_outs = aouts;
          while (r_outs != bddtrue)
            {
              out_dc_vec[bddvar_to_num.at(bdd_var(r_outs))] = false;
              r_outs = bdd_high(r_outs);
            }
        }

        for (unsigned s = 0; s < astrat->num_states(); ++s)
          {
            // Player state -> continue
            if (sp_ptr && (*sp_ptr)[s])
              continue;
            // Convert src state to bdd
            bdd src_bdd = state2bdd(s);

            for (const auto& e : astrat->out(s))
              {
                // High latches of dst
                state_to_vec(next_state_vec,
                             sp_ptr ? sn[astrat->out(e.dst).begin()->dst]
                                    : sn[e.dst],
                             latchoff);

                // Get high outs depending on cond
                output_to_vec(out_vec, out_dc_vec,
                              used_outc[i][astrat->edge_number(e)],
                              bddvar_to_num);

                // The condition that joins in_cond and src
                // Note that the circuit and the strategy share a
                // bdd_dict
                bdd tot_cond = src_bdd & bdd_exist(e.cond, aouts);
                // Test should not have any outs from other strats

                // Set in latches/outs having "high"
                for (auto&& nl : next_state_vec)
                  {
                    assert (latchoff <= nl && nl < latchoff_next);
                    latch.at(nl) |= tot_cond;
                  }
                for (auto&& ao : out_vec)
                  out.at(ao) |= tot_cond;
                // And the do not cares
                for (const auto& [ao, v] : out_dc_vec)
                  if (v)
                    out_dc.at(ao) |= tot_cond;
              } // edges
          } // state
      } //strat

    struct tr_opt
    {
      char method;
      bool use_dual = false;
      bool use_dontcare = false;
      int use_split_off = 0;
      std::string s;
    };

    auto to_treat = [&mode]()
      {
        std::vector<tr_opt> res;
        std::stringstream s;
        s << mode;
        std::string buffer;
        std::string buffer2;

        while (std::getline(s, buffer, ','))
          {
            tr_opt this_opt;
            // Store raw info
            this_opt.s = buffer;
            std::stringstream s2;
            s2 << buffer;
            std::getline(s2, buffer2, '+');
            // Method
            if (buffer2 == "ite")
              this_opt.method = 0;
            else if (buffer2 == "isop")
              this_opt.method = 1;
            else if (buffer2 == "both")
              this_opt.method = 2;
            else if (buffer2 == "optim")
              this_opt.method = 3;
            else
              throw std::runtime_error("Encoding option unrecognised, "
                                       "expected ite, isop or both.");

            if (s2)
              while (std::getline(s2, buffer2, '+'))
                {
                  if (buffer2 == "ud")
                    this_opt.use_dual = true;
                  else if (buffer2 == "dc")
                    this_opt.use_dontcare = true;
                  else if (buffer2.find("sub") == 0)
                    {
                        if (buffer2.size() != 4)
                          {
                            std::cerr << "Got option " << mode << '\n';
                            throw std::runtime_error("Option for splitting off"
                                                     " sub-expressions is "
                                                     "expected to have "
                                                     "4 chars");
                          }
                        this_opt.use_split_off =
                                          (int)buffer2[3] - 48;
                        if ((this_opt.use_split_off < 0)
                            || (this_opt.use_split_off > 2))
                          throw std::runtime_error("Option for splitting off "
                                                   "must be one of sub0, sub1, "
                                                   "or sub2");
                    }
                  else
                    throw std::runtime_error(buffer + " does not describe a "
                    "mode supported for AIGER creation. Expected\n"
                    "ite|isop|both[+sub0|sub1|sub2][+dc][+ud]\n"
                    "or a coma separated list of such expressions.");
                }
            res.push_back(std::move(this_opt));
          }
        return res;
      }();

    auto sf = circuit.get_safe_point_();
    unsigned min_gates = -1u;
    aig::safe_stash ss;
    std::function<unsigned(const bdd&, const bdd&)> bdd2var;
    std::function<unsigned(const bdd&, const bdd&)> bdd2var_min;

    // Fuse the do not cares
    for (unsigned i = 0; i < n_outs; ++i)
      out_dc[i] |= out[i];

    for (const auto& amodedescr : to_treat)
      {
        if (amodedescr.method == 3)
          {
            // Here it is more tricky
            // First get a vector with all conditions needed in the
            // strategy
            std::vector<bdd> all_cond;
            all_cond.reserve(out.size() + latch.size());
            all_cond.insert(all_cond.end(), out.cbegin(), out.cend());
            all_cond.insert(all_cond.end(), latch.cbegin(), latch.cend());
            // Then construct it
            circuit.encode_all_bdds(all_cond);
            bdd2var = [&circuit](const bdd& b, const bdd&)->unsigned
                        {return circuit.bdd2aigvar(b); };
          }
        else
          {
            bdd2var = [&circuit, amodedescr](const auto& b1, const auto& b2)
              {
                static std::vector<bdd> alt_conds;
                alt_conds.clear();

                alt_conds.push_back(b1);

                if (amodedescr.use_dontcare && (b2 != bddfalse))
                  {
                    assert(bdd_implies(b1, b2));
                    alt_conds.push_back(b2);
                  }

                return circuit.encode_bdd(alt_conds, amodedescr.method,
                                          amodedescr.use_dual,
                                          amodedescr.use_split_off);
              };

            // Create the vars
            for (unsigned i = 0; i < n_outs; ++i)
              {
                trace << "Assign out " << i << '\n';
                if (circuit.num_gates() > min_gates)
                  break;
                circuit.set_output(i, bdd2var(out[i], out_dc[i]));
              }
            for (unsigned i = 0; i < n_latches; ++i)
              {
                trace << "Assign latch " << i << '\n';
                if (circuit.num_gates() > min_gates)
                  break;
                circuit.set_next_latch(i, bdd2var(latch[i], bddfalse));
              }
          }

        // Overwrite the stash if we generated less gates
        if (circuit.num_gates() < min_gates)
          {
            trace << "New best mode: " << amodedescr.s
                  << " with Ngates: " << circuit.num_gates() << '\n';
            min_gates = circuit.num_gates();
            ss = circuit.roll_back_(sf, true);
            bdd2var_min = bdd2var;
          }
        else
          circuit.roll_back_(sf, false);
      }
    //Use the best sol
    circuit.reapply_(sf, ss);
    trace << "Finished encoding, reassigning\n"
          << "Final gate count is " << circuit.num_gates() << '\n';
    // Reset them
    for (unsigned i = 0; i < n_outs; ++i)
      circuit.set_output(i, bdd2var_min(out[i], out_dc[i]));

    // Set unused signal to false by default
    const unsigned n_outs_all = output_names_all.size();
    for (unsigned i = n_outs; i < n_outs_all; ++i)
      circuit.set_output(i, circuit.aig_false());

    // RS may contains assignments for unused signals, such as
    //     out1 := 1
    //     out2 := 0
    //     out3 := in1
    //     out4 := !out3
    // But because the formula is simplified in a loop (forcing
    // some of those values in the formula reveal more values to
    // be forced), it is possible that the rhs refers to a variable
    // that is forced later in the mapping.  Therefore the mapping
    // should be applied in reverse order.
    if (rs)
      {
        auto mapping = rs->get_mapping();
        for (auto it = mapping.rbegin(); it != mapping.rend(); ++it)
          {
            auto [from, from_is_input, to] = *it;
            if (from_is_input)
              continue;

            auto j = std::find(output_names_all.begin(),
                               output_names_all.end(),
                               from.ap_name());
            assert(j != output_names_all.end());
            int i = j - output_names_all.begin();
            if (to.is_tt())
              {
                circuit.set_output(i, circuit.aig_true());
                continue;
              }
            else if (to.is_ff())
              {
                circuit.set_output(i, circuit.aig_false());
                continue;
              }
            else
              {
                formula repr = to;
                bool neg_repr = false;
                  if (repr.is(op::Not))
                    {
                      neg_repr = true;
                      repr = repr[0];
                    }
                  // is repr an input?
                  auto ap_name = repr.ap_name();
                  if (auto it2 = std::find(input_names_all.begin(),
                                          input_names_all.end(),
                                          ap_name);
                      it2 != input_names_all.end())
                    {
                      unsigned ivar =
                        circuit.input_var(it2 - input_names_all.begin(),
                                          neg_repr);
                      circuit.set_output(i, ivar);
                    }
                  // is repr an output?
                  else
                    {
                      it2 = std::find(output_names_all.begin(),
                                      output_names_all.end(), ap_name);
                      assert(it2 != output_names_all.end());
                      unsigned outnum = it2 - output_names_all.begin();
                      unsigned outvar = circuit.output(outnum);
                      circuit.set_output(i, outvar + neg_repr);
                    }
              }
          }
      }
    for (unsigned i = 0; i < n_latches; ++i)
      circuit.set_next_latch(i, bdd2var_min(latch[i], bddfalse));
    return circuit_ptr;
  } // auts_to_aiger
}

namespace spot
{

  aig_ptr
  mealy_machine_to_aig(const const_twa_graph_ptr& m, const char* mode)
  {
    if (!m)
      throw std::runtime_error("mealy_machine_to_aig(): "
                               "m cannot be null.");

    return auts_to_aiger({{m, get_synthesis_outputs(m)}}, mode);
  }

  aig_ptr
  mealy_machine_to_aig(const mealy_like& m, const char* mode)
  {
    if (m.success != mealy_like::realizability_code::REALIZABLE_REGULAR)
      throw std::runtime_error("mealy_machine_to_aig(): "
                               "Can only handle regular mealy machine, yet.");

    return mealy_machine_to_aig(m.mealy_like, mode);
  }

  aig_ptr
  mealy_machine_to_aig(const twa_graph_ptr &m, const char *mode,
                       const std::vector<std::string>& ins,
                       const std::vector<std::string>& outs,
                       const realizability_simplifier* rs)
  {
    if (!m)
      throw std::runtime_error("mealy_machine_to_aig(): "
                               "m cannot be null");

    // Make sure ins and outs are disjoint
    {
      std::vector<std::string> all_ap = ins;
      all_ap.insert(all_ap.end(), outs.begin(), outs.end());
      check_double_names(all_ap, "mealy_machine_to_aig(): "
                                 "Atomic propositions appears in input "
                                 "and output propositions; ");
    }
    // Check if there exist outs or ins that are not used by the
    // strategy
    // ins -> simply declare them
    // outs -> set them to false
    std::vector<std::string> unused_outs;
    std::vector<std::string> unused_ins;
    {
      std::set<std::string> used_aps;
      for (const auto& f : m->ap())
        used_aps.insert(f.ap_name());
      for (const auto& ao : outs)
        if (!used_aps.count(ao))
          unused_outs.push_back(ao);
      for (const auto& ai : ins)
        if (!used_aps.count(ai))
          unused_ins.push_back(ai);
    }
    // todo Some additional checks?
    return auts_to_aiger({{m, get_synthesis_outputs(m)}}, mode,
                         unused_ins, unused_outs, rs);
  }

  aig_ptr
  mealy_machine_to_aig(mealy_like& m, const char *mode,
                       const std::vector<std::string>& ins,
                       const std::vector<std::string>& outs,
                       const realizability_simplifier* rs)
  {
    if (m.success != mealy_like::realizability_code::REALIZABLE_REGULAR)
      throw std::runtime_error("mealy_machine_to_aig(): "
                               "Can only handle regular mealy machine, yet.");

    return mealy_machine_to_aig(m.mealy_like, mode, ins, outs, rs);
  }

  aig_ptr
  mealy_machines_to_aig(const std::vector<const_twa_graph_ptr>& m_vec,
                        const char *mode)
  {
    std::for_each(m_vec.begin()+1, m_vec.end(),
                  [usedbdd = m_vec.at(0)->get_dict()](const auto& s)
                  {
                    if (usedbdd != s->get_dict())
                      throw std::runtime_error("mealy_machines_to_aig(): "
                                               "All machines have to "
                                               "share a bdd_dict.");
                  });

    std::vector<std::pair<const_twa_graph_ptr, bdd>> new_vec;
    new_vec.reserve(m_vec.size());

    bdd all_outputs = bddtrue;
    for (auto& am : m_vec)
      {
        bdd this_outputs = get_synthesis_outputs(am);
        // Check if outs do not overlap
        if (bdd_and(bdd_not(this_outputs), all_outputs) == bddfalse)
          throw std::runtime_error("mealy_machines_to_aig(): "
                                   "\"outs\" of the machines are not "
                                   "distinct.");
        all_outputs &= this_outputs;
        new_vec.emplace_back(am, this_outputs);
      }
    return auts_to_aiger(new_vec, mode);
  }

  aig_ptr
  mealy_machines_to_aig(const std::vector<mealy_like>& m_vec,
                        const char *mode)
  {
    if (std::any_of(m_vec.cbegin(), m_vec.cend(),
                 [](const auto& m)
                 {return m.success !=
                     mealy_like::realizability_code::REALIZABLE_REGULAR; }))
      throw std::runtime_error("mealy_machines_to_aig(): "
                               "Can only handle regular mealy machine for "
                               "the moment.");
    auto new_vec = std::vector<const_twa_graph_ptr>();
    new_vec.reserve(m_vec.size());
    std::transform(m_vec.cbegin(), m_vec.cend(),
                   std::back_inserter(new_vec),
                   [](const auto& m){return m.mealy_like; });

    return mealy_machines_to_aig(new_vec, mode);
  }

  // Note: This ignores the named property
  aig_ptr
  mealy_machines_to_aig(const std::vector<const_twa_graph_ptr>& m_vec,
                        const char *mode,
                        const std::vector<std::string>& ins,
                        const std::vector<std::vector<std::string>>& outs,
                        const realizability_simplifier* rs)
  {
    if (m_vec.empty())
      throw std::runtime_error("mealy_machines_to_aig(): No strategy given.");

    if (m_vec.size() != outs.size())
      throw std::runtime_error("mealy_machines_to_aig(): "
                               "Expecting as many outs as strategies.");

    std::for_each(m_vec.begin()+1, m_vec.end(),
                  [usedbdd = m_vec.at(0)->get_dict()](auto&& it)
                  {
                    if (usedbdd != it->get_dict())
                      throw std::runtime_error("mealy_machines_to_aig(): "
                                               "All strategies have to "
                                               "share a bdd_dict.");
                  });

    {
      std::vector<std::string> all_ap;
      for (const auto& av : outs)
        all_ap.insert(all_ap.end(), av.begin(), av.end());
      check_double_names(all_ap, "output proposition appears in "
                                 "multiple strategies: ");
      all_ap.insert(all_ap.end(), ins.begin(), ins.end());
      check_double_names(all_ap, "Atomic propositions appears in input "
                                 "and output propositions: ");
    }

    std::vector<std::pair<const_twa_graph_ptr, bdd>> new_vec;
    new_vec.reserve(m_vec.size());

    std::set<std::string> used_aps;

    for (size_t i = 0; i < m_vec.size(); ++i)
      {
        for (const auto& f : m_vec[i]->ap())
          used_aps.insert(f.ap_name());
        // todo Some additional checks?
        new_vec.emplace_back(m_vec[i],
                             get_synthesis_outputs(m_vec[i]));
      }

        // Check if there exist outs or ins that are not used by the
    // strategy
    // ins -> simply declare them
    // outs -> set them to false
    std::vector<std::string> unused_outs;
    std::vector<std::string> unused_ins;
    for (const auto& cao : outs)
      for (const auto& ao : cao)
        if (!used_aps.count(ao))
          unused_outs.push_back(ao);
    for (const auto& ai : ins)
      if (!used_aps.count(ai))
        unused_ins.push_back(ai);

    return auts_to_aiger(new_vec, mode, unused_ins, unused_outs, rs);
  }

  aig_ptr
  mealy_machines_to_aig(const std::vector<mealy_like>& strat_vec,
                        const char* mode,
                        const std::vector<std::string>& ins,
                        const std::vector<std::vector<std::string>>& outs,
                        const realizability_simplifier* rs)
  {
    // todo extend to TGBA and possibly others
    const unsigned ns = strat_vec.size();
    std::vector<const_twa_graph_ptr> m_machines;
    m_machines.reserve(ns);
    std::vector<std::vector<std::string>> outs_used;
    outs_used.reserve(ns);

    for (unsigned i = 0; i < ns; ++i)
      {
        switch (strat_vec[i].success)
        {
        case mealy_like::realizability_code::UNREALIZABLE:
          throw std::runtime_error("mealy_machines_to_aig(): One of the "
                                   "mealy like machines is not realizable.");
        case mealy_like::realizability_code::UNKNOWN:
          throw std::runtime_error("mealy_machines_to_aig(): One of the"
                                   "mealy like objects has "
                                   "status \"unkwnown\"");
        case mealy_like::realizability_code::REALIZABLE_REGULAR:
        {
          m_machines.push_back(strat_vec[i].mealy_like);
          outs_used.push_back(outs[i]);
          break;
        }
        case mealy_like::realizability_code::REALIZABLE_DTGBA:
          throw std::runtime_error("mealy_machines_to_aig(): TGBA not "
                                   "yet supported - TBD");
        default:
          throw std::runtime_error("mealy_machines_to_aig(): Unknown "
                                   "success identifier.");
        }
      }
    return mealy_machines_to_aig(m_machines, mode, ins, outs_used, rs);
  }

  std::ostream &
  print_aiger(std::ostream &os, const_aig_ptr circuit)
  {
    if (not circuit)
      return os; //Print nothing in case of nullptr

    auto n_inputs = circuit->num_inputs();
    auto n_outputs = circuit->num_outputs();
    auto n_latches = circuit->num_latches();
    auto gates = circuit->gates();

    // Writing gates to formatted buffer speed-ups output
    // as it avoids "<<" calls
    // vars are unsigned -> 10 digits at most
    char gate_buffer[3 * 10 + 5];
    auto write_gate = [&](unsigned o, unsigned i0, unsigned i1) {
      std::snprintf(gate_buffer, sizeof gate_buffer, "%u %u %u\n", o, i0, i1);
      os << gate_buffer;
    };
    // Count active gates
    unsigned n_gates = 0;
    for (auto &g : gates)
      if ((g.first != 0) && (g.second != 0))
        ++n_gates;
    // Note max_var_ is now an upper bound
    os << "aag " << circuit->max_var() / 2
        << ' ' << n_inputs
        << ' ' << n_latches
        << ' ' << n_outputs
        << ' ' << n_gates << '\n';
    for (unsigned i = 0; i < n_inputs; ++i)
      os << (1 + i) * 2 << '\n';
    for (unsigned i = 0; i < n_latches; ++i)
      os << (1 + n_inputs + i) * 2
          << ' ' << circuit->next_latches()[i] << '\n';
    for (unsigned i = 0; i < n_outputs; ++i)
      os << circuit->outputs()[i] << '\n';
    for (unsigned i = 0; i < n_gates; ++i)
      if ((gates[i].first != 0)
          && (gates[i].second != 0))
        write_gate(circuit->gate_var(i),
                    gates[i].first,
                    gates[i].second);
    for (unsigned i = 0; i < n_inputs; ++i)
      os << 'i' << i << ' ' << circuit->input_names()[i] << '\n';
    if (n_outputs > 0)
    {
      unsigned i;
      for (i = 0; i < n_outputs - 1; ++i)
        os << 'o' << i << ' ' << circuit->output_names()[i] << '\n';
      os << 'o' << i << ' ' << circuit->output_names()[i];
    }
    return os;
  }

  std::ostream&
  print_aiger(std::ostream& os, const const_twa_graph_ptr& aut,
              const char* mode)
  {
    print_aiger(os, mealy_machine_to_aig(aut, mode));
    return os;
  }
}
