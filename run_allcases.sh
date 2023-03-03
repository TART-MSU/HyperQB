#!/bin/sh
### HyperQB Tool Parameters
HyperQB=./hyperqbsh
PES='-pes' # default value
OPT='-opt'
HPES='-hpes'
HOPT='-hopt'
SINGLE='-single'
MULTI='-multi'
BUGHUNT="-bughunt" # default value
FIND="-find"

### This script runs all NEW experiments (0.1-6.1) we provided in Table 3 of our tool paper submission.
### To run previous experiments, please use run_tacas21.sh
### To run complete table, please use run_allcases.sh

### Requirements:
###   1. Before running, please make sure Docker is installed: (https://docs.docker.com/get-docker/)
###   2. Apple Chop Macs might need to use: --platform linux/amd64

### DEMOS
### [demo 1: run bakery algorithm with symmetry property]
# ./hyperqb.sh demo/bakery.smv demo/symmetry.hq 10 pes -single
### [demi 2: run SNARK algorithm with linearizability propoerty]
# ./hyperqb.sh demo/SNARK_conc.smv demo/SNARK_seq.smv demo/linearizability 18 pes -multi
### [demo 3: run simple_krip with simple_krip_formula]
# ./hyperqb.sh cases_bmc/simple_krip.smv  cases_bmc/simple_krip_formula.hq 5 pes


### All Experiments in CAV23 tool paper submission:

### (1) Previous cases in TACAS21 submission:

### [1.1-1.4 BAKERY]
${HyperQB} cases_bmc/bakery_3procs.smv cases_bmc/bakery_formula_sym1_3proc.hq 10 pes
${HyperQB} cases_bmc/bakery_3procs.smv cases_bmc/bakery_formula_sym2_3proc.hq 10 pes
${HyperQB} cases_bmc/bakery_5procs.smv cases_bmc/bakery_formula_sym1_5proc.hq 10 pes
${HyperQB} cases_bmc/bakery_5procs.smv cases_bmc/bakery_formula_sym2_5proc.hq 10 pes

### [2.1-2.2 SNARK 2.1-2.2]
${HyperQB} cases_bmc/snark1_M1_concurrent.smv cases_bmc/snark1_M2_sequential.smv  cases_bmc/snark1_formula.hq 18 pes
${HyperQB} cases_bmc/snark2_new_M1_concurrent.smv  cases_bmc/snark2_new_M2_sequential.smv cases_bmc/snark2_formula.hq 30 pes

### [3.1-3.2 3-Thread]
${HyperQB} cases_bmc/NI_incorrect.smv cases_bmc/NI_formula.hq 50 hpes
${HyperQB} cases_bmc/NI_correct.smv cases_bmc/NI_formula.hq 50 hopt

### [4.1-4.2 Non-repudiation Protocol]
${HyperQB} cases_bmc/NRP_incorrect.smv cases_bmc/NRP_formula.hq 15 hpes
${HyperQB} cases_bmc/NRP_correct.smv cases_bmc/NRP_formula.hq 15 hopt

### [5.1 Robotic planning: Shortest Path]
${HyperQB} cases_bmc/robotic_sp_100.smv  cases_bmc/robotic_sp_formula.hq 20 hpes -find
${HyperQB} cases_bmc/robotic_sp_400.smv cases_bmc/robotic_sp_formula.hq 40 hpes -find
${HyperQB} cases_bmc/robotic_sp_1600.smv cases_bmc/robotic_sp_formula.hq 80 hpes -find
${HyperQB} cases_bmc/robotic_sp_3600.smv cases_bmc/robotic_sp_formula.hq 120 hpes -find

### [5.1 Robotic planning: Initial State Robustness]
${HyperQB} cases_bmc/robotic_robustness_100.smv cases_bmc/robotic_robustness_formula.hq 20 hpes -find
${HyperQB} cases_bmc/robotic_robustness_400.smv cases_bmc/robotic_robustness_formula.hq 40 hpes -find
${HyperQB} cases_bmc/robotic_robustness_1600.smv cases_bmc/robotic_robustness_formula.hq 80 hpes -find
${HyperQB} cases_bmc/robotic_robustness_3600.smv cases_bmc/robotic_robustness_formula.hq 120 hpes -find

### [6.1 Mutation Testing]
${HyperQB} cases_bmc/mutation_testing.smv cases_bmc/mutation_testing.hq 10 hpes -find


### (2) New cases in CAV23 submission compare to TACAS21 submission:

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
