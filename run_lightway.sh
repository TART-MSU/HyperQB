#!/bin/bash
TIMEFORMAT="TOTAL: %Rs"

# HyperQB driver
HYPERQB="./hyperqb.sh"
CASEFOLDER="benchmarks/"


CASE="Case 0.1:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'1_bakery/bakery_3procs.smv'
    HQ=${CASEFOLDER}'1_bakery/bakery_phi_S1_3proc.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'0.1.hq'
    time ${HYPERQB} ${SMV} ${SMV} ${HQ} 7 -pes -find

CASE="Case 0.2:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'1_bakery/bakery_3procs.smv'
    HQ=${CASEFOLDER}'1_bakery/bakery_phi_S2_3proc.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'0.2.hq'
    time ${HYPERQB} ${SMV} ${SMV} ${HQ} 12 -pes -find

CASE="Case 0.3:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'1_bakery/bakery_3procs.smv'
    HQ=${CASEFOLDER}'1_bakery/bakery_phi_S3_3proc.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'0.3.hq'
    time ${HYPERQB} ${SMV} ${SMV} ${HQ} 20 -pes -find

CASE="Case 1.1:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'1_bakery/bakery_3procs.smv'
    HQ=${CASEFOLDER}'1_bakery/bakery_phi_sym1_3proc.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'1.1.hq'
    time ${HYPERQB} ${SMV} ${SMV} ${HQ} 10 -pes -bughunt
    

CASE="Case 1.2:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'1_bakery/bakery_3procs.smv'
    HQ=${CASEFOLDER}'1_bakery/bakery_phi_sym2_3proc.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'1.2.hq'
    time ${HYPERQB} ${SMV} ${SMV} ${HQ} 10 -pes -bughunt

CASE="Case 1.3:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'1_bakery/bakery_5procs.smv'
    HQ=${CASEFOLDER}'1_bakery/bakery_phi_sym1_5proc.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'1.3.hq'
    time ${HYPERQB} ${SMV} ${SMV} ${HQ} 10 -pes -bughunt

CASE="Case 1.4:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'1_bakery/bakery_5procs.smv'
    HQ=${CASEFOLDER}'1_bakery/bakery_phi_sym2_5proc.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'1.4.hq'
    time ${HYPERQB} ${SMV} ${SMV} ${HQ} 10 -pes -bughunt


CASE="Case 3.1:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'3_ni/NI_incorrect.smv'
    HQ=${CASEFOLDER}'3_ni/NI_formula.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'3.hq'
    time ${HYPERQB} ${SMV} ${SMV} ${HQ} 57 -hpes -bughunt


CASE="Case 3.2:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'3_ni/NI_correct.smv'
    HQ=${CASEFOLDER}'3_ni/NI_formula.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'3.hq'
    time ${HYPERQB} ${SMV} ${SMV} ${HQ} 57 -hopt -bughunt


CASE="Case 4.1:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'4_nrp/NRP_incorrect.smv'
    HQ=${CASEFOLDER}'4_nrp/NRP_formula.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'4.hq'
    time ${HYPERQB} ${SMV} ${SMV} ${HQ} 15 -hpes -find


CASE="Case 4.2:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'4_nrp/NRP_correct.smv'
    HQ=${CASEFOLDER}'4_nrp/NRP_formula.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'4.hq'
    time ${HYPERQB} ${SMV} ${SMV} ${HQ} 15 -hopt -find


CASE="Case 5.1:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'5_planning/robotic_robustness_100.smv'
    HQ=${CASEFOLDER}'5_planning/robotic_robustness_formula.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'5.hq'
    time ${HYPERQB} ${SMV} ${SMV} ${HQ} 20 -hopt -find


CASE="Case 5.2:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'5_planning/robotic_robustness_400.smv'
    HQ=${CASEFOLDER}'5_planning/robotic_robustness_formula.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'5.hq'
    time ${HYPERQB} ${SMV} ${SMV} ${HQ} 40 -hopt -find

CASE="Case 6.1:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'6_mutation/mutation_testing.smv'
    HQ=${CASEFOLDER}'6_mutation/mutation_testing.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'6.hq'
    time ${HYPERQB} ${SMV} ${SMV} ${HQ} 10 -hopt -find


### NEW ADDED EXPERIMENTS ###
CASE="Case 7.1:" 
    echo ${CASE}
    SMV1=${CASEFOLDER}'7_coterm/coterm1.smv'
    SMV2=${CASEFOLDER}'7_coterm/coterm2.smv'
    HQ=${CASEFOLDER}'7_coterm/coterm.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'7.hq'
    time ${HYPERQB} ${SMV1} ${SMV2} ${HQ} 102 -hopt -bughunt


CASE="Case 8.1:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'8_deniability/den_small.smv'
    HQ=${CASEFOLDER}'8_deniability/den_f1.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'8.hq'
    time ${HYPERQB} ${SMV} ${SMV} ${SMV} ${HQ} 10 -hopt -debug


CASE="Case 9.1:"
    echo ${CASE}
    SMV=${CASEFOLDER}'9_buffer/unscheduled_buffer.smv'
    HQ=${CASEFOLDER}'9_buffer/classic_OD.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'9.1.hq' 
    time ${HYPERQB} ${SMV} ${SMV} ${HQ} 10 -pes -find

CASE="Case 9.2:"
    echo ${CASE}
    SMV=${CASEFOLDER}'9_buffer/scheduled_buffer.smv'
    HQ=${CASEFOLDER}'9_buffer/intrans_OD.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'9.2.hq' 
    time ${HYPERQB} ${SMV} ${SMV} ${HQ} 10 -pes -find

CASE="Case 9.3:"
    echo ${CASE}
    SMV=${CASEFOLDER}'9_buffer/scheduled_buffer.smv'
    HQ=${CASEFOLDER}'9_buffer/intrans_GMNI.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'9.2.hq' 
    time ${HYPERQB} ${SMV} ${SMV} ${HQ} 10 -pes -find

# Case 10: NIexp
CASE="Case 10.1:"
    echo ${CASE}
    SMV=${CASEFOLDER}'10_NIexp/ni_example.smv'
    HQ=${CASEFOLDER}'10_NIexp/tini.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'10.1.hq'
    time ${HYPERQB} ${SMV} ${SMV} ${HQ} 10 -hopt -find

CASE="Case 10.2:"
    echo ${CASE}
    SMV=${CASEFOLDER}'10_NIexp/ni_example.smv'
    HQ=${CASEFOLDER}'10_NIexp/tsni.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'10.2.hq'
    time ${HYPERQB} ${SMV} ${SMV} ${HQ} 10 -hopt -find

# Case 11: ksafety
CASE="Case 11:"
    echo ${CASE}
    SMV=${CASEFOLDER}'11_ksafety/doubleSquare.smv'
    HQ=${CASEFOLDER}'11_ksafety/doubleSquare.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'11.hq'
    time ${HYPERQB} ${SMV} ${SMV} ${HQ} 64 -hopt -find

# Case 12: Mappying Synthesis
CASE="Case 12.1:"
    echo ${CASE}
    SMV_A=${CASEFOLDER}'12_mapsynth/msynth_MA.smv'
    SMV_B=${CASEFOLDER}'12_mapsynth/msynth_MB.smv'
    SMV_M=${CASEFOLDER}'12_mapsynth/msynth_MM.smv'
    HQ=${CASEFOLDER}'12_mapsynth/msynth.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'12.1.hq'
    time ${HYPERQB} ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B} ${HQ} 5 -hopt -find

CASE="Case 12.2:"
    echo ${CASE}
    SMV_A=${CASEFOLDER}'12_mapsynth/msynth2_MA.smv'
    SMV_B=${CASEFOLDER}'12_mapsynth/msynth2_MB.smv'
    SMV_M=${CASEFOLDER}'12_mapsynth/msynth2_MM.smv'
    HQ=${CASEFOLDER}'12_mapsynth/msynth2.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'12.2.hq'
    time ${HYPERQB} ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B} ${HQ} 5 -hopt -find

# Case 13: TeamLTL
CASE="Case 13.1:"
    echo ${CASE}
    SMV=${CASEFOLDER}'13_teamltl/team.smv'
    HQ=${CASEFOLDER}'13_teamltl/team.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'13.1.hq'
    time ${HYPERQB} ${SMV} ${SMV} ${SMV} ${HQ} 10 -pes -find
    echo ""

CASE="Case 13.2:"
    echo ${CASE}
    SMV=${CASEFOLDER}'13_teamltl/team2.smv'
    HQ=${CASEFOLDER}'13_teamltl/team.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'13.2.hq'
    time ${HYPERQB} ${SMV} ${SMV} ${SMV} ${HQ} 20 -pes -find

# case 14.1: input non-determinism (overhead: inclusion check)
CASE="Case 14.1:"
    echo ${CASE}
    SMV=${CASEFOLDER}'14_ndet/NI_v2.smv'
    HQ=${CASEFOLDER}'14_ndet/NI.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'14.hq'
    time ${HYPERQB} ${SMV} ${SMV} ${HQ} 5 -pes -find


# case 14.2: transition non-determinism (overhead: inclusion check)
CASE="Case 14.2:"
    echo ${CASE}
    SMV=${CASEFOLDER}'14_ndet/NI_v3.smv'
    HQ=${CASEFOLDER}'14_ndet/NI.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'14.hq'
    time ${HYPERQB} ${SMV} ${SMV} ${HQ} 5 -pes -find


