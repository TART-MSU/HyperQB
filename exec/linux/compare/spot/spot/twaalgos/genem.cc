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
#include <spot/twaalgos/genem.hh>

namespace spot
{
  namespace
  {
    enum genem_version_t { spot28, atva19, spot29, spot210, spot211, spot212 };
    static genem_version_t genem_version = spot29;
  }

  void generic_emptiness_check_select_version(const char* emversion)
  {
    if (emversion == nullptr || !strcasecmp(emversion, "spot29"))
      genem_version = spot29;
    else if (!strcasecmp(emversion, "spot212"))
      genem_version = spot212;
    else if (!strcasecmp(emversion, "spot211"))
      genem_version = spot211;
    else if (!strcasecmp(emversion, "spot210"))
      genem_version = spot210;
    else if (!strcasecmp(emversion, "spot28"))
      genem_version = spot28;
    else if (!strcasecmp(emversion, "atva19"))
      genem_version = atva19;
    else
      throw std::invalid_argument("generic_emptiness_check version should be "
                                  "one of {spot28, atva19, spot29, spot210, "
                                  "spot211, spot212}");
  }

  namespace
  {
    template <bool EarlyStop, typename Extra>
    static bool
    is_scc_empty(const scc_info& si, unsigned scc,
                 const acc_cond& autacc, Extra extra,
                 acc_cond::mark_t tocut = {});



    template <bool EarlyStop, typename Extra>
    static bool
    scc_split_check_filtered(const scc_info& upper_si, const acc_cond& acc,
                             Extra extra, acc_cond::mark_t tocut)
    {
      if constexpr (EarlyStop)
        {
          const int accepting_scc = upper_si.one_accepting_scc();
          if (accepting_scc >= 0)
            {
              if (extra)
                upper_si.get_accepting_run(accepting_scc, extra);
              return false;
            }
          if (!acc.uses_fin_acceptance())
            return true;
        }
      unsigned nscc = upper_si.scc_count();
      for (unsigned scc = 0; scc < nscc; ++scc)
        if (!is_scc_empty<EarlyStop, Extra>(upper_si, scc, acc, extra, tocut))
          if constexpr (EarlyStop)
            return false;
      return true;
    }

    template <bool EarlyStop, typename Extra>
    static bool
    scc_split_check(const scc_info& si, unsigned scc, const acc_cond& acc,
                    Extra extra, acc_cond::mark_t tocut)
    {
      if (genem_version == spot211
          || genem_version == spot212
          || genem_version == spot210)
        tocut |= acc.mafins();
      scc_and_mark_filter filt(si, scc, tocut);
      filt.override_acceptance(acc);
      scc_info upper_si(filt, EarlyStop
                        ? scc_info_options::STOP_ON_ACC
                        : scc_info_options::TRACK_STATES);
      return scc_split_check_filtered<EarlyStop>(upper_si, acc, extra, tocut);
    }

    template <bool EarlyStop, typename Extra>
    static bool
    is_scc_empty(const scc_info& si, unsigned scc,
                 const acc_cond& autacc, Extra extra,
                 acc_cond::mark_t tocut)
    {
      if (si.is_rejecting_scc(scc))
        return true;
      if constexpr (!EarlyStop)
        if (si.is_maximally_accepting_scc(scc))
          {
            extra(si, scc);
            return false;
          }
      acc_cond::mark_t sets = si.acc_sets_of(scc);
      acc_cond acc = autacc.restrict_to(sets);
      acc = acc.remove(si.common_sets_of(scc), false);

      if (SPOT_LIKELY(genem_version == spot210 || !EarlyStop))
        do
          {
            auto [fo, fpart] = acc.fin_one_extract();
            assert(fo >= 0);
            // Try to accept when Fin(fo) == true
            acc_cond::mark_t fo_m = {(unsigned) fo};
            if (!scc_split_check<EarlyStop, Extra>
                (si, scc, fpart, extra, fo_m))
              if constexpr (EarlyStop)
                return false;
            // Try to accept when Fin(fo) == false
            acc = acc.force_inf(fo_m);
          }
        while (!acc.is_f());
      else if (genem_version == spot211)
        {
          do
            {
              auto [fo, fpart, rest] = acc.fin_unit_one_split();
              acc_cond::mark_t fo_m = {(unsigned) fo};
              if (!scc_split_check<EarlyStop, Extra>
                  (si, scc, fpart, extra, fo_m))
                if constexpr (EarlyStop)
                  return false;
              acc = rest;
            }
          while (!acc.is_f());
        }
      else if (genem_version == spot212)
        {
          do
            {
              auto [fo, fpart, rest] = acc.fin_unit_one_split_improved();
              acc_cond::mark_t fo_m = {(unsigned) fo};
              if (!scc_split_check<EarlyStop, Extra>
                  (si, scc, fpart, extra, fo_m))
                if constexpr (EarlyStop)
                  return false;
              acc = rest;
            }
          while (!acc.is_f());
        }
      else if (genem_version == spot29)
        do
          {
            acc_cond::acc_code rest = acc_cond::acc_code::f();
            for (const acc_cond& disjunct: acc.top_disjuncts())
              if (acc_cond::mark_t fu = disjunct.mafins())
                {
                  if (!scc_split_check<EarlyStop, Extra>
                      (si, scc, disjunct.remove(fu, true), extra, fu))
                    if constexpr (EarlyStop)
                      return false;
                }
              else
                {
                  rest |= disjunct.get_acceptance();
                }
            if (rest.is_f())
              break;
            acc_cond subacc(acc.num_sets(), std::move(rest));
            int fo = subacc.fin_one();
            assert(fo >= 0);
            // Try to accept when Fin(fo) == true
            acc_cond::mark_t fo_m = {(unsigned) fo};
            if (!scc_split_check<EarlyStop, Extra>
                (si, scc, subacc.remove(fo_m, true), extra, fo_m))
              if constexpr (EarlyStop)
                return false;
            // Try to accept when Fin(fo) == false
            acc = subacc.force_inf(fo_m);
          }
        while (!acc.is_f());
      else
        {
          for (const acc_cond& disjunct: acc.top_disjuncts())
            if (acc_cond::mark_t fu = disjunct.fin_unit())
              {
                if (!scc_split_check<EarlyStop, Extra>
                    (si, scc, disjunct.remove(fu, true), extra, fu))
                  if constexpr (EarlyStop)
                    return false;
              }
            else
              {
                int fo = (SPOT_UNLIKELY(genem_version == spot28)
                          ? acc.fin_one() : disjunct.fin_one());
                assert(fo >= 0);
                // Try to accept when Fin(fo) == true
                acc_cond::mark_t fo_m = {(unsigned) fo};
                if (!scc_split_check<EarlyStop, Extra>
                    (si, scc, disjunct.remove(fo_m, true), extra, fo_m))
                  if constexpr (EarlyStop)
                    return false;
                // Try to accept when Fin(fo) == false
                if (!is_scc_empty<EarlyStop, Extra>
                    (si, scc, disjunct.force_inf(fo_m), extra, tocut))
                  if constexpr (EarlyStop)
                    return false;
              }
        }
      return true;
    }

    static bool
    generic_emptiness_check_main(const twa_graph_ptr& aut,
                                 twa_run_ptr run)
    {
      // We used to call cleanup_acceptance_here(aut, false),
      // but it turns out this is usually a waste of time.
      auto& aut_acc = aut->acc();
      if (aut_acc.is_f())
        return true;
      if (!aut_acc.uses_fin_acceptance())
        {
          if (!run)
            return aut->is_empty();
          if (auto p = aut->accepting_run())
            {
              *run = *p;
              return false;
            }
          return true;
        }
      // Filter with mafins() right away if possible.
      // scc_and_mark_filter will have no effect if mafins() is
      // empty.
      scc_and_mark_filter filt(aut, aut_acc.mafins());
      scc_info si(filt, scc_info_options::STOP_ON_ACC);

      const int accepting_scc = si.one_accepting_scc();
      if (accepting_scc >= 0)
        {
          if (run)
            si.get_accepting_run(accepting_scc, run);
          return false;
        }

      unsigned nscc = si.scc_count();
      for (unsigned scc = 0; scc < nscc; ++scc)
        if (!is_scc_empty<true, twa_run_ptr>(si, scc, aut_acc, run))
          return false;
      return true;
    }
  }

  bool generic_emptiness_check(const const_twa_graph_ptr& aut)
  {
    if (SPOT_UNLIKELY(!aut->is_existential()))
      throw std::runtime_error("generic_emptiness_check() "
                               "does not support alternating automata");
    auto aut_ = std::const_pointer_cast<twa_graph>(aut);
    acc_cond old = aut_->acc();
    bool res = generic_emptiness_check_main(aut_, nullptr);
    aut_->set_acceptance(old);
    return res;
  }

  twa_run_ptr generic_accepting_run(const const_twa_graph_ptr& aut)
  {
    if (SPOT_UNLIKELY(!aut->is_existential()))
      throw std::runtime_error("generic_accepting_run() "
                               "does not support alternating automata");
    auto aut_ = std::const_pointer_cast<twa_graph>(aut);
    acc_cond old = aut_->acc();
    twa_run_ptr run = std::make_shared<twa_run>(aut_);
    bool res = generic_emptiness_check_main(aut_, run);
    aut_->set_acceptance(old);
    if (!res)
      return run;
    return nullptr;
  }

  bool generic_emptiness_check_for_scc(const scc_info& si,
                                       unsigned scc)
  {
    if (si.is_accepting_scc(scc))
      return false;
    return is_scc_empty<true, twa_run_ptr>
      (si, scc, si.get_aut()->acc(), nullptr);
  }

  bool
  generic_emptiness_check_for_scc(const scc_info& si, unsigned scc,
                                  const acc_cond& forced_acc)
  {
    if (si.is_trivial(scc))
      return true;
    return scc_split_check<true, twa_run_ptr>
      (si, scc, forced_acc, nullptr, {});
  }

  bool
  maximal_accepting_loops_for_scc(const scc_info& si, unsigned scc,
                                  const acc_cond& forced_acc,
                                  const bitvect& keep,
                                  std::function<void(const scc_info&,
                                                     unsigned)> callback)
  {
    if (si.is_trivial(scc))
      return false;
    scc_and_mark_filter filt(si, scc, {}, keep);
    filt.override_acceptance(forced_acc);
    scc_info upper_si(filt, scc_info_options::TRACK_STATES);
    return !scc_split_check_filtered<false>(upper_si, forced_acc, callback, {});
  }

  // return ⊤ if there exists at least one accepting transition.
  static bool
  accepting_transitions_aux(const scc_info &si, unsigned scc,
                                const acc_cond acc,
                                acc_cond::mark_t removed_colors,
                                acc_cond::mark_t tocut,
                                std::vector<bool> &accepting_transitions,
                                const bitvect& kept)
  {
    bool result = false;
    scc_and_mark_filter filt(si, scc, tocut, kept);
    filt.override_acceptance(acc);
    scc_info upper_si(filt, scc_info_options::ALL);
    for (unsigned sc = 0; sc < upper_si.scc_count(); ++sc)
      result |= accepting_transitions_scc(upper_si, sc, acc, removed_colors,
                                accepting_transitions, kept);
    return result;
  }

  bool
  accepting_transitions_scc(const scc_info &si, unsigned scc,
                              const acc_cond aut_acc,
                              acc_cond::mark_t removed_colors,
                              std::vector<bool>& accepting_transitions,
                              const bitvect& kept)
  {
    // The idea is the same as in is_scc_empty()
    bool result = false;
    acc_cond::mark_t sets = si.acc_sets_of(scc);
    acc_cond acc = aut_acc.restrict_to(sets);
    acc = acc.remove(si.common_sets_of(scc), false);

    auto inner_edges = si.inner_edges_of(scc);

    if (si.is_trivial(scc))
      return false;
    if (acc.is_t() || acc.accepting(acc.get_acceptance().used_sets()))
    {
      for (auto& e : inner_edges)
        if ((e.acc & removed_colors) == acc_cond::mark_t {})
          accepting_transitions[si.get_aut()->edge_number(e)] = true;
      return true;
    }
    else if (acc.is_f())
      return false;
    acc_cond::acc_code rest = acc_cond::acc_code::f();
    for (const acc_cond& disjunct: acc.top_disjuncts())
      if (acc_cond::mark_t fu = disjunct.fin_unit())
        result |= accepting_transitions_aux(si, scc, acc.remove(fu, true),
                                  (removed_colors | fu), fu,
                                  accepting_transitions, kept);
      else
        rest |= disjunct.get_acceptance();
    if (!rest.is_f())
    {
      acc_cond::mark_t m = { (unsigned) acc.fin_one() };
      result |= accepting_transitions_aux(si, scc, acc.remove(m, true),
                                (removed_colors | m), m, accepting_transitions,
                                kept);
      result |= accepting_transitions_scc(si, scc, acc.remove(m, false),
                                removed_colors, accepting_transitions,
                                kept);
    }
    return result;
  }

  std::vector<bool>
  accepting_transitions(const const_twa_graph_ptr aut, acc_cond cond)
  {
    auto aut_vector_size = aut->edge_vector().size();
    std::vector<bool> result(aut_vector_size, false);
    auto kept = make_bitvect(aut_vector_size);
    scc_info si(aut);
    for (unsigned scc = 0; scc < si.scc_count(); ++scc)
      accepting_transitions_scc(si, scc, cond, {}, result, *kept);
    delete kept;
    return result;
  }
}
