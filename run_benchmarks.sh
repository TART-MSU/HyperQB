#!/bin/sh
##################################################
#  This script runs ALL experiments (0.1-11.1)   #
##################################################
### Requirements:
### 1. Before running, please make sure Docker is installed: (https://docs.docker.com/get-docker/)
### 2. Apple Chip Macs might need to use: --platform linux/amd64

### HyperQB Tool Parameters
HyperQB=./hyperqb2.0.sh
# HyperQB2=./hyperqb_2.0.sh
# HyperQB_v=./run_versions.sh
PES='-pes' # default value
OPT='-opt'
HPES='-hpes'
HOPT='-hopt'
BUGHUNT="-bughunt" # default value is -bughunt
FIND="-find"

###################
# All Experiments #
###################

### (1) experiments from set1 :


CASEFOLDER="benchmarks"
PROJ="1_bakery"
### [0.1-0.3 BAKERY]
# ${HyperQB} ${CASEFOLDER}/${PROJ}/bakery_3procs.smv ${CASEFOLDER}/${PROJ}/bakery_3procs.smv ${CASEFOLDER}/${PROJ}/bakery_phi_S1_3proc.hq 7 ${PES} ${FIND}
# ${HyperQB} ${CASEFOLDER}/${PROJ}/bakery_3procs.smv ${CASEFOLDER}/${PROJ}/bakery_3procs.smv ${CASEFOLDER}/${PROJ}/bakery_phi_S2_3proc.hq 12 ${PES} ${FIND}
# ${HyperQB} ${CASEFOLDER}/${PROJ}/bakery_3procs.smv ${CASEFOLDER}/${PROJ}/bakery_3procs.smv ${CASEFOLDER}/${PROJ}/bakery_phi_S3_3proc.hq 20 ${OPT} ${FIND}
### [1.1-1.4 BAKERY]
# ${HyperQB} ${CASEFOLDER}/${PROJ}/bakery_3procs.smv ${CASEFOLDER}/${PROJ}/bakery_3procs.smv ${CASEFOLDER}/${PROJ}/bakery_phi_sym1_3proc.hq 10 ${PES} ${BUGHUNT}
# ${HyperQB} ${CASEFOLDER}/${PROJ}/bakery_3procs.smv ${CASEFOLDER}/${PROJ}/bakery_3procs.smv ${CASEFOLDER}/${PROJ}/bakery_phi_sym2_3proc.hq 10 ${PES} ${BUGHUNT}
# ${HyperQB} ${CASEFOLDER}/${PROJ}/bakery_5procs.smv ${CASEFOLDER}/${PROJ}/bakery_5procs.smv ${CASEFOLDER}/${PROJ}/bakery_phi_sym1_5proc.hq 10 ${PES} ${BUGHUNT}
# ${HyperQB} ${CASEFOLDER}/${PROJ}/bakery_5procs.smv ${CASEFOLDER}/${PROJ}/bakery_5procs.smv ${CASEFOLDER}/${PROJ}/bakery_phi_sym2_5proc.hq 10 ${PES} ${BUGHUNT}


PROJ="2_snark"
### [2.1-2.2 SNARK 2.1-2.2]
# ${HyperQB} ${CASEFOLDER}/${PROJ}/snark1_M1_concurrent.smv ${CASEFOLDER}/${PROJ}/snark1_M2_sequential.smv  ${CASEFOLDER}/${PROJ}/snark1_formula.hq 18 ${PES} ${FIND} 
# ${HyperQB} ${CASEFOLDER}/${PROJ}/snark2_new_M1_concurrent.smv  ${CASEFOLDER}/${PROJ}/snark2_new_M2_sequential.smv ${CASEFOLDER}/${PROJ}/snark2_formula.hq 30 ${PES} ${FIND}


PROJ="3_ni"
### [3.1-3.2 3-Thread]
# ${HyperQB} ${CASEFOLDER}/${PROJ}/NI_incorrect.smv ${CASEFOLDER}/${PROJ}/NI_incorrect.smv ${CASEFOLDER}/${PROJ}/NI_formula.hq 57 ${HPES} ${BUGHUNT}
# ${HyperQB} ${CASEFOLDER}/${PROJ}/NI_correct.smv ${CASEFOLDER}/${PROJ}/NI_correct.smv ${CASEFOLDER}/${PROJ}/NI_formula.hq 57 ${HOPT} ${BUGHUNT}


PROJ="4_nrp"
### [4.1-4.2 Non-repudiation Protocol]
# ${HyperQB} ${CASEFOLDER}/${PROJ}/NRP_incorrect.smv ${CASEFOLDER}/${PROJ}/NRP_incorrect.smv ${CASEFOLDER}/${PROJ}/NRP_formula.hq 15 ${HPES} ${BUGHUNT}
# ${HyperQB} ${CASEFOLDER}/${PROJ}/NRP_correct.smv ${CASEFOLDER}/${PROJ}/NRP_correct.smv ${CASEFOLDER}/${PROJ}/NRP_formula.hq 15 ${HOPT}  ${BUGHUNT}


PROJ="5_planning"
### [5.1 Robotic planning: Shortest Path]
# ${HyperQB} ${CASEFOLDER}/${PROJ}/robotic_sp_100.smv  ${CASEFOLDER}/${PROJ}/robotic_sp_100.smv ${CASEFOLDER}/${PROJ}/robotic_sp_formula.hq 20 ${PES} ${FIND}
# ${HyperQB} ${CASEFOLDER}/${PROJ}/robotic_sp_400.smv ${CASEFOLDER}/${PROJ}/robotic_sp_400.smv ${CASEFOLDER}/${PROJ}/robotic_sp_formula.hq 40 ${PES} ${FIND}
# ${HyperQB} ${CASEFOLDER}/${PROJ}/robotic_sp_1600.smv ${CASEFOLDER}/${PROJ}/robotic_sp_1600.smv ${CASEFOLDER}/${PROJ}/robotic_sp_formula.hq 80 ${PES} ${FIND}
# ${HyperQB} ${CASEFOLDER}/${PROJ}/robotic_sp_3600.smv ${CASEFOLDER}/${PROJ}/robotic_sp_3600.smv ${CASEFOLDER}/${PROJ}/robotic_sp_formula.hq 120 ${PES} ${FIND}
### [5.2 Robotic planning: Initial State Robustness]
# ${HyperQB} ${CASEFOLDER}/${PROJ}/robotic_robustness_100.smv ${CASEFOLDER}/${PROJ}/robotic_robustness_100.smv ${CASEFOLDER}/${PROJ}/robotic_robustness_formula.hq 20 ${PES} ${FIND}
# ${HyperQB} ${CASEFOLDER}/${PROJ}/robotic_robustness_400.smv ${CASEFOLDER}/${PROJ}/robotic_robustness_400.smv  ${CASEFOLDER}/${PROJ}/robotic_robustness_formula.hq 40 ${PES} ${FIND}
# ${HyperQB} ${CASEFOLDER}/${PROJ}/robotic_robustness_1600.smv ${CASEFOLDER}/${PROJ}/robotic_robustness_1600.smv ${CASEFOLDER}/${PROJ}/robotic_robustness_formula.hq 80 ${PES} ${FIND}
# ${HyperQB} ${CASEFOLDER}/${PROJ}/robotic_robustness_3600.smv ${CASEFOLDER}/${PROJ}/robotic_robustness_3600.smv ${CASEFOLDER}/${PROJ}/robotic_robustness_formula.hq 120 ${PES} ${FIND}


PROJ="6_mutation"
### [6.1 Mutation Testing]
# ${HyperQB} ${CASEFOLDER}/${PROJ}/mutation_testing.smv ${CASEFOLDER}/${PROJ}/mutation_testing.smv ${CASEFOLDER}/${PROJ}/mutation_testing.hq 10 ${HOPT} ${FIND}


PROJ="7_coterm"
### [7.1 Coterm]
# ${HyperQB} ${CASEFOLDER}/${PROJ}/coterm1.smv ${CASEFOLDER}/${PROJ}/coterm2.smv ${CASEFOLDER}/${PROJ}/coterm.hq 102 ${OPT} ${BUGHUNT}


PROJ="8_deniability"
### [8.1 Deniability]
# ${HyperQB} ${CASEFOLDER}/${PROJ}/electronic_wallet.smv ${CASEFOLDER}/${PROJ}/electronic_wallet.smv ${CASEFOLDER}/${PROJ}/electronic_wallet.smv ${CASEFOLDER}/cav_deniability/den.hq 20 ${OPT} ${BUGHUNT}


PROJ="9_buffer"
### [9.1 - 9.3 Intransitive]
### first, it violates classic_OD (PES, SAT)
# ${HyperQB} ${CASEFOLDER}/${PROJ}/unscheduled_buffer.smv ${CASEFOLDER}/${PROJ}/unscheduled_buffer.smv ${CASEFOLDER}/${PROJ}/classic_OD.hq 10 ${PES} ${BUGHUNT}
### however, with scheduler involves, it satisfies intransitive_OD (OPT, UNSAT)
# ${HyperQB} ${CASEFOLDER}/${PROJ}/scheduled_buffer.smv ${CASEFOLDER}/${PROJ}/scheduled_buffer.smv ${CASEFOLDER}/${PROJ}/intrans_OD.hq 10 ${OPT} ${BUGHUNT}
### same, the scheduler guarantees GMNI
# ${HyperQB} ${CASEFOLDER}/${PROJ}/scheduled_buffer.smv ${CASEFOLDER}/${PROJ}/scheduled_buffer.smv ${CASEFOLDER}/${PROJ}/intrans_GMNI.hq 10 ${PES} ${BUGHUNT}


PROJ="10_NIexp"
### [10.1 - 10.2 TINI and TSNI]
# ${HyperQB} ${CASEFOLDER}/${PROJ}/ni_example.smv ${CASEFOLDER}/${PROJ}/ni_example.smv ${CASEFOLDER}/${PROJ}/tini.hq 10 ${OPT}  ${BUGHUNT}
# ${HyperQB} ${CASEFOLDER}/${PROJ}/ni_example.smv ${CASEFOLDER}/${PROJ}/ni_example.smv ${CASEFOLDER}/${PROJ}/tsni.hq 10 ${OPT} ${BUGHUNT}


PROJ="11_continuity" 
### [11.1 K-safety]
# ${HyperQB} ${CASEFOLDER}/${PROJ}/doubleSquare.smv ${CASEFOLDER}/${PROJ}/doubleSquare.smv ${CASEFOLDER}/${PROJ}/doubleSquare1.hq 64 ${PES} ${BUGHUNT}






# echo "------(AutoHyper Starts)------"
# time ${AUTOHYPER} -nusmv ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B}  ${HQAUTO} -v 4
