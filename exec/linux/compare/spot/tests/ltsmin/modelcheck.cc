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
#include "bin/common_conv.hh"
#include "bin/common_setup.hh"
#include "bin/common_output.hh"

#include <spot/ltsmin/ltsmin.hh>
#include <spot/ltsmin/spins_kripke.hh>
#include <spot/mc/mc_instanciator.hh>
#include <spot/twaalgos/dot.hh>
#include <spot/tl/defaultenv.hh>
#include <spot/tl/parse.hh>
#include <spot/twaalgos/translate.hh>
#include <spot/twaalgos/emptiness.hh>
#include <spot/twaalgos/postproc.hh>
#include <spot/twaalgos/degen.hh>
#include <spot/twaalgos/are_isomorphic.hh>
#include <spot/twa/twaproduct.hh>
#include <spot/misc/timer.hh>
#include <spot/misc/memusage.hh>
#include <cstring>
#include <spot/kripke/kripkegraph.hh>
#include <spot/twaalgos/hoa.hh>

#include <algorithm>
#include <thread>
#include <spot/twacube/twacube.hh>
#include <spot/twacube_algos/convert.hh>

static const char argp_program_doc[] =
"Process model and formula to check wether a "
"model meets a specification.\v\
Exit status:\n\
  0  No counterexample found\n\
  1  A counterexample has been found\n\
  2  Errors occurs during processing";

const unsigned DOT_MODEL = 1;
const unsigned HOA_MODEL = 2;
const unsigned DOT_PRODUCT = 4;
const unsigned DOT_FORMULA = 8;
const unsigned CSV = 16;

// Handle all options specified in the command line
struct mc_options_
{
  bool compute_counterexample = false;
  unsigned output = 0;
  bool is_empty = false;
  char* formula = nullptr;
  char* model = nullptr;
  bool selfloopize = true;
  char* dead_ap = nullptr;
  bool use_timer = false;
  unsigned compress = 0;
  unsigned nb_threads = 1;
  bool csv = false;
  spot::mc_algorithm algorithm = spot::mc_algorithm::BLOEMEN_EC;
  bool force_parallel = false;
} mc_options;


static int
parse_opt_finput(int key, char* arg, struct argp_state*)
{
  // This switch is alphabetically-ordered.
  switch (key)
    {
    case CSV:
      mc_options.csv = true;
      break;
    case 'B':
      mc_options.algorithm = spot::mc_algorithm::BLOEMEN_EC;
      mc_options.force_parallel = true;
      break;
    case 'b':
      // FIXME Differenciate  bloemen and bloemen_ec: -b/-B is not enough
      mc_options.algorithm = spot::mc_algorithm::BLOEMEN_SCC;
      mc_options.force_parallel = true;
      break;
    case 'c':
      mc_options.compute_counterexample = true;
      break;
    case 'C':
      mc_options.algorithm = spot::mc_algorithm::CNDFS;
      mc_options.force_parallel = true;
      break;
    case 'd':
      if (strcmp(arg, "model") == 0)
        mc_options.output |= DOT_MODEL;
      else if (strcmp(arg, "product") == 0)
        mc_options.output |= DOT_PRODUCT;
      else if (strcmp(arg, "formula") == 0)
        mc_options.output |= DOT_FORMULA;
      else
        {
          std::cerr << "Unknown argument: '" << arg
                    << "' for option --dot\n";
          return ARGP_ERR_UNKNOWN;
        }
      break;
    case 'e':
      mc_options.is_empty = true;
      break;
    case 'f':
      mc_options.formula = arg;
      break;
    case 'h':
      mc_options.algorithm = spot::mc_algorithm::DEADLOCK;
      mc_options.force_parallel = true;
      mc_options.selfloopize = false;
      break;
    case 'm':
      mc_options.model = arg;
      break;
    case 'k':
      mc_options.output |= HOA_MODEL;
      break;
    case 'p':
      mc_options.nb_threads = to_unsigned(arg, "-p/--parallel");
      mc_options.force_parallel = true;
      break;
    case 'r':
      mc_options.algorithm = spot::mc_algorithm::REACHABILITY;
      mc_options.force_parallel = true;
      break;
    case 's':
      mc_options.dead_ap = arg;
      break;
    case 't':
      mc_options.use_timer = true;
      break;
    case 'w':
      mc_options.algorithm = spot::mc_algorithm::SWARMING;
      mc_options.force_parallel = true;
      break;
    case 'z':
      mc_options.compress = to_unsigned(arg, "-z/--compress");
      break;
    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

static const argp_option options[] =
  {
    // Keep each section sorted
    // ------------------------------------------------------------
    { nullptr, 0, nullptr, 0, "Input options:", 1 },
    { "formula", 'f', "STRING", 0, "use the formula STRING", 0 },
    // FIXME do we want support for reading more than one formula?
    { "model", 'm', "STRING", 0, "use  the model stored in file STRING", 0 },
    // ------------------------------------------------------------
    { nullptr, 0, nullptr, 0, "Process options:", 2 },
    { "bloemen-ec", 'B', nullptr, 0,
      "run the emptiness-check of Bloemen et al.  (HVC'16). Return 1 "
      "if a counterexample is found.", 0},
    { "bloemen", 'b', nullptr, 0,
      "run the SCC computation of Bloemen et al. (PPOPP'16). Return 1 "
      "if a counterexample is found.", 0 },
    { "cndfs", 'C', nullptr, 0,
      "run the emptiness check of Evangelista et al. (ATVA'12). Return 1 "
      "if a counterexample is found.", 0 },
    { "counterexample", 'c', nullptr, 0,
      "compute an accepting counterexample (if it exists)", 0 },
    { "has-deadlock", 'h', nullptr, 0,
      "check if the model has a deadlock. "
      "Return 1 if the model contains a deadlock."
      , 0 },
    { "is-empty", 'e', nullptr, 0,
      "check if the model meets its specification. Uses Cou99 in sequential "
      "and bloemen-ec in pallel (option -p). Return 1 if a counterexample "
      "is found."
      , 0 },
    { "parallel", 'p', "INT", 0, "use INT threads (when possible)", 0 },
    { "reachability", 'r', nullptr, 0, "performs a traversal of the model "
      , 0 },
    { "selfloopize", 's', "STRING", 0,
      "use STRING as property for marking deadlock "
      "states (by default selfloopize is activated with STRING='true')", 0 },
    { "swarming", 'w', nullptr, 0,
      "run the technique of of Holzmann et al. (IEEE'11) with the emptiness-"
      "check of Renault et al. (LPAR'13). Returns 1 if a counterexample "
      "is found.", 0 },
    { "timer", 't', nullptr, 0,
      "time the different phases of the execution", 0 },
    // ------------------------------------------------------------
    { nullptr, 0, nullptr, 0, "Output options:", 3 },
    { "csv", CSV, nullptr, 0,
      "output a CSV containing interesting values", 0 },
    { "dot", 'd', "[model|product|formula]", 0,
      "output the associated automaton in dot format", 0 },
    { "kripke", 'k', nullptr, 0,
      "output the model state-space in Kripke format", 0 },
    // ------------------------------------------------------------
    { nullptr, 0, nullptr, 0, "Optimization options:", 4 },
    { "compress", 'z', "INT", 0, "specify the level of compression\n"
      "1 : handle large models\n"
      "2 : (faster) assume all values in [0 .. 2^28-1]", 0 },
    // ------------------------------------------------------------
    { nullptr, 0, nullptr, 0, "General options:", 5 },
    { nullptr, 0, nullptr, 0, nullptr, 0 }
  };

const struct argp finput_argp = { options, parse_opt_finput,
                                  nullptr, nullptr, nullptr,
                                  nullptr, nullptr };

const struct argp_child children[] =
  {
    { &finput_argp, 0, nullptr, 1 },
    { &misc_argp, 0, nullptr, -1 },
    { nullptr, 0, nullptr, 0 }
  };

static std::string split_filename(const std::string& str) {
  unsigned found = str.find_last_of("/");
  return str.substr(found+1);
}

static int checked_main()
{
  spot::default_environment& env = spot::default_environment::instance();
  spot::atomic_prop_set ap;
  auto dict = spot::make_bdd_dict();
  spot::const_kripke_ptr model = nullptr;
  spot::const_twa_graph_ptr prop = nullptr;
  spot::const_twa_ptr product = nullptr;
  spot::emptiness_check_instantiator_ptr echeck_inst = nullptr;
  int exit_code = 0;
  spot::postprocessor post;
  spot::formula deadf = spot::formula::ff();
  spot::formula f = nullptr;
  spot::timer_map tm;

  if (mc_options.selfloopize)
    {
      if (mc_options.dead_ap == nullptr ||
          !strcasecmp(mc_options.dead_ap, "true"))
        deadf = spot::formula::tt();
      else if (!strcasecmp(mc_options.dead_ap, "false"))
        deadf = spot::formula::ff();
      else
        deadf = env.require(mc_options.dead_ap);
    }

  if (mc_options.formula != nullptr)
    {
      tm.start("parsing formula");
      {
        auto pf = spot::parse_infix_psl(mc_options.formula, env, false);
        exit_code = pf.format_errors(std::cerr);
        f = pf.f;
      }
      tm.stop("parsing formula");

      tm.start("translating formula");
      {
        spot::translator trans(dict);
        // if (deterministic) FIXME
        //   trans.set_pref(spot::postprocessor::Deterministic);
        prop = trans.run(&f);
      }
      tm.stop("translating formula");

      atomic_prop_collect(f, &ap);

      if (mc_options.output & DOT_FORMULA)
        {
          tm.start("dot output");
          spot::print_dot(std::cout, prop);
          tm.stop("dot output");
        }
    }

  if (mc_options.model != nullptr && !mc_options.force_parallel)
    {
      tm.start("loading ltsmin model");
      try
        {
          model = spot::ltsmin_model::load(mc_options.model)
            .kripke(&ap, dict, deadf, mc_options.compress);
        }
      catch (const std::runtime_error& e)
        {
          std::cerr << e.what() << '\n';
        }
      tm.stop("loading ltsmin model");

      if (!model)
        {
          exit_code = 2;
          goto safe_exit;
        }

      if (mc_options.output & DOT_MODEL)
        {
          tm.start("dot output");
          spot::print_dot(std::cout, model);
          tm.stop("dot output");
        }

      if (mc_options.output & HOA_MODEL)
        {
          tm.start("kripke output");
          spot::print_hoa(std::cout, model);
          tm.stop("kripke output");
          goto safe_exit;
        }
    }
  else if (mc_options.force_parallel && mc_options.output)
    {
      std::cerr << "Cannot combine 'Ouput options' with parallelism.\n"
                << "Consider removing -p.\n";
      goto safe_exit;
    }

  if (mc_options.force_parallel == false &&
      mc_options.formula != nullptr &&
      mc_options.model != nullptr)
    {
      std::cout << "Warning : using sequential algorithms (BDD-based)\n\n";

      product = spot::otf_product(model, prop);

      if (mc_options.is_empty)
        {
          const char* err;
          echeck_inst = spot::make_emptiness_check_instantiator("Cou99", &err);
          if (!echeck_inst)
            {
              std::cerr << "Unknown emptiness check algorihm `"
                        << err <<  "'\n";
              exit_code = 1;
              goto safe_exit;
            }

          auto ec = echeck_inst->instantiate(product);
          assert(ec);
          int memused = spot::memusage();
          tm.start("running emptiness check");
          spot::emptiness_check_result_ptr res;
          try
            {
              res = ec->check();
            }
          catch (const std::bad_alloc&)
          {
            std::cerr << "Out of memory during emptiness check.\n";
            if (!mc_options.compress)
              std::cerr << "Try option -z for state compression.\n";
            exit_code = 2;
            exit(exit_code);
          }
        tm.stop("running emptiness check");
        memused = spot::memusage() - memused;

        ec->print_stats(std::cout);
        std::cout << memused << " pages allocated for emptiness check"
                  << std::endl;

        if (!res)
          {
            std::cout << "no accepting run found" << std::endl;
          }
        else if (!mc_options.compute_counterexample)
          {
            std::cout << "an accepting run exists "
                      << "(use -c to print it)" << std::endl;
            exit_code = 1;
          }
        else
          {
            exit_code = 1;
            spot::twa_run_ptr run;
            tm.start("computing accepting run");
            try
              {
                run = res->accepting_run();
              }
            catch (const std::bad_alloc&)
              {
                std::cerr
                  << "Out of memory while looking for counterexample.\n";
                exit_code = 2;
                exit(exit_code);
              }
            tm.stop("computing accepting run");

            if (!run)
              {
                std::cout << "an accepting run exists" << std::endl;
              }
            else
              {
                tm.start("reducing accepting run");
                run = run->reduce();
                tm.stop("reducing accepting run");
                tm.start("printing accepting run");
                std::cout << *run;
                tm.stop("printing accepting run");
              }
          }

        if (mc_options.csv)
          {
            std::cout << "\nFind following the csv: "
                      << "model,formula,walltimems,memused,type,"
                      << "states,transitions\n";
            std::cout << '#'
                      << split_filename(mc_options.model)
                      << ','
                      << mc_options.formula << ','
                      << tm.timer("running emptiness check").walltime() << ','
                      << memused << ','
                      << (!res ? "EMPTY," : "NONEMPTY,")
                      << ec->statistics()->get("states") << ','
                      << ec->statistics()->get("transitions")
                      << std::endl;
          }
        }

      if (mc_options.output & DOT_PRODUCT)
        {
          tm.start("dot output");
          spot::print_dot(std::cout, product);
          tm.stop("dot output");
        }
    }
  else if (mc_options.force_parallel && mc_options.model != nullptr)
    {
      std::cout << "Warning : using parallel algorithms (CUBE-based)\n\n";

      if (mc_options.output & DOT_PRODUCT)
        {
          std::cerr << "\nERROR: Parallel algorithm doesn't support DOT"
            " output  for the synchronous product.\n"
            "       Please consider removing '-p' option\n";
          exit_code = 2;
          goto safe_exit;
        }

      if (mc_options.output & HOA_MODEL)
        {
          std::cerr << "\nERROR: Parallel algorithm doesn't support HOA"
            " output  for the synchronous product.\n"
            "       Please consider removing '-p' option\n";
          exit_code = 2;
          goto safe_exit;
        }

      if (prop == nullptr &&
          (mc_options.algorithm == spot::mc_algorithm::CNDFS ||
           mc_options.algorithm == spot::mc_algorithm::BLOEMEN_EC ||
           mc_options.algorithm == spot::mc_algorithm::SWARMING))
        {
          std::cerr << "\nERROR: Algorithm " << mc_options.algorithm
                    << " requires to provide a formula (--formula)\n";
          exit_code = 2;
          goto safe_exit;
        }

      unsigned int hc = std::thread::hardware_concurrency();
      if (mc_options.nb_threads > hc)
        std::cerr << "Warning: you require " << mc_options.nb_threads
                  << " threads, but your computer only support " << hc
                  << ". This could slow down parallel algorithms.\n";

      auto prop_degen = prop;
      if (mc_options.algorithm == spot::mc_algorithm::CNDFS)
        {
          // Only support Single Acceptance Conditions
          tm.start("degeneralize");
          prop_degen = spot::degeneralize_tba(prop);
          tm.stop("degeneralize");
        }

      tm.start("twa to twacube");
      auto propcube = spot::twa_to_twacube(prop_degen);
      tm.stop("twa to twacube");

      tm.start("load kripkecube");
      spot::ltsmin_kripkecube_ptr modelcube = nullptr;
      try
        {
          std::vector<std::string> aps = {};
          if (propcube != nullptr)
            aps = propcube->ap();

          modelcube = spot::ltsmin_model::load(mc_options.model)
            .kripkecube(aps, deadf, mc_options.compress, mc_options.nb_threads);
        }
      catch (const std::runtime_error& e)
        {
          std::cerr << e.what() << '\n';
        }
      tm.stop("load kripkecube");

      int memused = spot::memusage();
      tm.start("exploration");

      auto result =
        spot::ec_instanciator<spot::ltsmin_kripkecube_ptr, spot::cspins_state,
                              spot::cspins_iterator, spot::cspins_state_hash,
                              spot::cspins_state_equal>
        (mc_options.algorithm, modelcube, propcube,
         mc_options.compute_counterexample);

      tm.stop("exploration");
      memused = spot::memusage() - memused;

      if (!modelcube)
        {
          exit_code = 2;
          goto safe_exit;
        }

      // Display statistics
      std::cout << result << '\n';
      std::cout << memused << " pages allocated for "
                << mc_options.algorithm << '\n';


      std::cout << "\nSummary :\n";
      auto rval = result.value[0];
      std::for_each(result.value.rbegin(), result.value.rend(),
                    [&](spot::mc_rvalue n) { rval = rval | n; });

      if (rval == spot::mc_rvalue::NO_DEADLOCK ||
          rval == spot::mc_rvalue::EMPTY ||
          rval == spot::mc_rvalue::SUCCESS)
        std::cout << "no accepting run / counterexample found\n";
      else if (!mc_options.compute_counterexample)
        {
          std::cout << "an accepting run exists "
                    << "(use -c to print it)" << std::endl;
          exit_code = 1;
        }
      else
        {
          std::cout << "an accepting run exists\n" <<  result.trace << '\n';
          exit_code = 1;
        }

      if (mc_options.csv)
        {
          // Grab The informations to display into the CSV
          unsigned c_states = 0;
          unsigned c_trans = 0;
          int c_sccs = 0; // not unsigned since it can be negative
          unsigned walltime = 0;
          for (unsigned i = 0; i < result.finisher.size(); ++i)
            {
              if (result.finisher[i])
                walltime = result.walltime[i];
              c_states += result.states[i];
              c_trans += result.transitions[i];
              c_sccs += result.sccs[i];
            }

          std::cout << "Find following the csv: "
                    << "model,formula,walltimems,memused,type,"
                    << "cumul_states,cumul_transitions,cumul_sccs\n";
          std::cout << '#'
                    << split_filename(mc_options.model) << ',';

          if (mc_options.formula != nullptr)
            std::cout << mc_options.formula;

          std::cout << ',' << walltime << ',' << memused << ','
                    << rval << ',' << c_states << ',' << c_trans << ','
                    << (c_sccs < 0 ? -1 : c_sccs) << '\n';
        }
    }

 safe_exit:
  if (mc_options.use_timer)
    tm.print(std::cout);
  tm.reset_all();                // This helps valgrind.
  return exit_code;
}

int
main(int argc, char** argv)
{
  setup(argv);
  const argp ap = { nullptr, nullptr, nullptr,
                    argp_program_doc, children, nullptr, nullptr };

  if (int err = argp_parse(&ap, argc, argv, ARGP_NO_HELP, nullptr, nullptr))
    exit(err);

  auto exit_code = checked_main();

  // Additional checks to debug reference counts in formulas.
  assert(spot::fnode::instances_check());
  exit(exit_code);
}
