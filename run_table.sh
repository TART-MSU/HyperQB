#!/bin/sh
##################################################
#  This script runs ALL experiments (0.1-11.1)   #
##################################################

### Requirements:
### 1. Before running, please make sure Docker is installed: (https://docs.docker.com/get-docker/)
### 2. Apple Chip Macs might need to use: --platform linux/amd64

### HyperQB Tool Parameters
HyperQB=./hyperqb.sh
PES='-pes' # default value
OPT='-opt'
HPES='-hpes'
HOPT='-hopt'
BUGHUNT="-bughunt" # default value is -bughunt
FIND="-find"



###################
# All Experiments #
###################

### EXPERIMENTS FROM TACAS'21 PAPER ###

CASEFOLDER="benchmarks"
CASE="1_bakery"
### [0.1-0.3 BAKERY] 
# ${HyperQB} ${CASEFOLDER}/${CASE}/bakery_3procs.smv ${CASEFOLDER}/${CASE}/bakery_3procs.smv ${CASEFOLDER}/${CASE}/bakery_phi_S1_3proc.hq 7 ${PES} ${FIND}
# ${HyperQB} ${CASEFOLDER}/${CASE}/bakery_3procs.smv ${CASEFOLDER}/${CASE}/bakery_3procs.smv ${CASEFOLDER}/${CASE}/bakery_phi_S2_3proc.hq 12 ${PES} ${FIND}
# ${HyperQB} ${CASEFOLDER}/${CASE}/bakery_3procs.smv ${CASEFOLDER}/${CASE}/bakery_3procs.smv ${CASEFOLDER}/${CASE}/bakery_phi_S3_3proc.hq 20 ${OPT} ${FIND}
### [1.1-1.4 BAKERY]
# ${HyperQB} ${CASEFOLDER}/${CASE}/bakery_3procs.smv ${CASEFOLDER}/${CASE}/bakery_3procs.smv ${CASEFOLDER}/${CASE}/bakery_phi_sym1_3proc.hq 10 ${PES} ${BUGHUNT}
# ${HyperQB} ${CASEFOLDER}/${CASE}/bakery_3procs.smv ${CASEFOLDER}/${CASE}/bakery_3procs.smv ${CASEFOLDER}/${CASE}/bakery_phi_sym2_3proc.hq 10 ${PES} ${BUGHUNT}
# ${HyperQB} ${CASEFOLDER}/${CASE}/bakery_5procs.smv ${CASEFOLDER}/${CASE}/bakery_5procs.smv ${CASEFOLDER}/${CASE}/bakery_phi_sym1_5proc.hq 10 ${PES} ${BUGHUNT}
# ${HyperQB} ${CASEFOLDER}/${CASE}/bakery_5procs.smv ${CASEFOLDER}/${CASE}/bakery_5procs.smv ${CASEFOLDER}/${CASE}/bakery_phi_sym2_5proc.hq 10 ${PES} ${BUGHUNT}


CASE="2_snark"
### [2.1-2.2 SNARK 2.1-2.2]
# ${HyperQB} ${CASEFOLDER}/${CASE}/snark1_M1_concurrent.smv ${CASEFOLDER}/${CASE}/snark1_M2_sequential.smv  ${CASEFOLDER}/${CASE}/snark1_formula.hq 18 ${PES} ${FIND} 
# ${HyperQB} ${CASEFOLDER}/${CASE}/snark2_new_M1_concurrent.smv  ${CASEFOLDER}/${CASE}/snark2_new_M2_sequential.smv ${CASEFOLDER}/${CASE}/snark2_formula.hq 30 ${PES} ${FIND}


CASE="3_ni"
### [3.1-3.2 3-Thread]
# ${HyperQB} ${CASEFOLDER}/${CASE}/NI_incorrect.smv ${CASEFOLDER}/${CASE}/NI_incorrect.smv ${CASEFOLDER}/${CASE}/NI_formula.hq 57 ${HPES} ${BUGHUNT}
# ${HyperQB} ${CASEFOLDER}/${CASE}/NI_correct.smv ${CASEFOLDER}/${CASE}/NI_correct.smv ${CASEFOLDER}/${CASE}/NI_formula.hq 57 ${HOPT} ${BUGHUNT}


CASE="4_nrp"
### [4.1-4.2 Non-repudiation Protocol]
# ${HyperQB} ${CASEFOLDER}/${CASE}/NRP_incorrect.smv ${CASEFOLDER}/${CASE}/NRP_incorrect.smv ${CASEFOLDER}/${CASE}/NRP_formula.hq 15 ${HPES} ${BUGHUNT}
# ${HyperQB} ${CASEFOLDER}/${CASE}/NRP_correct.smv ${CASEFOLDER}/${CASE}/NRP_correct.smv ${CASEFOLDER}/${CASE}/NRP_formula.hq 15 ${HOPT}  ${BUGHUNT}


CASE="5_planning"
### [5 Robotic planning: Initial State Robustness]
# ${HyperQB} ${CASEFOLDER}/${CASE}/robotic_robustness_100.smv ${CASEFOLDER}/${CASE}/robotic_robustness_100.smv ${CASEFOLDER}/${CASE}/robotic_robustness_formula.hq 20 ${PES} ${FIND}
# ${HyperQB} ${CASEFOLDER}/${CASE}/robotic_robustness_400.smv ${CASEFOLDER}/${CASE}/robotic_robustness_400.smv  ${CASEFOLDER}/${CASE}/robotic_robustness_formula.hq 40 ${PES} ${FIND}
# ${HyperQB} ${CASEFOLDER}/${CASE}/robotic_robustness_1600.smv ${CASEFOLDER}/${CASE}/robotic_robustness_1600.smv ${CASEFOLDER}/${CASE}/robotic_robustness_formula.hq 80 ${PES} ${FIND}
# ${HyperQB} ${CASEFOLDER}/${CASE}/robotic_robustness_3600.smv ${CASEFOLDER}/${CASE}/robotic_robustness_3600.smv ${CASEFOLDER}/${CASE}/robotic_robustness_formula.hq 120 ${PES} ${FIND}


CASE="6_mutation"
### [6.1 Mutation Testing]
# ${HyperQB} ${CASEFOLDER}/${CASE}/mutation_testing.smv ${CASEFOLDER}/${CASE}/mutation_testing.smv ${CASEFOLDER}/${CASE}/mutation_testing.hq 10 ${HOPT} ${FIND}


### NEW ADDED EXPERIMENTS ###

CASE="7_coterm"
### [7.1 Coterm]
# ${HyperQB} ${CASEFOLDER}/${CASE}/coterm1.smv ${CASEFOLDER}/${CASE}/coterm2.smv ${CASEFOLDER}/${CASE}/coterm.hq 102 ${OPT} ${BUGHUNT}


CASE="8_deniability"
### [8.1 - 8.2 Deniability]
# time ${HyperQB} ${CASEFOLDER}/${CASE}/den_small.smv ${CASEFOLDER}/${CASE}/den_small.smv ${CASEFOLDER}/${CASE}/den_small.smv ${CASEFOLDER}/${CASE}/den_f1.hq 10 ${OPT} ${BUGHUNT}
# time ${HyperQB} ${CASEFOLDER}/${CASE}/den.smv ${CASEFOLDER}/${CASE}/den.smv ${CASEFOLDER}/${CASE}/den.smv ${CASEFOLDER}/${CASE}/den_f1.hq 20 ${OPT} ${BUGHUNT}


CASE="9_buffer"
### [9.1 - 9.3 Intransitive]
# ${HyperQB} ${CASEFOLDER}/${CASE}/unscheduled_buffer.smv ${CASEFOLDER}/${CASE}/unscheduled_buffer.smv ${CASEFOLDER}/${CASE}/classic_OD.hq 10 ${PES} ${BUGHUNT}
# ${HyperQB} ${CASEFOLDER}/${CASE}/scheduled_buffer.smv ${CASEFOLDER}/${CASE}/scheduled_buffer.smv ${CASEFOLDER}/${CASE}/intrans_OD.hq 10 ${OPT} ${BUGHUNT}
# ${HyperQB} ${CASEFOLDER}/${CASE}/scheduled_buffer.smv ${CASEFOLDER}/${CASE}/scheduled_buffer.smv ${CASEFOLDER}/${CASE}/intrans_GMNI.hq 10 ${PES} ${BUGHUNT}


CASE="10_NIexp"
### [10.1 - 10.2 TINI and TSNI]
# ${HyperQB} ${CASEFOLDER}/${CASE}/ni_example.smv ${CASEFOLDER}/${CASE}/ni_example.smv ${CASEFOLDER}/${CASE}/tini.hq 10 ${OPT}  ${BUGHUNT}
# ${HyperQB} ${CASEFOLDER}/${CASE}/ni_example.smv ${CASEFOLDER}/${CASE}/ni_example.smv ${CASEFOLDER}/${CASE}/tsni.hq 10 ${OPT} ${BUGHUNT}


CASE="11_ksafety" 
### [11.1 K-safety]
# ${HyperQB} ${CASEFOLDER}/${CASE}/doubleSquare.smv ${CASEFOLDER}/${CASE}/doubleSquare.smv ${CASEFOLDER}/${CASE}/doubleSquare1.hq 64 ${PES} ${BUGHUNT}



CASE="12_mapsynth"
# SMV_A=benchmarks/12_mapsynth/msynth_MA.smv
# SMV_B=benchmarks/12_mapsynth/msynth_MB.smv
# SMV_M=benchmarks/12_mapsynth/msynth_MM.smv
# HQ=benchmarks/12_mapsynth/msynth_f1.hq
# time ${HYPERQB} ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B} ${HQ} 5 -pes -find
# time ${HYPERQB} ${CASEFOLDER}/${CASE}/msynth_MM.smv ${CASEFOLDER}/${CASE}/msynth_MA.smv ${CASEFOLDER}/${CASE}/msynth_MB.smv ${CASEFOLDER}/${CASE}/msynth_f1.hq 5 -pes -find

# time ${HYPERQB} benchmarks/12_mapsynth/msynth_MM.smv benchmarks/12_mapsynth/msynth_MA.smv benchmarks/12_mapsynth/msynth_MB.smv benchmarks/12_mapsynth/msynth_f1.hq 5 -pes -find

# SMV_A='benchmarks/12_mapsynth/msynth2_MA.smv'
# SMV_B='benchmarks/12_mapsynth/msynth2_MB.smv'
# SMV_M='benchmarks/12_mapsynth/msynth2_MM.smv'
# HQ='cases_compare/msynth2_f1.hq'
# time ${HYPERQB} ${CASEFOLDER}/${CASE}/msynth_MM.smv ${CASEFOLDER}/${CASE}/msynth_MA.smv ${CASEFOLDER}/${CASE}/msynth_MB.smv ${CASEFOLDER}/${CASE}/msynth_f1.hq 8 -pes -find

# time ${HYPERQB} ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B} ${HQ} 8 -pes -find


CASE="13_teamltl"
# echo ${CASE}
# time ${HyperQB} ${CASEFOLDER}/${CASE}/team.smv ${CASEFOLDER}/${CASE}/team.smv ${CASEFOLDER}/${CASE}/team.smv ${CASEFOLDER}/${CASE}/team_f1.hq 10 ${PES} ${BUGHUNT}
# time ${HyperQB} ${CASEFOLDER}/${CASE}/team2.smv ${CASEFOLDER}/${CASE}/team2.smv ${CASEFOLDER}/${CASE}/team2.smv ${CASEFOLDER}/${CASE}/team_f1.hq 20 ${PES} ${BUGHUNT}


CASE="14_ndet"
# time ${HyperQB} ${CASEFOLDER}/${CASE}/NI_v1.smv ${CASEFOLDER}/${CASE}/NI_v1.smv ${CASEFOLDER}/${CASE}/NI_v1.smv ${CASEFOLDER}/${CASE}/NI_f1.hq 5 ${PES} ${BUGHUNT}
# time ${HyperQB} ${CASEFOLDER}/${CASE}/NI_v2.smv ${CASEFOLDER}/${CASE}/NI_v2.smv  ${CASEFOLDER}/${CASE}/NI_f1.hq 5 ${PES} ${BUGHUNT}
# time ${HyperQB} ${CASEFOLDER}/${CASE}/NI_v3.smv ${CASEFOLDER}/${CASE}/NI_v3.smv  ${CASEFOLDER}/${CASE}/NI_f1.hq 5 ${PES} ${BUGHUNT}