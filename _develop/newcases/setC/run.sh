#!/bin/sh
###################
#  Emperical setA #
###################
HYPERQB="./hyperqb.sh"
SEMANTICS="-pes"
MODE="-bughunt"

### [case_small]

### [infoflow1] 
# CASE="setC/basic/infoflow1/infoflow1.smv" # absolute path for .smv
# PROP="setC/basic/infoflow1/infoflow1.hq"  # absolute path for .hq 
# ${HYPERQB} ${CASE} ${CASE} ${PROP} 5 ${SEMANTICS} ${MODE}

### [infoflow2]

# CASE="setC/basic/infoflow2/infoflow2.smv" # absolute path for .smv
# PROP="setC/basic/infoflow2/infoflow2.hq"  # absolute path for .hq 
# ${HYPERQB} ${CASE} ${CASE} ${PROP} 5 ${SEMANTICS} ${MODE}

### [case_medium]

# TODO
# CASE="setC/case_medium/addition/addition.smv"
# PROP="setC/case_medium/addition/addition.hq" 
# ${HYPERQB} ${CASE} ${CASE} ${PROP} 5 ${SEMANTICS} ${MODE}

# CASE="setC/case_medium/fig3/fig3.smv"
# PROP="setC/case_medium/fig3/fig3.hq" 
# ${HYPERQB} ${CASE} ${CASE} ${PROP} 5 ${SEMANTICS} ${MODE}

# CASE="setC/case_medium/LoopTest/LoopTest.smv"
# PROP="setC/case_medium/LoopTest/LoopTest.hq" 
# ${HYPERQB} ${CASE} ${CASE} ${PROP} 5 ${SEMANTICS} ${MODE}

CASE="setC/case_medium/nested_condition/nested_condition.smv"
PROP="setC/case_medium/nested_condition/nested_condition.hq" 
${HYPERQB} ${CASE} ${CASE} ${PROP} 5 ${SEMANTICS} ${MODE}



### [case_large]

# CASE="setC/case_large/amt.smv"
# PROP="setC/case_large/amt.hq" 
# ${HYPERQB} ${CASE} ${CASE} ${PROP} 5 ${SEMANTICS} ${MODE}













