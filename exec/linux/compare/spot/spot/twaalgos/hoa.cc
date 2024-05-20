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
#include <ostream>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <map>
#include <spot/twa/twa.hh>
#include <spot/twa/twagraph.hh>
#include <spot/twaalgos/hoa.hh>
#include <spot/misc/escape.hh>
#include <spot/misc/bddlt.hh>
#include <spot/misc/minato.hh>
#include <spot/tl/formula.hh>
#include <spot/kripke/fairkripke.hh>
#include <spot/kripke/kripkegraph.hh>
#include <spot/twaalgos/split.hh>

using namespace std::string_literals;

namespace spot
{
  hoa_alias_formater::hoa_alias_formater(const const_twa_graph_ptr& aut,
                                         const char* false_str,
                                         const char* true_str,
                                         const char* or_str,
                                         const char* and_str,
                                         const char* not_str,
                                         const char* lpar_str,
                                         const char* rpar_str,
                                         std::function<std::string(int)>
                                         ap_printer)
    : false_str_(false_str), true_str_(true_str), or_str_(or_str),
      and_str_(and_str), not_str_(not_str), lpar_str_(lpar_str),
      rpar_str_(rpar_str), ap_printer_(ap_printer)
  {
    auto aliases = aliases_ = aut->get_named_prop<aliases_t>("aliases");
    if (!aliases)
      return;
    bdd sup = aut->ap_vars();
    // Remove all aliases that use variables that are not
    // registered in this automaton.
    auto badvar = [sup](std::pair<std::string, bdd>& p) {
      return bdd_exist(bdd_support(p.second), sup) != bddtrue;
    };
    aliases->erase(std::remove_if(aliases->begin(),
                                  aliases->end(),
                                  badvar),
                   aliases->end());
    unsigned count = aliases->size();
    for (unsigned i = 0; i < count; ++i)
      {
        bdd a = (*aliases)[i].second;
        aliases_map_[a.id()] = i;
        if (bdd_is_cube(a))
          alias_cubes_.emplace_back(a, i);
        bdd neg = !a;
        // do not overwrite an existing alias with a negation
        aliases_map_.emplace(neg.id(), i);
        if (bdd_is_cube(neg))
          alias_cubes_.emplace_back(neg, i);
      }
  }

  std::string
  hoa_alias_formater::encode_label(bdd label, unsigned aliases_start)
  {
    if (aliases_)
      // Check if we have a perfect alias match for this label.
      if (auto p = aliases_map_.find(label.id()); p != aliases_map_.end())
        if (unsigned pos = p->second; pos >= aliases_start)
          {
            auto& a = (*aliases_)[pos];
            if (a.second == label)
              return '@' + a.first;
            else
              return std::string(not_str_) + '@' + a.first;
          }
    if (label == bddtrue)
      return true_str_;
    if (label == bddfalse)
      return false_str_;
    std::ostringstream s;
    bool notfirstor = false;
    if (aliases_)
      {
        bdd orig_label = label;
        // If we have some aliases that imply the label, we can use them
        // and try to cover most of it as a sum of labels.
        unsigned alias_count = aliases_->size();
        for (unsigned i = aliases_start; i < alias_count; ++i)
          if (auto& a = (*aliases_)[i]; bdd_implies(a.second, orig_label))
            {
              bdd rest = label - a.second;
              if (rest != label)
                {
                  if (notfirstor)
                    s << or_str_;
                  s << '@' << a.first;
                  notfirstor = true;
                  label = rest;
                  if (label == bddfalse)
                    return s.str();
                }
            }
        // If the label was not completely translated as a
        // disjunction of aliases, maybe we can see it as a
        // conjunction?  Let's try.
        // We first try to remove cubes, from the labels, and then
        // try to cover the rest with non-cubes.
        {
          std::ostringstream s2;
          bdd labelconj = orig_label; // start again
          bool notfirstand = false;
          unsigned alias_count = aliases_->size();
          // first pass using only cubes
          for (auto [acube, i]: alias_cubes_)
            if (i >= aliases_start)
              if (bdd_implies(orig_label, acube))
                if (bdd rest = bdd_exist(labelconj, bdd_support(acube));
                    rest != labelconj)
                  {
                    auto& a = (*aliases_)[i];
                    if (notfirstand)
                      s2 << and_str_;
                    if (acube != a.second)
                      s2 << not_str_;
                    s2 << '@' << a.first;
                    notfirstand = true;
                    labelconj = rest;
                    if (labelconj == bddtrue)
                      return s2.str();
                  }
          // second pass using all non-cube aliases
          for (unsigned i = aliases_start; i < alias_count; ++i)
            {
              auto& a = (*aliases_)[i];
              bdd neg = !a.second;
              if (!bdd_is_cube(a.second)
                  && bdd_implies(orig_label, a.second))
                {
                  bdd rest = labelconj | neg;
                  if (rest != labelconj)
                    {
                      if (notfirstand)
                        s2 << and_str_;
                      s2 << '@' << a.first;
                      notfirstand = true;
                      labelconj = rest;
                      if (labelconj == bddtrue)
                        return s2.str();
                    }
                }
              else if (!bdd_is_cube(neg)
                       && bdd_implies(orig_label, neg))
                {
                  bdd rest = labelconj | a.second;
                  if (rest != labelconj)
                    {
                      if (notfirstand)
                        s2 << and_str_;
                      s2 << not_str_ << '@' << a.first;
                      notfirstand = true;
                      labelconj = rest;
                      if (labelconj == bddtrue)
                        return s2.str();
                    }
                }
            }
          // If we did not manage to make it look like a
          // conjunction of aliases, let's continue with
          // our (possibly partial) disjunction.
        }
      }
    minato_isop isop(label);
    bdd cube = isop.next();
    std::ostringstream s3;
    while (cube != bddfalse)
      {
        if (notfirstor)
          s << or_str_;
        bool notfirstand = false;
        s3.str("");
        if (aliases_)
          {
            // We know that cube did not match any aliases.  But
            // maybe it can be built as a conjunction of aliases?
            // (or negated aliases)
            bdd orig_cube = cube;
            for (auto [acube, i]: alias_cubes_)
              if (i >= aliases_start)
                if (bdd_implies(orig_cube, acube))
                  if (bdd rest = bdd_exist(cube, bdd_support(acube));
                      rest != cube)
                    {
                      if (notfirstand)
                        s3 << and_str_;
                      if (acube != (*aliases_)[i].second)
                        s3 << not_str_;
                      s3 << '@' << (*aliases_)[i].first;
                      cube = rest;
                      if (cube == bddtrue)
                        break;
                      notfirstand = true;
                    }
          }
        while (cube != bddtrue)
          {
            if (notfirstand)
              s3 << and_str_;
            bdd h = bdd_high(cube);
            if (h == bddfalse)
              {
                s3 << not_str_ << ap_printer_(bdd_var(cube));
                cube = bdd_low(cube);
              }
            else
              {
                s3 << ap_printer_(bdd_var(cube));
                cube = h;
              }
            if (cube == bddtrue)
              break;
            notfirstand = true;
          }
        bdd nextcube = isop.next();

        if (notfirstand && (notfirstor || nextcube != bddfalse))
          s << lpar_str_ << s3.str() << rpar_str_;
        else
          s << s3.str();
        cube = nextcube;
        notfirstor = true;
      }
    return s.str();
  }

  namespace
  {
    struct metadata final
    {
      // Assign a number to each atomic proposition.
      typedef std::map<int, unsigned> ap_map;
      ap_map ap;
      typedef std::vector<int> vap_t;
      vap_t vap;

      std::vector<bool> common_acc;
      bool has_state_acc;
      bool is_complete;
      bool is_universal;
      bool is_colored;
      bool use_implicit_labels;
      bool use_state_labels = true;
      bdd all_ap;
      hoa_alias_formater haf;

      // Label support: the set of all conditions occurring in the
      // automaton.
      typedef std::map<bdd, std::string, bdd_less_than> sup_map;
      sup_map sup;

      metadata(const const_twa_graph_ptr& aut, bool implicit,
               bool state_labels)
        : haf(aut, "f", "t", " | ", "&", "!", "", "",
              [&ap=this->ap](int num){ return std::to_string(ap[num]); })
      {
        check_det_and_comp(aut);
        use_implicit_labels = implicit && is_universal && is_complete;
        use_state_labels &= state_labels;
        number_all_ap(aut);
      }

      std::ostream&
      emit_acc(std::ostream& os, acc_cond::mark_t b)
      {
        // FIXME: We could use a cache for this.
        if (!b)
          return os;
        os << " {";
        bool notfirst = false;
        for (auto v: b.sets())
          {
            if (notfirst)
              os << ' ';
            else
              notfirst = true;
            os << v;
          }
        os << '}';
        return os;
      }

      void check_det_and_comp(const const_twa_graph_ptr& aut)
      {
        std::string empty;

        unsigned ns = aut->num_states();
        bool universal = true;
        bool complete = true;
        bool state_acc = true;
        bool nodeadend = true;
        bool colored = aut->num_sets() >= 1;
        for (unsigned src = 0; src < ns; ++src)
          {
            bdd sum = bddfalse;
            bdd available = bddtrue;
            bool st_acc = true;
            bool notfirst = false;
            acc_cond::mark_t prev = {};
            bool has_succ = false;
            bdd lastcond = bddfalse;
            for (auto& t: aut->out(src))
              {
                if (has_succ)
                  use_state_labels &= lastcond == t.cond;
                else
                  lastcond = t.cond;
                if (complete)
                  sum |= t.cond;
                if (universal)
                  {
                    if (!bdd_implies(t.cond, available))
                      universal = false;
                    else
                      available -= t.cond;
                  }
                sup.insert(std::make_pair(t.cond, empty));
                if (st_acc)
                  {
                    if (notfirst && prev != t.acc)
                      {
                        st_acc = false;
                      }
                    else
                      {
                        notfirst = true;
                        prev = t.acc;
                      }
                  }
                if (colored)
                  {
                    auto a = t.acc;
                    if (!a || a.remove_some(1))
                      colored = false;
                  }
                has_succ = true;
              }
            nodeadend &= has_succ;
            if (complete)
              complete &= sum == bddtrue;
            common_acc.push_back(st_acc);
            state_acc &= st_acc;
          }
        is_universal = universal;
        is_complete = complete;
        has_state_acc = state_acc;
        // If the automaton has state-based acceptance and contain
        // some states without successors do not declare it as
        // colored.
        is_colored = colored && (!has_state_acc || nodeadend);
        // If the automaton declares that it is universal or
        // state-based, make sure that it really is.
        if (aut->prop_universal().is_true() && !universal)
          throw std::runtime_error("print_hoa(): automaton is not universal"
                                   " but prop_universal()==true");
        if (aut->prop_universal().is_false() && universal)
          throw std::runtime_error("print_hoa(): automaton is universal"
                                   " despite prop_universal()==false");
        if (aut->prop_complete().is_true() && !complete)
          throw std::runtime_error("print_hoa(): automaton is not complete"
                                   " but prop_complete()==true");
        if (aut->prop_complete().is_false() && complete)
          throw std::runtime_error("print_hoa(): automaton is complete"
                                   " but prop_complete()==false");
        if (aut->prop_state_acc() && !state_acc)
          throw std::runtime_error("print_hoa(): automaton has "
                                   "transition-based acceptance despite"
                                   " prop_state_acc()==true");
      }

      std::string encode_label(bdd label, unsigned aliases_start = 0)
      {
        return haf.encode_label(label, aliases_start);
      }

      void number_all_ap(const const_twa_graph_ptr& aut)
      {
        // Make sure that the automaton uses only atomic propositions
        // that have been registered via twa::register_ap() or some
        // variant.  If that is not the case, it is a bug that should
        // be fixed in the function creating the automaton.  Since
        // that function could be written by the user, throw an
        // exception rather than using an assert().
        bdd all = bddtrue;
        for (auto& i: sup)
          all &= bdd_support(i.first);
        all_ap = aut->ap_vars();
        // Complains if the transitions use variables that hasn't been
        // declared in the AP set of the automaton.  The error seems
        // to be frequent enough to warrant a longer message.  We only
        // do this diagnostic in the HOA printer and not in the dot
        // printer so that we still have one way to print the
        // automaton for debugging.
        if (bdd_exist(all, all_ap) != bddtrue)
          throw std::runtime_error
            ("print_hoa(): automaton uses unregistered atomic propositions\n\n"
             "This error usually occurs if the algorithm that created "
             "this automaton is\nmissing a call to copy_ap_of() or "
             "register_ap().  If you believe this is\na bug in Spot itself, "
             "please email <spot@lrde.epita.fr>");
        all = all_ap;

        while (all != bddtrue)
          {
            int v = bdd_var(all);
            all = bdd_high(all);
            ap.insert(std::make_pair(v, vap.size()));
            vap.emplace_back(v);
          }

        if (use_implicit_labels)
          return;

        for (auto& i: sup)
          i.second = encode_label(i.first);
      }
    };

  }

  enum hoa_acceptance
    {
      Hoa_Acceptance_States,        /// state-based acceptance if
                                /// (globally) possible
                                /// transition-based acceptance
                                /// otherwise.
      Hoa_Acceptance_Transitions, /// transition-based acceptance globally
      Hoa_Acceptance_Mixed    /// mix state-based and transition-based
    };

  static std::ostream&
  print_hoa(std::ostream& os,
                const const_twa_graph_ptr& aut,
                const char* opt)
  {
    bool newline = true;
    hoa_acceptance acceptance = Hoa_Acceptance_States;
    bool implicit_labels = false;
    bool verbose = false;
    bool state_labels = false;
    bool v1_1 = false;
    bool alias_basis = false;

    if (opt)
      while (*opt)
        {
          switch (char c = *opt++)
            {
            case '1':
              if (opt[0] == '.' && opt[1] == '1')
                {
                  v1_1 = true;
                  opt += 2;
                }
              else if (opt[0] == '.' && opt[1] == '0')
                {
                  v1_1 = false;
                  opt += 2;
                }
              else
                {
                  v1_1 = false;
                }
              break;
            case 'b':
              alias_basis = true;
              break;
            case 'i':
              implicit_labels = true;
              break;
            case 'k':
              state_labels = true;
              break;
            case 'K':
              state_labels = false;
              break;
            case 'l':
              newline = false;
              break;
            case 'm':
              acceptance = Hoa_Acceptance_Mixed;
              break;
            case 's':
              acceptance = Hoa_Acceptance_States;
              break;
            case 't':
              acceptance = Hoa_Acceptance_Transitions;
              break;
            case 'v':
              verbose = true;
              break;
            default:
              throw std::runtime_error("unknown option for print_hoa(): "s + c);
            }
        }

    if (SPOT_UNLIKELY(aut->prop_weak().is_false()
                      && (aut->acc().is_t() || aut->acc().is_f())))
      throw std::runtime_error("print_hoa(): automaton is declared not weak, "
                               "but the acceptance makes this impossible");

    // If we were asked to create an alias basis, make sure we save
    // existing aliases, so we can restore it before we exit this
    // function.
    std::vector<std::pair<std::string, bdd>> old_aliases;
    if (aut->ap().size() <= 1)
      alias_basis = false;
    if (alias_basis)
      {
        if (auto* aliases = get_aliases(aut))
          old_aliases = *aliases;
        create_alias_basis(std::const_pointer_cast<twa_graph>(aut));
      }
    // restore the old aliases using a unique_ptr-based scope guard,
    // because there are too many ways to exit this function.
    auto restore_aliases = [&old_aliases, alias_basis, aut](void*) {
      if (alias_basis)
        set_aliases(std::const_pointer_cast<twa_graph>(aut), old_aliases);
    };
    std::unique_ptr<void, decltype(restore_aliases)>
      restore_aliases_guard((void*)1, restore_aliases);

    metadata md(aut, implicit_labels, state_labels);

    if (acceptance == Hoa_Acceptance_States && !md.has_state_acc)
      acceptance = Hoa_Acceptance_Transitions;

    auto print_dst = [&os, &aut](unsigned dst)
      {
        bool notfirst = false;
        for (unsigned d: aut->univ_dests(dst))
          {
            if (notfirst)
              os << '&';
            else
              notfirst = true;
            os << d;
          }
      };

    unsigned num_states = aut->num_states();
    unsigned init = aut->get_init_state_number();

    const char nl = newline ? '\n' : ' ';
    os << (v1_1 ? "HOA: v1.1" : "HOA: v1") << nl;
    auto n = aut->get_named_prop<std::string>("automaton-name");
    if (n)
      escape_str(os << "name: \"", *n) << '"' << nl;
    unsigned nap = md.vap.size();
    os << "States: " << num_states << nl
       << "Start: ";
    print_dst(init);
    os << nl
       << "AP: " << nap;
    auto d = aut->get_dict();
    for (auto& i: md.vap)
      escape_str(os << " \"", d->bdd_map[i].f.ap_name()) << '"';
    os << nl;

    unsigned num_acc = aut->num_sets();
    acc_cond::acc_code acc_c = aut->acc().get_acceptance();
    if (aut->acc().is_generalized_buchi())
      {
        if (aut->acc().is_all())
          os << "acc-name: all";
        else if (aut->acc().is_buchi())
          os << "acc-name: Buchi";
        else
          os << "acc-name: generalized-Buchi " << num_acc;
        os << nl;
      }
    else if (aut->acc().is_generalized_co_buchi())
      {
        if (aut->acc().is_none())
          os << "acc-name: none";
        else if (aut->acc().is_co_buchi())
          os << "acc-name: co-Buchi";
        else
          os << "acc-name: generalized-co-Buchi " << num_acc;
        os << nl;
      }
    else
      {
        int r = aut->acc().is_rabin();
        assert(r != 0);
        if (r > 0)
          {
            os << "acc-name: Rabin " << r << nl;
            // Force the acceptance to remove any duplicate sets, and
            // make sure it is correctly ordered.
            acc_c = acc_cond::acc_code::rabin(r);
          }
        else
          {
            r = aut->acc().is_streett();
            assert(r != 0);
            if (r > 0)
              {
                os << "acc-name: Streett " << r << nl;
                // Force the acceptance to remove any duplicate sets, and
                // make sure it is correctly ordered.
                acc_c = acc_cond::acc_code::streett(r);
              }
            else
              {
                std::vector<unsigned> pairs;
                if (aut->acc().is_generalized_rabin(pairs))
                  {
                    os << "acc-name: generalized-Rabin " << pairs.size();
                    for (auto p: pairs)
                      os << ' ' << p;
                    os << nl;
                    // Force the acceptance to remove any duplicate
                    // sets, and make sure it is correctly ordered.
                    acc_c = acc_cond::acc_code::generalized_rabin(pairs.begin(),
                                                                  pairs.end());
                  }
                else
                  {
                    bool max = false;
                    bool odd = false;
                    if (aut->acc().is_parity(max, odd))
                      os << "acc-name: parity "
                         << (max ? "max " : "min ")
                         << (odd ? "odd " : "even ")
                         << num_acc << nl;
                  }
              }
          }
      }
    os << "Acceptance: " << num_acc << ' ';
    os << acc_c;
    os << nl;
    os << "properties:";
    // Make sure the property line is not too large,
    // otherwise our test cases do not fit in 80 columns...
    unsigned prop_len = 60;
    auto prop = [&](const char* str)
      {
        if (newline)
          {
            auto l = strlen(str);
            if (prop_len < l)
              {
                prop_len = 60;
                os << "\nproperties:";
              }
            prop_len -= l;
          }
        os << str;
      };
    implicit_labels = md.use_implicit_labels;
    state_labels = md.use_state_labels;
    if (implicit_labels)
      prop(" implicit-labels");
    else if (state_labels)
      prop(" state-labels explicit-labels");
    else
      prop(" trans-labels explicit-labels");
    if (acceptance == Hoa_Acceptance_States)
      prop(" state-acc");
    else if (acceptance == Hoa_Acceptance_Transitions)
      prop(" trans-acc");
    if (md.is_colored)
      prop(" colored");
    else if (verbose && v1_1)
      prop(" !colored");
    if (md.is_complete)
      prop(" complete");
    else if (v1_1)
      prop(" !complete");
    // The definition of "deterministic" was changed between HOA v1
    // (were it meant "universal") and HOA v1.1 were it means
    // ("universal" and "existential").
    if (!v1_1)
      {
        if (md.is_universal)
          prop(" deterministic");
        // It's probable that nobody cares about the "no-univ-branch"
        // property.  The "univ-branch" property seems more important to
        // announce that the automaton might not be parsable by tools that
        // do not support alternating automata.
        if (!aut->is_existential())
          {
            prop(" univ-branch");
          }
        else if (verbose)
          {
            if (v1_1)
              prop(" !univ-branch");
            else
              prop(" no-univ-branch");
          }
      }
    else
      {
        if (md.is_universal && aut->is_existential())
          {
            prop(" deterministic");
            if (verbose)
              prop(" !univ-branch !exist-branch");
          }
        else
          {
            prop(" !deterministic");
            if (!aut->is_existential())
              prop(" univ-branch");
            else if (verbose)
              prop(" !univ-branch");
            if (!md.is_universal)
              prop(" exist-branch");
            else if (verbose)
              prop(" !exist-branch");
          }
      }
    // Deterministic automata are also unambiguous, so writing both
    // properties seems redundant.  People working on unambiguous
    // automata are usually concerned about non-deterministic
    // unambiguous automata.  So do not mention "unambiguous"
    // in the case of deterministic automata.
    if (aut->prop_unambiguous() && (verbose || !md.is_universal))
      prop(" unambiguous");
    else if (v1_1 && !aut->prop_unambiguous())
      prop(" !unambiguous");
    if (aut->prop_semi_deterministic() && (verbose || !md.is_universal))
      prop(" semi-deterministic");
    else if (v1_1 && !aut->prop_semi_deterministic())
      prop(" !semi-deterministic");
    if (aut->prop_stutter_invariant())
      prop(" stutter-invariant");
    if (!aut->prop_stutter_invariant())
      {
        if (v1_1)
          prop(" !stutter-invariant");
        else
          prop(" stutter-sensitive");
      }
    if (aut->prop_terminal())
      prop(" terminal");
    if (aut->prop_very_weak())
      prop(" very-weak");
    if (aut->prop_weak() && (verbose || (aut->prop_terminal() != true &&
                                         aut->prop_very_weak() != true)))
      prop(" weak");
    if (aut->prop_inherently_weak() && (verbose || aut->prop_weak() != true))
      prop(" inherently-weak");
    if (v1_1 && !aut->prop_terminal() && (verbose || aut->prop_weak() != false))
      prop(" !terminal");
    if (v1_1 && !aut->prop_very_weak() && (verbose
                                           || aut->prop_weak() != false))
      prop(" !very-weak");
    if (v1_1 && !aut->prop_weak() && (verbose ||
                                      aut->prop_inherently_weak() != false))
      prop(" !weak");
    if (v1_1 && !aut->prop_inherently_weak())
      prop(" !inherently-weak");
    os << nl;

    // highlighted states and edges are only output in the 1.1 format,
    // because we use a dot in the header name.
    if (v1_1)
      {
        if (auto hstates = aut->get_named_prop
            <std::map<unsigned, unsigned>>("highlight-states");
            hstates && !hstates->empty())
          {
            os << "spot.highlight.states:";
            for (auto& p: *hstates)
              os << ' ' << p.first << ' ' << p.second;
            os << nl;
          }
        if (auto hedges = aut->get_named_prop
            <std::map<unsigned, unsigned>>("highlight-edges");
            hedges && !hedges->empty())
          {
            // Numbering edges is a delicate process.  The
            // "highlight-edges" property uses edges numbers that are
            // indices in the "edges" vector.  However these edges
            // need not be sorted.  When edges are output in HOA, they
            // are output with increasing source state number, and the
            // edges number expected in the HOA file should use that
            // order.  So we need to make a first pass on the
            // automaton to number all edges as they will be output.
            unsigned maxedge = aut->edge_vector().size();
            std::vector<unsigned> renum(maxedge);
            unsigned edge = 0;
            for (unsigned i = 0; i < num_states; ++i)
              for (auto& t: aut->out(i))
                renum[aut->get_graph().index_of_edge(t)] = ++edge;
            os << "spot.highlight.edges:";
            for (auto& p: *hedges)
              if (p.first < maxedge) // highlighted edges could come from user
                os << ' ' << renum[p.first] << ' ' << p.second;
            os << nl;
          }
      }
    if (auto word = aut->get_named_prop<std::string>("accepted-word"))
      {
        os << (v1_1 ? "spot." : "spot-") << "accepted-word: \"";
        escape_str(os, *word) << '"' << nl;
      }
    if (auto word = aut->get_named_prop<std::string>("rejected-word"))
      {
        os << (v1_1 ? "spot." : "spot-") << "rejected-word: \"";
        escape_str(os, *word) << '"' << nl;
      }
    if (auto player = aut->get_named_prop<std::vector<bool>>("state-player"))
      {
        os << (v1_1 ? "spot." : "spot-") << "state-player:";
        if (player->size() != num_states)
          throw std::runtime_error("print_hoa(): state-player property has"
                                   " " + std::to_string(player->size()) +
                                   " states but automaton has " +
                                   std::to_string(num_states));
        unsigned n = 0;
        while (n < num_states)
          {
            os << ' ' << (*player)[n];
            ++n;
            if (newline && n < num_states && (n % 30 == 0))
              os << "\n                  ";
          }
        os << nl;
      }
    if (auto synout = aut->get_named_prop<bdd>("synthesis-outputs"))
      {
        bdd vars = bdd_support(*synout);
        os << "controllable-AP:";
        while (vars != bddtrue)
          {
            int v = bdd_var(vars);
            vars = bdd_high(vars);
            if (auto p = md.ap.find(v); p != md.ap.end())
              os << ' ' << p->second;
            else
              throw std::runtime_error("print_hoa(): synthesis-outputs has "
                                       "unregistered proposition");
          }
        os << nl;
      }
    if (auto* aliases = md.haf.aliases())
      {
        int cnt = aliases->size();
        for (int i = cnt - 1; i >= 0; --i)
          os << "Alias: @" << (*aliases)[i].first << ' '
             << md.encode_label((*aliases)[i].second, i + 1) << nl;
      }

    // If we want to output implicit labels, we have to
    // fill a vector with all destinations in order.
    std::vector<unsigned> out;
    std::vector<acc_cond::mark_t> outm;
    if (implicit_labels)
      {
        out.resize(1UL << nap);
        if (acceptance != Hoa_Acceptance_States)
          outm.resize(1UL << nap);
      }

    os << "--BODY--" << nl;

    auto sn = aut->get_named_prop<std::vector<std::string>>("state-names");
    for (unsigned i = 0; i < num_states; ++i)
      {
        hoa_acceptance this_acc = acceptance;
        if (this_acc == Hoa_Acceptance_Mixed)
          this_acc = (md.common_acc[i] ?
                      Hoa_Acceptance_States : Hoa_Acceptance_Transitions);

        os << "State: ";
        if (state_labels)
          {
            bool output = false;
            for (auto& t: aut->out(i))
              {
                os << '[' << md.sup[t.cond] << "] ";
                output = true;
                break;
              }
            if (!output)
              os << "[f] ";
          }
        os << i;
        if (sn && i < sn->size() && !(*sn)[i].empty())
          os << " \"" << (*sn)[i] << '"';
        if (this_acc == Hoa_Acceptance_States)
          {
            acc_cond::mark_t acc = {};
            for (auto& t: aut->out(i))
              {
                acc = t.acc;
                break;
              }
            md.emit_acc(os, acc);
          }
        os << nl;

        if (!implicit_labels && !state_labels)
          {

            for (auto& t: aut->out(i))
              {
                os << '[' << md.sup[t.cond] << "] ";
                print_dst(t.dst);
                if (this_acc == Hoa_Acceptance_Transitions)
                  md.emit_acc(os, t.acc);
                os << nl;
              }
          }
        else if (state_labels)
          {
            unsigned n = 0;
            for (auto& t: aut->out(i))
              {
                print_dst(t.dst);
                if (this_acc == Hoa_Acceptance_Transitions)
                  {
                    md.emit_acc(os, t.acc);
                    os << nl;
                  }
                else
                  {
                    ++n;
                    os << (((n & 15) && t.next_succ) ? ' ' : nl);
                  }
              }
          }
        else
          {
            for (auto& t: aut->out(i))
              for (bdd one: minterms_of(t.cond, md.all_ap))
                {
                  unsigned level = 1;
                  unsigned pos = 0U;
                  while (one != bddtrue)
                    {
                      bdd h = bdd_high(one);
                      if (h == bddfalse)
                        {
                          one = bdd_low(one);
                        }
                      else
                        {
                          pos |= level;
                          one = h;
                        }
                      level <<= 1;
                    }
                  out[pos] = t.dst;
                  if (this_acc != Hoa_Acceptance_States)
                    outm[pos] = t.acc;
                }
            unsigned n = out.size();
            for (unsigned i = 0; i < n;)
              {
                print_dst(out[i]);
                if (this_acc != Hoa_Acceptance_States)
                  {
                    md.emit_acc(os, outm[i]) << nl;
                    ++i;
                  }
                else
                  {
                    ++i;
                    os << (((i & 15) && i < n) ? ' ' : nl);
                  }
              }
          }
      }
    os << "--END--";                // No newline.  Let the caller decide.
    return os;
  }

  std::ostream&
  print_hoa(std::ostream& os,
            const const_twa_ptr& aut,
            const char* opt)
  {
    bool preserve_names = false;
    // for Kripke structures, automatically append "k" to the options.
    // (Unless "K" was given.)
    char* tmpopt = nullptr;
    if (std::dynamic_pointer_cast<const fair_kripke>(aut) &&
        (!opt || (strchr(opt, 'K') == nullptr)))
      {
        unsigned n = opt ? strlen(opt) : 0;
        tmpopt = new char[n + 2];
        if (opt)
          strcpy(tmpopt, opt);
        tmpopt[n] = 'k';
        tmpopt[n + 1] = 0;
        // Preserve names if we have some state names, or if we are
        // not a kripke_graph.
        auto sn = aut->get_named_prop<std::vector<std::string>>("state-names");
        preserve_names =
          !!sn || !std::dynamic_pointer_cast<const kripke_graph>(aut);
      }

    auto a = std::dynamic_pointer_cast<const twa_graph>(aut);
    if (!a)
      a = make_twa_graph(aut, twa::prop_set::all(), preserve_names);

    print_hoa(os, a, tmpopt ? tmpopt : opt);
    delete[] tmpopt;
    return os;
  }

  std::vector<std::pair<std::string, bdd>>*
  get_aliases(const const_twa_ptr& g)
  {
    return
      g->get_named_prop<std::vector<std::pair<std::string, bdd>>>("aliases");
  }

  void
  set_aliases(twa_ptr g,
              const std::vector<std::pair<std::string, bdd>>& aliases)
  {
    if (aliases.empty())
      {
        g->set_named_prop("aliases", nullptr);
      }
    else
      {
        auto a = g->get_or_set_named_prop
          <std::vector<std::pair<std::string, bdd>>>("aliases");
        *a = aliases;
      }
  }

  void
  create_alias_basis(const twa_graph_ptr& aut)
  {
    edge_separator es;
    es.add_to_basis(aut);
    std::vector<std::pair<std::string, bdd>> aliases;
    unsigned n = 0;
    for (bdd b: es.basis())
      aliases.emplace_back(std::to_string(n++), b);
    std::reverse(aliases.begin(), aliases.end());
    set_aliases(aut, aliases);
  }

}
