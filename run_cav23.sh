#!/bin/sh
### HyperQB Tool Parameters
HyperQB=./hyperqbsh
PES='-pes' # default value
OPT='-opt'
SINGLE='-single'
MULTI='-multi'
BUGHUNT="-bughunt" # default value
FIND="-find"

### This script runs all NEW experiments (7.1-11.1) we provided in Table 3 of our tool paper submission.
### To run complete table (with 0.1-6.1 from our previous paper), please use run_allcases.sh
### please uncomment any line with a single leading `#` and execute ./run_cav23.sh in the terminal

### Requirements:
###   1. Before running, please make sure Docker is installed: (https://docs.docker.com/get-docker/)
###   2. Apple Chop Macs might need to use: --platform linux/amd64


### [CAV running example]
# ${HyperQB} demo/cav_running_exp.smv demo/cav_running_exp.smv demo/cav_running_exp.hq 3 ${PES} ${MULTI} ${BUGHUNT}

### [7.1 Coterm]
# ${HyperQB} cases_bmc/cav_coterm/coterm1.smv cases_bmc/cav_coterm/coterm2.smv cases_bmc/cav_coterm/coterm.hq 102 ${OPT} ${MULTI} ${BUGHUNT}

### [8.1 Deniability]
# ${HyperQB} cases_bmc/cav_deniability/electronic_wallet.smv cases_bmc/cav_deniability/electronic_wallet.smv cases_bmc/cav_deniability/den.hq 20 ${PES} ${MULTI} ${BUGHUNT}

### [9.1 - 9.3 Intransitive]
### first, it violates classic_OD (PES, SAT)
# ${HyperQB} cases_bmc/cav_shared_buffer/unscheduled_buffer.smv cases_bmc/cav_shared_buffer/unscheduled_buffer.smv cases_bmc/cav_shared_buffer/classic_OD.hq 10 ${PES} ${MULTI} ${BUGHUNT}
### however, with scheduler involves, it satisfies intransitive_OD (OPT, UNSAT)
# ${HyperQB} cases_bmc/cav_shared_buffer/scheduled_buffer.smv cases_bmc/cav_shared_buffer/scheduled_buffer.smv cases_bmc/cav_shared_buffer/intrans_OD.hq 10 ${OPT} ${MULTI} ${BUGHUNT}
### next, it violates
### same, the scheduler guarantees GMNI
# ${HyperQB} cases_bmc/cav_shared_buffer/scheduled_buffer.smv cases_bmc/cav_shared_buffer/scheduled_buffer.smv cases_bmc/cav_shared_buffer/intrans_GMNI.hq 10 ${PES} ${MULTI} ${BUGHUNT}

### [10.1 - 10.2 TINI and TSNI]
# ${HyperQB} cases_bmc/cav_tini/ni_example.smv cases_bmc/cav_tini/ni_example.smv cases_bmc/cav_tini/tini.hq 10 ${OPT} ${MULTI} ${BUGHUNT}
# ${HyperQB} cases_bmc/cav_tsni/ni_example.smv cases_bmc/cav_tsni/ni_example.smv cases_bmc/cav_tsni/tsni.hq 10 ${OPT} ${MULTI} ${BUGHUNT}

### [11.1 K-safety]
### doubleSquare
# ${HyperQB} cases_bmc/cav_ksafety/doubleSquare.smv cases_bmc/cav_ksafety/doubleSquare.smv cases_bmc/cav_ksafety/doubleSquare1.hq 64 ${PES} ${MULTI} ${BUGHUNT}
