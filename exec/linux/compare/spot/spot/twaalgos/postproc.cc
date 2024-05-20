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
#include <spot/twaalgos/postproc.hh>
#include <spot/twaalgos/minimize.hh>
#include <spot/twaalgos/simulation.hh>
#include <spot/twaalgos/sccfilter.hh>
#include <spot/twaalgos/degen.hh>
#include <spot/twaalgos/stripacc.hh>
#include <cstdlib>
#include <spot/misc/optionmap.hh>
#include <spot/twaalgos/powerset.hh>
#include <spot/twaalgos/isdet.hh>
#include <spot/twaalgos/dtbasat.hh>
#include <spot/twaalgos/dtwasat.hh>
#include <spot/twaalgos/complete.hh>
#include <spot/twaalgos/totgba.hh>
#include <spot/twaalgos/sbacc.hh>
#include <spot/twaalgos/sepsets.hh>
#include <spot/twaalgos/determinize.hh>
#include <spot/twaalgos/alternation.hh>
#include <spot/twaalgos/parity.hh>
#include <spot/twaalgos/cobuchi.hh>
#include <spot/twaalgos/cleanacc.hh>
#include <spot/twaalgos/toparity.hh>
#include <spot/twaalgos/zlktree.hh>

namespace spot
{
  namespace
  {
    static twa_graph_ptr
    ensure_ba(twa_graph_ptr& a, bool no_trivial)
    {
      if (a->acc().is_t())
        {
          auto m = a->set_buchi();
          if (!no_trivial)
            {
              for (auto& t: a->edges())
                t.acc = m;
            }
          else
            {
              scc_info si(a);
              unsigned nc = si.scc_count();
              for (unsigned i = 0; i < nc; ++i)
                // Cannot use "is_accepting_scc" because the
                // acceptance condition was already changed.
                if (!si.is_trivial(i))
                  for (auto& e: si.edges_of(i))
                    const_cast<acc_cond::mark_t&>(e.acc) = m;
            }
          a->prop_state_acc(true);
        }
      return a;
    }
  }

  postprocessor::postprocessor(const option_map* opt)
  {
    if (opt)
      {
        degen_order_ = opt->get("degen-order", 0);
        degen_reset_ = opt->get("degen-reset", 1);
        degen_cache_ = opt->get("degen-lcache", 1);
        degen_lskip_ = opt->get("degen-lskip", 1);
        degen_lowinit_ = opt->get("degen-lowinit", 0);
        degen_remscc_ = opt->get("degen-remscc", 1);
        det_scc_ = opt->get("det-scc", 1);
        det_simul_ = opt->get("det-simul", -1);
        det_stutter_ = opt->get("det-stutter", 1);
        det_max_states_ = opt->get("det-max-states", -1);
        det_max_edges_ = opt->get("det-max-edges", -1);
        simul_ = opt->get("simul", -1);
        simul_method_ = opt->get("simul-method", -1);
        dpa_simul_ = opt->get("dpa-simul", -1);
        dba_simul_ = opt->get("dba-simul", -1);
        scc_filter_ = opt->get("scc-filter", -1);
        ba_simul_ = opt->get("ba-simul", -1);
        tba_determinisation_ = opt->get("tba-det", 0);
        sat_minimize_ = opt->get("sat-minimize", 0);
        sat_incr_steps_ = opt->get("sat-incr-steps", -2); // -2 or any num < -1
        sat_langmap_ = opt->get("sat-langmap", 0);
        sat_acc_ = opt->get("sat-acc", 0);
        sat_states_ = opt->get("sat-states", 0);
        state_based_ = opt->get("state-based", 0);
        wdba_minimize_ = opt->get("wdba-minimize", -1);
        gen_reduce_parity_ = opt->get("gen-reduce-parity", 1);
        simul_max_ = opt->get("simul-max", 4096);
        merge_states_min_ = opt->get("merge-states-min", 128);
        wdba_det_max_ = opt->get("wdba-det-max", 4096);
        simul_trans_pruning_ = opt->get("simul-trans-pruning", 512);
        acd_ = opt->get("acd", 1);

        if (sat_acc_ && sat_minimize_ == 0)
          sat_minimize_ = 1;        // Dicho.
        if (sat_states_ && sat_minimize_ == 0)
          sat_minimize_ = 1;
        if (sat_minimize_)
          {
            tba_determinisation_ = 1;
            if (sat_acc_ <= 0)
              sat_acc_ = -1;
            if (sat_states_ <= 0)
              sat_states_ = -1;
          }

        // Set default param value if not provided and used.
        if (sat_minimize_ == 2 && sat_incr_steps_ < 0) // Assume algorithm.
            sat_incr_steps_ = 6;
        else if (sat_minimize_ == 3 && sat_incr_steps_ < -1) // Incr algorithm.
            sat_incr_steps_ = 2;
      }
  }

  twa_graph_ptr
  postprocessor::do_simul(const twa_graph_ptr& a, int opt) const
  {
    if (opt == 0)
      return a;
    if (merge_states_min_ > 0
        && static_cast<unsigned>(merge_states_min_) < a->num_states())
      a->merge_states();
    if (simul_max_ > 0 && static_cast<unsigned>(simul_max_) < a->num_states())
      return a;

    static unsigned sim = [&]()
    {
      if (simul_method_ != -1)
        return simul_method_;

      char* s = getenv("SPOT_SIMULATION_REDUCTION");
      return s ? *s - '0' : 0;
    }();

    if (sim == 2)
      opt += 3;

    // FIXME: simulation-based reduction now have work-arounds for
    // non-separated sets, so we can probably try them.
    if (!has_separate_sets(a))
      return a;
    switch (opt)
      {
      case 1:
        return simulation(a, simul_trans_pruning_);
      case 2:
        return cosimulation(a, simul_trans_pruning_);
      case 3:
        return iterated_simulations(a, simul_trans_pruning_);
      case 4:
        return reduce_direct_sim(a);
      case 5:
        return reduce_direct_cosim(a);
      case 6:
        return reduce_iterated(a);
      default:
        return iterated_simulations(a, simul_trans_pruning_);
      }
  }

  twa_graph_ptr
  postprocessor::do_sba_simul(const twa_graph_ptr& a, int opt) const
  {
    if (ba_simul_ <= 0)
      return a;
    if (simul_max_ > 0 && static_cast<unsigned>(simul_max_) < a->num_states())
      return a;
    switch (opt)
      {
      case 0:
        return a;
      case 1:
        return simulation_sba(a, simul_trans_pruning_);
      case 2:
        return cosimulation_sba(a, simul_trans_pruning_);
      case 3:
      default:
        return iterated_simulations_sba(a, simul_trans_pruning_);
      }
  }

  twa_graph_ptr
  postprocessor::choose_degen(const twa_graph_ptr& a) const
  {
    if (state_based_)
      return do_degen(a);
    else
      return do_degen_tba(a);
  }

  twa_graph_ptr
  postprocessor::do_degen(const twa_graph_ptr& a) const
  {
    auto d = degeneralize(a,
                          degen_reset_, degen_order_,
                          degen_cache_, degen_lskip_,
                          degen_lowinit_, degen_remscc_);
    return do_sba_simul(d, ba_simul_);
  }

  twa_graph_ptr
  postprocessor::do_degen_tba(const twa_graph_ptr& a) const
  {
    return degeneralize_tba(a,
                            degen_reset_, degen_order_,
                            degen_cache_, degen_lskip_,
                            degen_lowinit_, degen_remscc_);
  }

  twa_graph_ptr
  postprocessor::do_scc_filter(const twa_graph_ptr& a, bool arg) const
  {
    if (scc_filter_ == 0)
      return a;
    if (state_based_ && a->prop_state_acc().is_true())
      return scc_filter_states(a, arg);
    else
      return scc_filter(a, arg, nullptr, type_ == CoBuchi || type_ == Buchi);
  }

  twa_graph_ptr
  postprocessor::do_scc_filter(const twa_graph_ptr& a) const
  {
    return do_scc_filter(a, scc_filter_ > 1);
  }

#define PREF_ (pref_ & (Small | Deterministic))
#define COMP_ (pref_ & Complete)
#define SBACC_ (pref_ & SBAcc)
#define COLORED_ (pref_ & Colored)


  twa_graph_ptr
  postprocessor::finalize(twa_graph_ptr tmp) const
  {
    if (PREF_ != Any && level_ != Low)
      tmp->remove_unused_ap();
    bool was_complete = tmp->prop_complete().is_true();
    if (COMP_ && !was_complete)
      tmp = complete(tmp);
    bool want_parity = type_ & Parity;
    if (want_parity && tmp->num_sets() > 1
        && (tmp->acc().is_generalized_buchi()
            || tmp->acc().is_generalized_co_buchi()))
      tmp = choose_degen(tmp);
    assert(!!SBACC_ == state_based_);
    if (state_based_)
      tmp = sbacc(tmp);
    if (type_ == Buchi)
      tmp = ensure_ba(tmp, level_ == High);
    if (want_parity)
      {
        if (!acd_was_used_ || (COMP_ && !was_complete))
          reduce_parity_here(tmp, COLORED_);
        parity_kind kind = parity_kind_any;
        parity_style style = parity_style_any;
        if ((type_ & ParityMin) == ParityMin)
          kind = parity_kind_min;
        else if ((type_ & ParityMax) == ParityMax)
          kind = parity_kind_max;
        if ((type_ & ParityOdd) == ParityOdd)
          style = parity_style_odd;
        else if ((type_ & ParityEven) == ParityEven)
          style = parity_style_even;
        change_parity_here(tmp, kind, style);
      }
    return tmp;
  }

  twa_graph_ptr
  postprocessor::run(twa_graph_ptr a, formula f)
  {
    if (simul_ < 0)
      simul_ = (level_ == Low) ? 1 : 3;
    if (ba_simul_ < 0)
      ba_simul_ = (level_ == High) ? simul_ : 0;
    int detaut_simul_default = (level_ != Low && simul_ > 0) ? 1 : 0;
    if (dpa_simul_ < 0)
      dpa_simul_ = detaut_simul_default;
    if (dba_simul_ < 0)
      dba_simul_ = detaut_simul_default;
    if (det_simul_ < 0)
      det_simul_ = simul_ > 0;
    if (scc_filter_ < 0)
      scc_filter_ = 1;
    if (type_ == BA)
      {
        pref_ |= SBAcc;
        type_ = Buchi;
      }
    if (SBACC_)
      state_based_ = true;
    else if (state_based_)
      pref_ |= SBAcc;

    bool via_gba =
      (type_ == Buchi) || (type_ == GeneralizedBuchi) || (type_ == Monitor);
    bool want_parity = type_ & Parity;
    acd_was_used_ = false;

    if (COLORED_ && !want_parity)
      throw std::runtime_error("postprocessor: the Colored setting only works "
                               "for parity acceptance");



    // Attempt to simplify the acceptance condition, unless this is a
    // parity automaton and we want parity acceptance in the output.
    auto simplify_acc = [&](twa_graph_ptr in)
      {
        if (PREF_ == Any && level_ == Low)
          return in;
        bool isparity = in->acc().is_parity();
        if (isparity && in->is_existential()
            && (type_ == Generic || want_parity))
          {
            auto res = reduce_parity(in);
            if (want_parity || gen_reduce_parity_)
              return res;
            // In case type_ == Generic and gen_reduce_parity_ == 0,
            // we only return the result of reduce_parity() if it can
            // lower the number of colors.  Otherwise,
            // simplify_acceptance() will not do better and we return
            // the input unchanged.  The reason for not always using
            // the output of reduce_parity() is that is may hide
            // symmetries between automata, as in issue #402.
            return (res->num_sets() < in->num_sets()) ? res : in;
          }
        if (want_parity && isparity)
          return cleanup_parity(in);
        if (level_ == High)
          return simplify_acceptance(in);
        else
          return cleanup_acceptance(in);
      };
    a = simplify_acc(a);

    if (!a->is_existential() &&
        // We will probably have to revisit this condition later.
        // Currently, the intent is that postprocessor should never
        // return an alternating automaton, unless it is called with
        // its laxest settings.
        !(type_ == Generic && PREF_ == Any && level_ == Low))
      a = remove_alternation(a);

    // If we do want a parity automaton, we can use to_parity().
    // However (1) degeneralization is faster if the input is
    // GBA, and (2) if we want a deterministic parity automaton and the
    // input is not deterministic, that is useless here.  We need
    // to determinize it first, and our determinization
    // function only deal with TGBA as input.
    if ((via_gba || (want_parity && !a->acc().is_parity()))
        && !a->acc().is_generalized_buchi())
      {
        if (want_parity && (PREF_ != Deterministic || is_deterministic(a)))
          {
            if (acd_)
              {
                a = acd_transform(a, COLORED_);
                acd_was_used_ = true;
              }
            else
              {
                a = to_parity(a);
              }
          }
        else
          {
            a = to_generalized_buchi(a);
            if (PREF_ == Any && level_ == Low)
              a = do_scc_filter(a, true);
          }
      }

    if (PREF_ == Any && level_ == Low
        && (type_ == Generic
            || type_ == GeneralizedBuchi
            || (type_ == Buchi && a->acc().is_buchi())
            || (type_ == Monitor && a->num_sets() == 0)
            || (want_parity && a->acc().is_parity())
            || (type_ == CoBuchi && a->acc().is_co_buchi())))
      return finalize(a);

    int original_acc = a->num_sets();

    // Remove useless SCCs.
    if (type_ == Monitor)
      // Do not bother about acceptance conditions, they will be
      // ignored.
      a = scc_filter_states(a);
    else
      a = do_scc_filter(a, (PREF_ == Any) && !COLORED_);

    if (type_ == Monitor)
      {
        if (PREF_ == Deterministic)
          a = minimize_monitor(a);
        else
          strip_acceptance_here(a);

        if (PREF_ != Any)
          {
            if (PREF_ != Deterministic)
              a = do_simul(a, simul_);

            // For Small,High we return the smallest between the output of
            // the simulation, and that of the deterministic minimization.
            // Prefer the deterministic automaton in case of equality.
            if (PREF_ == Small && level_ == High && simul_)
              {
                auto m = minimize_monitor(a);
                if (m->num_states() <= a->num_states())
                  a = m;
              }
          }
        return finalize(a);
      }

    if (PREF_ == Any)
      {
        if (type_ == Buchi
            || (type_ == CoBuchi && a->acc().is_generalized_co_buchi()))
          {
            a = choose_degen(a);
          }
        else if (type_ == CoBuchi)
          {
            a = to_nca(a);
            if (state_based_ && a->prop_state_acc().is_true())
              a = do_sba_simul(a, simul_);
            else
              a = do_simul(a, simul_);
          }
        return finalize(a);
      }

    bool dba_is_wdba = false;
    bool dba_is_minimal = false;
    twa_graph_ptr dba = nullptr;
    twa_graph_ptr sim = nullptr;

    output_aborter aborter_
      (det_max_states_ >= 0 ? static_cast<unsigned>(det_max_states_) : -1U,
       det_max_edges_ >= 0 ? static_cast<unsigned>(det_max_edges_) : -1U);
    output_aborter* aborter =
      (det_max_states_ >= 0 || det_max_edges_ >= 0) ? &aborter_ : nullptr;

    int wdba_minimize = wdba_minimize_;
    if (wdba_minimize == -1)
      {
        if (level_ == High)
          wdba_minimize = 1;
        else if (level_ == Medium || PREF_ == Deterministic)
          wdba_minimize = 2;
        else
          wdba_minimize = 0;
      }
    if (wdba_minimize == 2)
      wdba_minimize = minimize_obligation_guaranteed_to_work(a, f);
    if (wdba_minimize)
      {
        bool reject_bigger = (PREF_ == Small) && (level_ <= Medium);
        output_aborter* ab = aborter;
        output_aborter wdba_aborter(wdba_det_max_ > 0 ?
                                    (static_cast<unsigned>(wdba_det_max_)
                                     + a->num_states()) : -1U);
        if (!ab && PREF_ != Deterministic)
          ab = &wdba_aborter;
        dba = minimize_obligation(a, f, nullptr, reject_bigger, ab);

        if (dba
            && dba->prop_inherently_weak().is_true()
            && dba->prop_universal().is_true())
          {
            // The WDBA is a BA, so no degeneralization is required.
            // We just need to add an acceptance set if there is none.
            dba_is_minimal = dba_is_wdba = true;
            if (type_ == Buchi)
              ensure_ba(dba, level_ == High);
          }
        else
          {
            // Minimization failed.
            dba = nullptr;
          }
      }

    // Run a simulation when wdba failed (or was not run), or
    // at hard levels if we want a small output.
    if (!dba || (level_ == High && PREF_ == Small))
      {
        if ((state_based_ && a->prop_state_acc().is_true())
            && !tba_determinisation_
            && (type_ != Buchi || a->acc().is_buchi()))
          {
            sim = do_sba_simul(a, ba_simul_);
          }
        else
          {
            sim = do_simul(a, simul_);
            // Degeneralize the result of the simulation if needed.
            // No need to do that if tba_determinisation_ will be used.
            if (type_ == Buchi && !tba_determinisation_)
              sim = choose_degen(sim);
            else if (want_parity && !sim->acc().is_parity())
              sim = do_degen_tba(sim);
            else if (state_based_ && !tba_determinisation_)
              sim = sbacc(sim);
          }
      }

    // If WDBA failed, but the simulation returned a deterministic
    // automaton, use it as dba.
    assert(dba || sim);
    if (!dba && is_deterministic(sim))
      {
        std::swap(sim, dba);
        // We postponed degeneralization above in case we would need
        // to perform TBA-determinisation, but now it is clear
        // that we won't perform it.  So do degeneralize.
        if (tba_determinisation_)
          {
            if (type_ == Buchi)
              dba = choose_degen(dba);
            else if (state_based_)
              dba = sbacc(dba);
            assert(is_deterministic(dba));
          }
      }

    // If we don't have a DBA, attempt tba-determinization if requested.
    if (tba_determinisation_ && !dba)
      {
        twa_graph_ptr tmpd = nullptr;
        if (PREF_ == Deterministic
            && f
            && f.is_syntactic_recurrence()
            && sim->num_sets() > 1)
          tmpd = degeneralize_tba(sim);

        auto in = tmpd ? tmpd : sim;

        // These thresholds are arbitrary.
        //
        // For producing Small automata, we assume that a
        // deterministic automaton that is twice the size of the
        // original will never get reduced to a smaller one.  We also
        // do not want more than 2^13 cycles in an SCC.
        //
        // For Deterministic automata, we accept automata that
        // are 8 times bigger, with no more that 2^15 cycle per SCC.
        // The cycle threshold is the most important limit here.  You
        // may up it if you want to try producing larger automata.
        auto tmp =
          tba_determinize_check(in,
                                (PREF_ == Small) ? 2 : 8,
                                1 << ((PREF_ == Small) ? 13 : 15),
                                f);
        if (tmp && tmp != in)
          {
            // There is no point in running the reverse simulation on
            // a deterministic automaton, since all prefixes are
            // unique.
            dba = do_simul(tmp, dba_simul_);
          }
        if (dba && PREF_ == Deterministic)
          {
            // disregard the result of the simulation.
            sim = nullptr;
          }
        else
          {
            // degeneralize sim, because we did not do it earlier
            if (type_ == Buchi)
              sim = choose_degen(sim);
          }
      }

    if ((PREF_ == Deterministic && (type_ == Generic || want_parity)) && !dba)
      {
        bool det_simul = det_simul_;
        auto tba = to_generalized_buchi(sim);
        if (simul_max_ > 0
            && static_cast<unsigned>(simul_max_) < tba->num_states())
          det_simul = false;
        dba = tgba_determinize(tba,
                               false, det_scc_, det_simul, det_stutter_,
                               aborter, simul_trans_pruning_);
        // Setting det-max-states or det-max-edges may cause tgba_determinize
        // to fail.

        if (dba)
          {
            dba = do_simul(simplify_acc(dba), dpa_simul_);
            sim = nullptr;
          }
      }

    // Now dba contains either the result of WDBA-minimization (in
    // that case dba_is_wdba=true), or some deterministic automaton
    // that is either the result of the simulation or of the
    // TBA-determinization (dba_is_wdba=false in both cases), or a
    // parity automaton coming from tgba_determinize().  If the dba is
    // a WDBA, we do not have to run SAT-minimization.  A negative
    // value in sat_minimize_ can force its use for debugging.
    if (sat_minimize_ && dba && (!dba_is_wdba || sat_minimize_ < 0))
      {
        if (type_ == Generic)
          throw std::runtime_error
            ("postproc() not yet updated to mix sat-minimize and Generic");
        unsigned target_acc;
        if (type_ == Buchi)
          target_acc = 1;
        else if (sat_acc_ != -1)
          target_acc = sat_acc_;
        else
          // Take the number of acceptance sets from the input
          // automaton, not from dba, because dba often has been
          // degeneralized by tba_determinize_check().  Make sure it
          // is at least 1.
          target_acc = original_acc > 0 ? original_acc : 1;

        const_twa_graph_ptr in = nullptr;
        if (target_acc == 1)
          {
            // If we are seeking a minimal DBA with unknown number of
            // states, then we should start from the degeneralized,
            // because the input TBA might be smaller.
            if (state_based_)
              in = degeneralize(dba);
            else
              in = degeneralize_tba(dba);
          }
        else
          {
            in = dba;
          }

        twa_graph_ptr res = complete(in);
        if (target_acc == 1)
          {
            if (sat_states_ != -1)
              res = dtba_sat_synthetize(res, sat_states_, state_based_);
            else if (sat_minimize_ == 1)
              res = dtba_sat_minimize_dichotomy
                (res, state_based_, sat_langmap_);
            else if (sat_minimize_ == 2)
              res = dtba_sat_minimize_assume(res, state_based_, -1,
                                             sat_incr_steps_);
            else if (sat_minimize_ == 3)
              res = dtba_sat_minimize_incr(res, state_based_, -1,
                                           sat_incr_steps_);
            else // if (sat_minimize == 4 || sat_minimize == -1)
              res = dtba_sat_minimize(res, state_based_);
          }
        else
          {
            if (sat_states_ != -1)
              res = dtwa_sat_synthetize
                (res, target_acc,
                 acc_cond::acc_code::generalized_buchi(target_acc),
                 sat_states_, state_based_);
            else if (sat_minimize_ == 1)
              res = dtwa_sat_minimize_dichotomy
                (res, target_acc,
                 acc_cond::acc_code::generalized_buchi(target_acc),
                 state_based_, sat_langmap_);
            else if (sat_minimize_ == 2)
              res = dtwa_sat_minimize_assume
                (res, target_acc,
                 acc_cond::acc_code::generalized_buchi(target_acc),
                 state_based_, -1, false, sat_incr_steps_);
            else if (sat_minimize_ == 3)
              res = dtwa_sat_minimize_incr
                (res, target_acc,
                 acc_cond::acc_code::generalized_buchi(target_acc),
                 state_based_, -1, false, sat_incr_steps_);
            else // if (sat_minimize_ == 4 || sat_minimize_ == -1)
              res = dtwa_sat_minimize
                (res, target_acc,
                 acc_cond::acc_code::generalized_buchi(target_acc),
                 state_based_);
          }

        if (res)
          {
            dba = do_scc_filter(res, true);
            dba_is_minimal = true;
          }
      }

    // Degeneralize the dba resulting from tba-determinization or
    // sat-minimization (which is a TBA) if requested and needed.
    if (dba && !dba_is_wdba && type_ == Buchi && state_based_
        && !(dba_is_minimal && dba->num_sets() == 1))
      dba = degeneralize(dba);

    if (dba && sim)
      {
        if (dba->num_states() > sim->num_states())
          dba = nullptr;
        else
          sim = nullptr;
      }

    sim = dba ? dba : sim;
    if (level_ == High && scc_filter_ != 0 && !(acd_was_used_ && COLORED_))
      // Do that even for WDBA, to remove marks from transitions
      // leaving trivial SCCs.
      sim = do_scc_filter(sim, true);

    if (type_ == CoBuchi)
      {
        unsigned ns = sim->num_states();
        bool weak = sim->prop_weak().is_true();

        if (PREF_ == Deterministic)
          sim = to_dca(sim);
        else
          sim = to_nca(sim);

        // if the input of to_dca/to_nca was weak, the number of
        // states has not changed, and running simulation is useless.
        if (!weak || (level_ != Low && ns < sim->num_states()))
          {
            if (state_based_ && sim->prop_state_acc().is_true())
              sim = do_sba_simul(sim, simul_);
            else
              sim = do_simul(sim, simul_);
          }
      }

    return finalize(sim);
  }
}
