#!/bin/sh

# This script runs experiments we provided in the tool paper.
# * Requirements *
#   Before running, please make sure Docker is installed: (https://docs.docker.com/get-docker/)

# HyperQube Tool Parameters
HYPERQUBE=./hyperqube.sh
PESSIMISTIC='pes' # default value
OPTIMISTIC='opt'
SINGLEMODEL='-single'
MULTIMODEL='-multi'
BUGHUNT="-bughunt" # default value
FIND="-find"


# [DEMO 1: run bakery algorithm with symmetry property]
# CASEFOLDER=demo/
# ${HYPERQUBE} ${CASEFOLDER}bakery.smv ${CASEFOLDER}symmetry.hq 10 ${PESSIMISTIC} ${SINGLEMODEL} ${BUGHUNT}

# [Deniability]
CASEFOLDER=cases_bmc/deniability/
${HYPERQUBE} ${CASEFOLDER}electronic_wallet.smv dummy.hq 15 ${PESSIMISTIC} ${SINGLEMODEL} ${FIND}
