#!/bin/sh
##########################
#  Simple exp of HyperQB #
##########################
# HYPERQB_NNF="./hyperqb.sh"
# HYPERQB_test="./hyperqb_test.sh"

# MODEL="demo/nway63.smv" # test on {demo/nway3, 7, 15, 31, 63}
# HP="demo/nway_AE.hq" # test on {nway_EE, AE, EA, AA}
# ENCODING="-NN" # test on {"-NN", "-YN", "-NY", "-YY"}

# K=30 # (!) fine tune this for each case so there are observable differences (i.e., not just 0.5s different)


# MODEL="demo/bakery.smv" 
# HP="demo/symmetry.hq" 

# MODEL="demo/test.smv"
# MODEL="_develop/newcases/bounded_loop.smv"
# MODEL="_develop/newcases/counter_det.smv"
# MODEL="_develop/newcases/fig2.smv"
# HP="demo/test.hq"

# MODEL="demo/check.smv"
# HP="demo/check.hq"

# MODEL="demo/nway63.smv"
# HP="demo/nway_AA.hq"

# MODEL="demo/cmu1.smv"
# HP="demo/cmu1.hq"
# K=10

# ENCODING="-NN" 

# ${HYPERQB_test} ${MODEL} ${MODEL} ${MODEL} ${HP} ${K} -pes -find ${ENCODING}




HYPERQB="./hyperqb.sh"
SEMANTICS="-pes"
MODE="-bughunt"
K=20

CASE="setC/case_medium/fig3/fig3.smv"
PROP="setC/case_medium/fig3/fig3.hq"

${HYPERQB} ${CASE} ${CASE} ${PROP} ${K} ${SEMANTICS} ${MODE}

