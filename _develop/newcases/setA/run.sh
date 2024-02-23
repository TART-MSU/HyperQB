#!/bin/sh
###################
#  Emperical setA #
###################
HYPERQB="./hyperqb.sh"
SEMANTICS="-pes"
MODE="-bughunt"

### [infoflow1] 
CASE="temp/setA/basic/infoflow1/infoflow1.smv" # absolute path for .smv
PROP="temp/setA/basic/infoflow1/infoflow1.hq"  # absolute path for .hq 
${HYPERQB} ${CASE} ${CASE} ${PROP} 5 ${SEMANTICS} ${MODE}

### [infoflow2]

# TODO

### [case_small]

# TODO

### [case_large]

# TODO













