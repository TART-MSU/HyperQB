#!/bin/sh
############################
#  Simple demos of HyperQB #
############################
HYPERQB="./hyperqb.sh"
HYPERQB2="./hyperqb2.sh"

# exec/mac/genqbf -I src/gentr/I.bool -R src/gentr/R.bool -J src/gentr/I.bool -S src/gentr/R.bool -P src/gentr/P.hq -k 2 -F "EE" -f qcir -o src/gentr/out.qcir -sem "PES" -n --fast -new "NN" --debug

# exec/mac/genqbf -I build_today/I_1.bool -R build_today/R_1.bool -J build_today/I_2.bool -S build_today/I_2.bool -P build_today/P.hq -k 2 -F "EE" -f qcir -o out.qcir -sem "PES" -n --fast -new "NN" --debug

### [demo 0: run simple_krip with simple_krip_formula]
# ${HYPERQB} demo/mini.smv demo/mini.smv demo/mini.hq 3 -pes -find -NN
# ${HYPERQB} demo/mini2.smv demo/mini2.smv demo/mini2.hq 3 -pes -find

### [demo 1: run info-flow example]
# ${HYPERQB} demo/infoflow.smv demo/infoflow.smv demo/infoflow1.hq 5 -pes -debug
# ${HYPERQB} demo/infoflow.smv demo/infoflow.smv demo/infoflow.smv demo/infoflow2.hq 5 -pes -debug

### [demo 2: run bakery algorithm with symmetry property]
# ${HYPERQB} demo/bakery.smv demo/bakery.smv demo/symmetry.hq 30 -pes -debug
# ${HYPERQB2} demo/bakery.smv demo/bakery.smv demo/symmetry.hq 30 -pes -debug

### [demo 3: run SNARK algorithm with linearizability propoerty, bug1]
# ${HYPERQB} demo/snark1_conc.smv demo/snark1_seq.smv demo/lin.hq 18 -pes -debug








