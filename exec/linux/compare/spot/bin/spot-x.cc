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

#include "common_sys.hh"
#include <string>
#include <iostream>
#include <cstdlib>
#include <argp.h>
#include "common_setup.hh"

static const char argp_program_doc[] = "\
Common fine-tuning options for programs installed with Spot.\n\
\n\
The argument of -x or --extra-options is a comma-separated list of KEY=INT \
assignments that are passed to the post-processing or translation routines \
(they may be passed to other algorithms in the future).  These options are \
mostly used for benchmarking and debugging purpose.  KEY (without any \
value) is a shorthand for KEY=1, while !KEY is a shorthand for KEY=0.";

#define DOC(NAME, TXT) NAME, 0, nullptr, OPTION_DOC | OPTION_NO_USAGE, TXT, 0

static const argp_option options[] =
  {
    { nullptr, 0, nullptr, 0, "Temporal logic simplification options:", 0 },
    { DOC("tls-impl",
          "Control usage of implication-based rewriting.  \
(0) disables it, (1) enables rules based on syntactic implications, \
(2) additionally allows automata-based implication checks, (3) enables \
more rules based on automata-based implication checks.  The default value \
depends on the --low, --medium, or --high settings.") },
    { DOC("tls-max-states",
          "Maximum number of states of automata involved in automata-based \
implication checks for formula simplifications.  Defaults to 64.") },
    { DOC("tls-max-ops",
          "Maximum number of operands in n-ary operators (or, and) on which \
implication-based simplifications are attempted.  Defaults to 16.") },
    { nullptr, 0, nullptr, 0, "Translation options:", 0 },
    { DOC("ltl-split", "Set to 0 to disable the translation of automata \
as product or sum of subformulas.") },
    { DOC("branch-post", "Set to 0 to disable branching-postponement \
(done during translation, may create more states) and delayed-branching \
(almost similar, but done after translation to only remove states).  \
Set to 1 to force branching-postponement, and to 2 \
to force delayed-branching.  By default, delayed-branching is used.") },
    { DOC("comp-susp", "Set to 1 to enable compositional suspension, \
as described in our SPIN'13 paper (see Bibliography below).  Set to 2, \
to build only the skeleton TGBA without composing it.  Set to 0 (the \
default) to disable.  Nowadays, ltl-split already takes care of that for \
suspendable subformulas at the top level.") },
    { DOC("early-susp", "When set to 1, start compositional suspension on \
the transitions that enter accepting SCCs, and not only on the transitions \
inside accepting SCCs.  This option defaults to 0, and is only used when \
comp-susp=1.") },
    { DOC("exprop", "When set, this causes the core LTL translation to \
explicitly iterate over all possible valuations of atomic propositions when \
considering the successors of a BDD-encoded state, instead of discovering \
possible successors by rewriting the BDD as a sum of product.  This is enabled \
by default for --high, and disabled by default otherwise.  When unambiguous \
automata are required, this option is forced and cannot be disabled.") },
    { DOC("skel-simul", "Default to 1.  Set to 0 to disable simulation \
on the skeleton automaton during compositional suspension.  Only used when \
comp-susp=1.") },
    { DOC("skel-wdba", "Set to 0 to disable WDBA \
minimization on the skeleton automaton during compositional suspension.  \
Set to 1 always WDBA-minimize the skeleton.  Set to 2 to keep the WDBA \
only if it is smaller than the original skeleton.  This option is only \
used when comp-susp=1 and default to 1 or 2 depending on whether --small \
or --deterministic is specified.") },
    { nullptr, 0, nullptr, 0, "Postprocessing options:", 0 },
    { DOC("acd", "Set to 1 (the default) to use paritize automata using \
the alternating cycle decomposition.  Set to 0 to use paritization based \
on latest appearance record variants.") },
    { DOC("scc-filter", "Set to 1 (the default) to enable \
SCC-pruning and acceptance simplification at the beginning of \
post-processing.  Transitions that are outside accepting SCC are \
removed from accepting sets, except those that enter into an accepting \
SCC.  Set to 2 to remove even these entering transition from the \
accepting sets.  Set to 0 to disable this SCC-pruning and acceptance \
simplification pass.") },
    { DOC("degen-reset", "If non-zero (the default), the \
degeneralization algorithm will reset its level any time it exits \
an SCC.") },
    { DOC("degen-lcache", "If non-zero (the default is 1), whenever the \
degeneralization algorithm enters an SCC on a state that has already \
been associated to a level elsewhere, it should reuse that level.  \
Different values can be used to select which level to reuse: 1 always \
uses the first level created, 2 uses the minimum level seen so far, and \
3 uses the maximum level seen so far.  The \"lcache\" stands for \
\"level cache\".") },
    { DOC("degen-order", "If non-zero, the degeneralization algorithm \
will compute an independent degeneralization order for each SCC it \
processes.  This is currently disabled by default.") },
    { DOC("degen-lskip", "If non-zero (the default), the degeneralization \
algorithm will skip as many levels as possible for each transition.  This \
is enabled by default as it very often reduce the number of resulting \
states.  A consequence of skipping levels is that the degeneralized \
automaton tends to have smaller cycles around the accepting states.  \
Disabling skipping will produce automata with large cycles, and often \
with more states.") },
    { DOC("degen-lowinit", "Whenever the degeneralization algorihm enters \
a new SCC (or starts from the initial state), it starts on some level L that \
is compatible with all outgoing transitions.  If degen-lowinit is zero \
(the default) and the corresponding state (in the generalized automaton) \
has an accepting self-loop, then level L is replaced by the accepting \
level, as it might favor finding accepting cycles earlier.  If \
degen-lowinit is non-zero, then level L is always used without looking \
for the presence of an accepting self-loop.") },
    { DOC("degen-remscc", "If non-zero (the default), make sure the output \
of the degeneralization has as many SCCs as the input, by removing superfluous \
ones.") },
    { DOC("det-max-states", "When defined to a positive integer N, \
determinizations will be aborted whenever the number of generated \
states would exceed N.  In this case a non-deterministic automaton \
will be returned.")},
    { DOC("det-max-edges", "When defined to a positive integer N, \
determinizations will be aborted whenever the number of generated \
edges would exceed N.  In this case a non-deterministic automaton \
will be returned.")},
    { DOC("det-scc", "Set to 0 to disable scc-based optimizations in \
the determinization algorithm.") },
    { DOC("det-simul", "Set to 0 to disable simulation-based optimizations in \
the determinization algorithm.  This is enabled by default, unless \"simul\" \
is set to 0.  (Do not confuse this with option \"dpa-simul\",  \
which runs a simulation-based reduction after determinization.)") },
    { DOC("det-stutter", "Set to 0 to disable optimizations based on \
the stutter-invariance in the determinization algorithm.") },
    { DOC("dpa-simul", "Set to 1 to enable simulation-based reduction after \
running a Safra-like determinization to obtain a DPA, or 0 to disable.  By \
default this is disabled at low level or if parameter \"simul\" is set to 0.  \
(Do not confuse this with option det-simul, which uses a simulation-based \
optimizations during the determinization.)") },
    { DOC("gen-reduce-parity", "When the postprocessor routines are \
configured to output automata with any kind of acceptance condition, \
but they happen to process an automaton with parity acceptance, they \
call a function to minimize the number of colors needed.  This option \
controls what happen when this reduction does not reduce the number of \
colors: when set (the default) the output of the reduction is returned, \
this means the colors in the automaton may have changed slightly, and in \
particular, there is no transition with more than one color; when unset, \
the original automaton is returned.") },
    { DOC("gf-guarantee", "Set to 0 to disable alternate constructions \
for GF(guarantee)->[D]BA and FG(safety)->DCA.  Those constructions \
are from an LICS'18 paper by J. Esparza, J. Křentínský, and S. Sickert.  \
This is enabled by default for medium and high optimization \
levels.  Unless we are building deterministic automata, the \
resulting automata are compared to the automata built using the \
more traditional pipeline, and only kept if they are better.") },
    { DOC("simul", "Set to 0 to disable simulation-based reductions.  \
Set to 1 to use only direct simulation.  Set to 2 to use only reverse \
simulation.  Set to 3 to iterate both direct and reverse simulations. \
The default is 3, except when option --low is specified, in which case \
the default is 1.") },
    { DOC("ba-simul", "Set to 0 to disable simulation-based reductions \
on automata where state-based acceptance must be preserved (e.g., \
after degeneralization has been performed). The name suggests this applies \
only to Büchi automata for historical reasons; it really applies to any \
state-based acceptance nowadays. \
Set to 1 to use only direct simulation.  Set to 2 to use only reverse \
simulation.  Set to 3 to iterate both direct and reverse simulations.   \
The default is the value of parameter \"simul\" in --high mode, and 0 \
therwise.") },
    { DOC("merge-states-min", "Number of states above which states are \
merged using a cheap approximation of a bisimulation quotient before \
attempting simulation-based reductions.  Defaults to 128.  Set to 0 to \
never merge states.") },
    { DOC("simul-max", "Number of states above which simulation-based \
reductions are skipped. Defaults to 4096.  Set to 0 to disable.  This \
applies to all simulation-based optimization, including those of the \
determinization algorithm.") },
    { DOC("simul-trans-pruning", "Number of equivalence classes above which \
simulation-based transition-pruning for non-deterministic automata is \
disabled.  Defaults to 512.  Set to 0 to disable.  This applies to all \
simulation-based reductions, as well as to the simulation-based optimization \
of the determinization algorithm.  Simulation-based reductions perform a \
number of BDD implication checks that is quadratic in the number of classes to \
implement transition pruning.  The number of equivalence classes is equal to \
the number of output states of the simulation-based reduction when \
transition-pruning is disabled, it is just an upper bound otherwise.") },
    { DOC("relabel-bool", "If set to a positive integer N, a formula \
with N atomic propositions or more will have its Boolean subformulas \
abstracted as atomic propositions during the translation to automaton. \
This relabeling can speeds the translation if a few Boolean subformulas \
use many atomic propositions.  This relabeling make sure \
the subexpressions that are replaced do not share atomic propositions.  \
By default N=4.  Setting this value to 0 will disable the rewriting.") },
    { DOC("relabel-overlap", "If set to a positive integer N, a formula \
with N atomic propositions or more will have its Boolean subformulas \
abstracted as atomic propositions during the translation to automaton. \
This version does not care about overlapping atomic propositions, so \
it can cause the created temporary automata to have incompatible \
combinations of atomic propositions that will be eventually be removed. \
This relabeling is attempted after relabel-bool.  By default, N=8.  Setting \
this value to 0 will disable the rewriting.") },
    { DOC("wdba-minimize", "Set to 0 to disable WDBA-minimization, to 1 to \
always try it, or 2 to attempt it only on syntactic obligations or on automata \
that are weak and deterministic.  The default is 1 in --high mode, else 2 in \
--medium or --deterministic modes, else 0 in --low mode.") },
    { DOC("wdba-det-max", "Maximum number of additional states allowed \
in intermediate steps of WDBA-minimization. If the number of additional \
states reached in the powerset construction or in the followup products \
exceeds this value, WDBA-minimization is aborted.  \
Defaults to 4096.  Set to 0 to disable.  This limit is ignored when -D used \
or when det-max-states is set.") },
    { DOC("tba-det", "Set to 1 to attempt a powerset determinization \
if the TGBA is not already deterministic.  Doing so will degeneralize \
the automaton.  This is disabled by default, unless sat-minimize is set.") },
    { DOC("dba-simul", "Set to 1 to enable simulation-based reduction after \
running the powerset determinization enabled by \"tba-det\".  By default, this \
is disabled at low level or if parameter \"simul\" is set to 0.") },
    { DOC("sat-minimize",
          "Set to a value between 1 and 4 to enable SAT-based minimization \
of deterministic ω-automata.  If the input has n states, a SAT solver is \
used to find an equivalent automaton with 1≤m<n states.  The value between \
1 and 4 selects how the lowest possible m is searched, see the SAT-MINIMIZE \
VALUE section.  SAT-based minimization uses PicoSAT (embedded in Spot), \
but another installed SAT-solver can be set thanks to the SPOT_SATSOLVER \
environment variable.  Enabling SAT-based minimization will also enable \
tba-det.") },
    { DOC("sat-incr-steps", "Set the value of sat-incr-steps.  This variable \
is used by two SAT-based minimization algorithms: (2) and (3).  They are both \
described below.") },
    { DOC("sat-langmap", "Find the lower bound of default sat-minimize \
procedure (1).  This relies on the fact that the size of the minimal automaton \
is at least equal to the total number of different languages recognized by \
the automaton's states.") },
    { DOC("sat-states",
          "When this is set to some positive integer, the SAT-based \
minimization will attempt to construct an automaton with the given number of \
states.  It may however return an automaton with fewer states if some of \
these are unreachable or useless.  Setting sat-states automatically \
enables sat-minimize, but no iteration is performed.  If no equivalent \
automaton could be constructed with the given number of states, the original \
automaton is returned.") },
    { DOC("sat-acc",
          "When this is set to some positive integer, the SAT-based will \
attempt to construct a TGBA with the given number of acceptance sets.  \
It may however return an automaton with fewer acceptance sets if \
some of these are useless.  Setting sat-acc automatically \
sets sat-minimize to 1 if not set differently.") },
    { DOC("state-based",
          "Set to 1 to instruct the SAT-minimization procedure to produce \
an automaton where all outgoing transition of a state have the same acceptance \
sets.  By default, this is only enabled when options -B or -S are used.") },
    { DOC("simul-method",
          "Chose which simulation based reduction to use: 1 force the \
signature-based BDD implementation, 2 force matrix-based and 0, the default, \
is a heuristic which chooses which implementation to use.") },
    { nullptr, 0, nullptr, 0, nullptr, 0 }
  };

const struct argp_child children[] =
  {
    { &misc_argp_hidden, 0, nullptr, -1 },
    { nullptr, 0, nullptr, 0 }
  };

int
main(int argc, char** argv)
{
  setup(argv);

  const argp ap = { options, nullptr, nullptr, argp_program_doc, children,
                    nullptr, nullptr };

  if (int err = argp_parse(&ap, argc, argv, ARGP_NO_HELP, nullptr, nullptr))
    exit(err);

  std::cerr << "This binary serves no purpose other than generating"
            << " the spot-x.7 manpage.\n";

  return 1;
}
