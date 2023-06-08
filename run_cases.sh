#!/bin/sh
#####################################################################################################
#  This script runs ALL experiments (0.1-11.1) we provided in Table 3 of our tool paper submission  #
#####################################################################################################

### HyperQB Tool Parameters
HyperQB=./hyperqb.sh
PES='-pes' # default value
OPT='-opt'
HPES='-hpes'
HOPT='-hopt'
# SINGLE='-single'
# MULTI='-multi'
BUGHUNT="-bughunt" # default value
FIND="-find"

### Requirements:
###   1. Before running, please make sure Docker is installed: (https://docs.docker.com/get-docker/)
###   2. Apple Chop Macs might need to use: --platform linux/amd64

##################################################
# All Experiments in CAV23 tool paper submission #
##################################################
### (1) Previous cases in TACAS21 submission:

### [0.1-0.3 BAKERY]
#### example to try #######
# ${HyperQB} cases_bmc/tacas_bakery/bakery_3procs.smv cases_bmc/tacas_bakery/bakery_3procs.smv cases_bmc/tacas_bakery/bakery_formula_S1_3proc.hq 7 ${PES} ${FIND}
# ${HyperQB} cases_bmc/tacas_bakery/bakery_3procs.smv cases_bmc/tacas_bakery/bakery_3procs.smv cases_bmc/tacas_bakery/bakery_formula_S2_3proc.hq 12 ${PES} ${FIND}
# ${HyperQB} cases_bmc/tacas_bakery/bakery_3procs.smv cases_bmc/tacas_bakery/bakery_3procs.smv cases_bmc/tacas_bakery/bakery_formula_S3_3proc.hq 20 ${OPT} ${FIND}
### [1.1-1.4 BAKERY]
# ${HyperQB} cases_bmc/tacas_bakery/bakery_3procs.smv cases_bmc/tacas_bakery/bakery_3procs.smv cases_bmc/tacas_bakery/bakery_formula_sym1_3proc.hq 10 ${PES} ${BUGHUNT}
# ${HyperQB} cases_bmc/tacas_bakery/bakery_3procs.smv cases_bmc/tacas_bakery/bakery_3procs.smv cases_bmc/tacas_bakery/bakery_formula_sym2_3proc.hq 10 ${PES} ${BUGHUNT}


${HyperQB} cases_bmc/tacas_bakery/bakery_5procs.smv cases_bmc/tacas_bakery/bakery_5procs.smv cases_bmc/tacas_bakery/bakery_formula_sym1_5proc.hq 10 ${PES} ${BUGHUNT}
# ${HyperQB} cases_bmc/tacas_bakery/bakery_5procs.smv cases_bmc/tacas_bakery/bakery_5procs.smv cases_bmc/tacas_bakery/bakery_formula_sym2_5proc.hq 10 ${PES} ${BUGHUNT}

### [2.1-2.2 SNARK 2.1-2.2]
## example to try  ######
# ${HyperQB} cases_bmc/tacas_snark/snark1_M1_concurrent.smv cases_bmc/tacas_snark/snark1_M2_sequential.smv  cases_bmc/tacas_snark/snark1_formula.hq 18 ${PES} ${FIND}
# ${HyperQB} cases_bmc/tacas_snark/snark2_new_M1_concurrent.smv  cases_bmc/tacas_snark/snark2_new_M2_sequential.smv cases_bmc/tacas_snark/snark2_formula.hq 30 ${PES} ${FIND}

### [3.1-3.2 3-Thread]
### EXAMPLE TO TRY ####
# ${HyperQB} cases_bmc/tacas_multi_threaded/NI_incorrect.smv cases_bmc/tacas_multi_threaded/NI_incorrect.smv cases_bmc/tacas_multi_threaded/NI_formula.hq 57 ${HPES} ${BUGHUNT}
# ${HyperQB} cases_bmc/tacas_multi_threaded/NI_correct.smv cases_bmc/tacas_multi_threaded/NI_correct.smv cases_bmc/tacas_multi_threaded/NI_formula.hq 57 ${HOPT} ${BUGHUNT}

### [4.1-4.2 Non-repudiation Protocol]
# ${HyperQB} cases_bmc/tacas_nrp/NRP_incorrect.smv cases_bmc/tacas_nrp/NRP_incorrect.smv cases_bmc/tacas_nrp/NRP_formula.hq 15 ${HPES} ${BUGHUNT}
# ${HyperQB} cases_bmc/tacas_nrp/NRP_correct.smv cases_bmc/tacas_nrp/NRP_correct.smv cases_bmc/tacas_nrp/NRP_formula.hq 15 ${HOPT}  ${BUGHUNT}

### -------------------------Table 5-----------------------------------
### [5.1 Robotic planning: Shortest Path]
# ${HyperQB} cases_bmc/tacas_robotic/robotic_sp_100.smv  cases_bmc/tacas_robotic/robotic_sp_100.smv cases_bmc/tacas_robotic/robotic_sp_formula.hq 20 ${PES} ${FIND}
# ${HyperQB} cases_bmc/tacas_robotic/robotic_sp_400.smv cases_bmc/tacas_robotic/robotic_sp_400.smv cases_bmc/tacas_robotic/robotic_sp_formula.hq 40 ${PES} ${FIND}
# ${HyperQB} cases_bmc/tacas_robotic/robotic_sp_1600.smv cases_bmc/tacas_robotic/robotic_sp_1600.smv cases_bmc/tacas_robotic/robotic_sp_formula.hq 80 ${PES} ${FIND}
# ${HyperQB} cases_bmc/tacas_robotic/robotic_sp_3600.smv cases_bmc/tacas_robotic/robotic_sp_3600.smv cases_bmc/tacas_robotic/robotic_sp_formula.hq 120 ${PES} ${FIND}

### [5.2 Robotic planning: Initial State Robustness]
# ${HyperQB} cases_bmc/tacas_robotic/robotic_robustness_100.smv cases_bmc/tacas_robotic/robotic_robustness_100.smv cases_bmc/tacas_robotic/robotic_robustness_formula.hq 20 ${PES} ${FIND}
# ${HyperQB} cases_bmc/tacas_robotic/robotic_robustness_400.smv cases_bmc/tacas_robotic/robotic_robustness_400.smv  cases_bmc/tacas_robotic/robotic_robustness_formula.hq 40 ${PES} ${FIND}
# ${HyperQB} cases_bmc/tacas_robotic/robotic_robustness_1600.smv cases_bmc/tacas_robotic/robotic_robustness_1600.smv cases_bmc/tacas_robotic/robotic_robustness_formula.hq 80 ${PES} ${FIND}
# ${HyperQB} cases_bmc/tacas_robotic/robotic_robustness_3600.smv cases_bmc/tacas_robotic/robotic_robustness_3600.smv cases_bmc/tacas_robotic/robotic_robustness_formula.hq 120 ${PES} ${FIND}

### --------------------------Table 5----------------------------------

### [6.1 Mutation Testing]
# ${HyperQB} cases_bmc/tacas_mutation_testing/mutation_testing.smv cases_bmc/tacas_mutation_testing/mutation_testing.smv cases_bmc/tacas_mutation_testing/mutation_testing.hq 10 ${HOPT} ${FIND}


### (2) New cases in CAV23 submission compare to TACAS21 submission:

### [7.1 Coterm]
# ${HyperQB} cases_bmc/cav_coterm/coterm1.smv cases_bmc/cav_coterm/coterm2.smv cases_bmc/cav_coterm/coterm.hq 102 ${OPT} ${BUGHUNT}

### [8.1 Deniability]
# ${HyperQB} cases_bmc/cav_deniability/electronic_wallet.smv cases_bmc/cav_deniability/electronic_wallet.smv cases_bmc/cav_deniability/electronic_wallet.smv cases_bmc/cav_deniability/den.hq 20 ${OPT} ${BUGHUNT}

### [9.1 - 9.3 Intransitive]
### first, it violates classic_OD (PES, SAT)
# ${HyperQB} cases_bmc/cav_shared_buffer/unscheduled_buffer.smv cases_bmc/cav_shared_buffer/unscheduled_buffer.smv cases_bmc/cav_shared_buffer/classic_OD.hq 10 ${PES} ${BUGHUNT}
### however, with scheduler involves, it satisfies intransitive_OD (OPT, UNSAT)
# ${HyperQB} cases_bmc/cav_shared_buffer/scheduled_buffer.smv cases_bmc/cav_shared_buffer/scheduled_buffer.smv cases_bmc/cav_shared_buffer/intrans_OD.hq 10 ${OPT} ${BUGHUNT}
### next, it violates
### same, the scheduler guarantees GMNI
# ${HyperQB} cases_bmc/cav_shared_buffer/scheduled_buffer.smv cases_bmc/cav_shared_buffer/scheduled_buffer.smv cases_bmc/cav_shared_buffer/intrans_GMNI.hq 10 ${PES} ${BUGHUNT}

### [10.1 - 10.2 TINI and TSNI]
# ${HyperQB} cases_bmc/cav_tini/ni_example.smv cases_bmc/cav_tini/ni_example.smv cases_bmc/cav_tini/tini.hq 10 ${OPT}  ${BUGHUNT}
# ${HyperQB} cases_bmc/cav_tsni/ni_example.smv cases_bmc/cav_tsni/ni_example.smv cases_bmc/cav_tsni/tsni.hq 10 ${OPT} ${BUGHUNT}

### [11.1 K-safety]
### doubleSquare
# ${HyperQB} cases_bmc/cav_ksafety/doubleSquare.smv cases_bmc/cav_ksafety/doubleSquare.smv cases_bmc/cav_ksafety/doubleSquare1.hq 64 ${PES} ${BUGHUNT}
