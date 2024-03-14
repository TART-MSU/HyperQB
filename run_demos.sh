#!/bin/sh
############################
#  Simple demos of HyperQB #
############################
HYPERQB="./hyperqb.sh"

### [demo 0: run simple_krip with simple_krip_formula]
# ${HYPERQB} demo/mini.smv demo/mini.smv demo/mini.hq 5 -pes -bughunt

${HYPERQB} demo/nway.smv demo/nway.smv demo/nway.hq 1 -pes -find

# ${HYPERQB} test/coll_item_sym.smv test/coll_item_sym.smv test/infoflow1.hq 2 -pes -bughunt

# ${HYPERQB} demo/mini2.smv demo/mini2.smv demo/mini2.hq 3 -pes -find

### [demo 1: run info-flow example]
# ${HYPERQB} demo/infoflow.smv demo/infoflow.smv demo/infoflow1.hq 5 -pes -debug
# ${HYPERQB} demo/infoflow.smv demo/infoflow.smv demo/infoflow.smv demo/infoflow2.hq 5 -pes -debug

### [demo 2: run bakery algorithm with symmetry property]
# ${HYPERQB} demo/bakery.smv demo/bakery.smv demo/symmetry.hq 10 -pes -debug

### [demo 3: run SNARK algorithm with linearizability propoerty, bug1]
# ${HYPERQB} demo/snark1_conc.smv demo/snark1_seq.smv demo/lin.hq 18 -pes -debug








