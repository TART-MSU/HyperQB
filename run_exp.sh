#!/bin/sh
##########################
#  Simple exp of HyperQB #
##########################
HYPERQB_NNF="./hyperqb.sh"
HYPERQB_nonNNF="./hyperqb_test.sh"

MODEL="demo/nway63.smv" # test on {demo/nway3, 7, 15, 31, 63}
HP="demo/nway_AE.hq" # test on {nway_EE, AE, EA, AA}
ENCODING="-NN" # test on {"-NN", "-YN", "-NY", "-YY"}

K=30 # (!) fine tune this for each case so there are observable differences (i.e., not just 0.5s different)

${HYPERQB_NNF} ${MODEL} ${MODEL} ${HP} ${K} -pes -find ${ENCODING}








