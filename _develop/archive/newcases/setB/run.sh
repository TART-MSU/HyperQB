#!/bin/sh
###################
#  Emperical setB #
###################
HYPERQB="./hyperqb.sh"
SEMANTICS="-pes"
MODE="-bughunt"

# ### [infoflow1] 
# CASE="setB/setB/basic/infoflow1/infoflow1.smv" # absolute path for .smv
# PROP="setB/setB/basic/infoflow1/infoflow1.hq"  # absolute path for .hq 
# ${HYPERQB} ${CASE} ${CASE} ${PROP} 5 ${SEMANTICS} ${MODE}

# ### [infoflow2]
# CASE="setB/setB/basic/infoflow2/infoflow2.smv" # absolute path for .smv
# PROP="setB/setB/basic/infoflow2/infoflow2.hq"  # absolute path for .hq 
# ${HYPERQB} ${CASE} ${CASE} ${PROP} 5 ${SEMANTICS} ${MODE}

# # TODO

# ### [case_small]
# ### [coll item sym]
# CASE="temp/setB/case_medium/coll_item_sym/coll_item_sym_bounded.smv" # absolute path for .smv
# PROP="temp/setB/case_medium/coll_item_sym/coll_item_sym.hq" # absolute path for .hq 
# # PROP="temp/setB/case_medium/exp1x3/exp1x3.hq"
# ${HYPERQB} ${CASE} ${CASE} ${PROP} 5 ${SEMANTICS} ${MODE}
# # TODO

### [exp1x3]
CASE="temp/setB/case_medium/exp1x3/exp1x3.smv" # absolute path for .smv
PROP="temp/setB/case_medium/exp1x3/exp.hq" # absolute path for .hq 
${HYPERQB} ${CASE} ${CASE} ${PROP} 100 ${SEMANTICS} ${MODE}

### [case_large]
# CASE="temp/setB/case_large/bank4.smv" # absolute path for .smv
# PROP="temp/setB/case_large/bank.hq" # absolute path for .hq 
# ${HYPERQB} ${CASE} ${CASE} ${PROP} 5 ${SEMANTICS} ${MODE}

# TODO













