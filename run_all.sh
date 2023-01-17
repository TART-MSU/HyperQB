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
# ${HYPERQUBE} demo/bakery.smv demo/symmetry.hq 10 ${PES} ${SINGLE} ${BUGHUNT}
# ${HYPERQUBE} demo/bakery.smv demo/bakery.smv demo/symmetry.hq 10 ${PES} ${MULTI} ${BUGHUNT}

# [DEMO 2: run SNARK1 with linearizabilty property]
# ${HYPERQUBE} demo/snark_conc.smv demo/snark_seq.smv demo/linearizability.hq 10 ${PES} ${MULTI} ${BUGHUNT}


# [Deniability]
# CASEFOLDER=cases_bmc/deniability/
# ${HYPERQUBE} ${CASEFOLDER}electronic_wallet.smv dummy.hq 15 ${PES} ${SINGLE} ${FIND}


# [Coterm]
${HYPERQUBE} cases_bmc/cav_coterm/coterm1.smv cases_bmc/cav_coterm/coterm1.smv cases_bmc/cav_coterm/coterm.hq 10 ${OPT} ${MULTI} ${BUGHUNT}
# ${HYPERQUBE} cases_bmc/cav_coterm/coterm2.smv cases_bmc/cav_coterm/coterm2.smv cases_bmc/cav_coterm/coterm.hq 10 ${OPT} ${MULTI} ${BUGHUNT}
