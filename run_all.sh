#!/bin/sh

# This script runs experiments we provided in the tool paper.
# * Requirements *
#   Before running, please make sure Docker is installed: (https://docs.docker.com/get-docker/)

# HyperQube Tool Parameters
HYPERQUBE=./hyperqube.sh

PES='-pes' # default value
OPT='-opt'
SINGLE='-single'
MULTI='-multi'
BUGHUNT="-bughunt" # default value
FIND="-find"

DUMMY="cases_bmc/dummy.hq"

# M1 Mac use:
# --platform linux/amd64

# [DEMO 1: run bakery algorithm with symmetry property]
# ${HYPERQUBE} demo/bakery.smv demo/bakery.smv demo/symmetry.hq 10 ${PES} ${MULTI} ${BUGHUNT}
# ${HYPERQUBE} demo/bakery.smv demo/bakery.smv demo/test.hq 2 ${PES} ${MULTI} ${FIND}
# ${HYPERQUBE} demo/bakery.smv demo/bakery.smv demo/test.hq 2 ${PES} ${MULTI}${FIND}
# ${HYPERQUBE} demo/tiny.smv demo/tiny.smv demo/mini_P.hq 5 ${PES} ${MULTI} ${FIND}
# ${HYPERQUBE} demo/mini.smv demo/mini.smv demo/mini_P.hq 3 ${PES} ${MULTI} ${FIND}
# ${HYPERQUBE} demo/bakery.smv demo/bakery.smv demo/symmetry.hq 2 ${PES} ${MULTI} ${FIND}
# [DEMO 2: run SNARK1 with linearizabilty property]
# ${HYPERQUBE} demo/snark_conc.smv demo/snark_seq.smv demo/lin.hq 3 ${PES} ${MULTI} ${BUGHUNT}



# [Deniability]
# CASEFOLDER=cases_bmc/deniability/
# ${HYPERQUBE} ${CASEFOLDER}electronic_wallet.smv dummy.hq 15 ${PES} ${SINGLE} ${FIND}


# [Coterm]
# ${HYPERQUBE} cases_bmc/cav_coterm/coterm1.smv cases_bmc/cav_coterm/coterm2.smv cases_bmc/cav_coterm/coterm.hq 102 ${OPT} ${MULTI} ${BUGHUNT}

# [K-safety]
# 1. doubleSquare
# ${HYPERQUBE} cases_bmc/cav_ksafety/doubleSquare.smv cases_bmc/cav_ksafety/doubleSquare.smv cases_bmc/cav_ksafety/doubleSquare1.hq 64 ${PES} ${MULTI} ${BUGHUNT}
# 2. arrayinsert
# ${HYPERQUBE} cases_bmc/cav_ksafety/arrayinsert.smv cases_bmc/cav_ksafety/arrayinsert.smv cases_bmc/cav_ksafety/arrayinsert.hq 15 ${PES} ${MULTI} ${BUGHUNT}

#  [TINI]
# ${HYPERQUBE} cases_bmc/cav_tini/ni_example.smv cases_bmc/cav_tini/ni_example.smv cases_bmc/cav_tini/tini.hq 10 ${OPT} ${MULTI} ${BUGHUNT}
# [TSNI]
# ${HYPERQUBE} cases_bmc/cav_tsni/ni_example.smv cases_bmc/cav_tsni/ni_example.smv cases_bmc/cav_tsni/tsni.hq 10 ${OPT} ${MULTI} ${BUGHUNT}

# [intransitive]
# --- first, it violates classical OD (PES, SAT)
# ${HYPERQUBE} cases_bmc/cav_shared_buffer/unscheduled_buffer.smv cases_bmc/cav_shared_buffer/unscheduled_buffer.smv cases_bmc/cav_shared_buffer/classic_OD.hq 10 ${PES} ${MULTI} ${BUGHUNT}
# --- however, with scheduler involves, it satisfies intransitive_OD
${HYPERQUBE} cases_bmc/cav_shared_buffer/scheduled_buffer.smv cases_bmc/cav_shared_buffer/scheduled_buffer.smv cases_bmc/cav_shared_buffer/intrans_OD.hq 10 ${OPT} ${MULTI} ${BUGHUNT}



# ${HYPERQUBE} cases_bmc/cav_shared_buffer/buffer.smv cases_bmc/cav_shared_buffer/buffer.smv cases_bmc/cav_shared_buffer/intransni.hq 10 ${OPT} ${MULTI} ${BUGHUNT}
