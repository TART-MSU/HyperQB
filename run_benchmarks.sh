#!/bin/bash
TIMEFORMAT="TOTAL: %Rs"
# HyperQB driver
HYPERQB="./hyperqb.sh"
# AH and AHQ bins
AUTOHYPER='/Users/tzuhan/install/autohyper/app/AutoHyper'
AUTOHYPERQ='/Users/tzuhan/install/autohyperq/app/AutoHyperQ'
# Timeout
TIMEOUT="timeout 1200s"
echo ${TIMEOUT}

CASEFOLDER="benchmarks/"
AHFORMULAS="AH_formulas/"

HQ="FALSE"
COMPAH="FALSE"
COMPAHQ="FALSE"
RUNALL="FALSE"

if (echo $* | grep -e "-AH" -q) then 
    COMPAH="TRUE" 
fi
if (echo $* | grep -e "-QAH" -q) then 
    COMPAHQ="TRUE"
fi


### TACAS21 EXPERIMENTS ###

if ((echo $* | grep -e "0.1" -q) || (echo $* | grep -e "-all" -q)) then
CASE="Case 0.1:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'1_bakery/bakery_3procs.smv'
    HQ=${CASEFOLDER}'1_bakery/bakery_phi_S1_3proc.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'0.1.hq'
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} 7 -pes -find
    echo ""
    if (${COMPAH} -eq "TRUE") then
    echo "------(AutoHyper Starts)------"
    echo "formula: " ${HQAUTO}
    ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} 
    fi
    if (${COMPAHQ} -eq "TRUE") then
    echo "------(AutoHyperQ Starts)------"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO} 
    fi
fi
if ((echo $* | grep -e "0.2" -q) || (echo $* | grep -e "-all" -q)) then
CASE="Case 0.2:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'1_bakery/bakery_3procs.smv'
    HQ=${CASEFOLDER}'1_bakery/bakery_phi_S2_3proc.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'0.2.hq'
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} 12 -pes -find
    echo ""
    if (${COMPAH} -eq "TRUE") then 
    echo "------(AutoHyper Starts)------"
    echo "formula: " ${HQAUTO}
    ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} 
    fi
    if (${COMPAHQ} -eq "TRUE") then
    echo "------(AutoHyperQ Starts)------"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO} --witness
    fi
fi
if ((echo $* | grep -e "0.3" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 0.3:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'1_bakery/bakery_3procs.smv'
    HQ=${CASEFOLDER}'1_bakery/bakery_phi_S3_3proc.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'0.3.hq'
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} 20 -pes -find
    echo ""
    if (${COMPAH} -eq "TRUE") then 
    echo "------(AutoHyper Starts)------"
    echo "formula: " ${HQAUTO}
    ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} 
    fi
    if (${COMPAHQ} -eq "TRUE") then
    echo "------(AutoHyperQ Starts)------"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO} --witness
    fi
fi
if ((echo $* | grep -e "1.1" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 1.1:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'1_bakery/bakery_3procs.smv'
    HQ=${CASEFOLDER}'1_bakery/bakery_phi_sym1_3proc.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'1.1.hq'
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} 10 -pes -bughunt
    echo ""
    if (${COMPAH} -eq "TRUE") then
    echo "------(AutoHyper Starts)------"
    echo "formula: " ${HQAUTO}
    ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} 
    fi
    if (${COMPAHQ} -eq "TRUE") then
    echo "------(AutoHyperQ Starts)------"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO} 
    fi
fi
if ((echo $* | grep -e "1.2" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 1.2:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'1_bakery/bakery_3procs.smv'
    HQ=${CASEFOLDER}'1_bakery/bakery_phi_sym2_3proc.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'1.2.hq'
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} 10 -pes -bughunt
    echo ""
    if (${COMPAH} -eq "TRUE") then
    echo "------(AutoHyper Starts)------"
    echo "formula: " ${HQAUTO}
    ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} 
    fi
    if (${COMPAHQ} -eq "TRUE") then
    echo "------(AutoHyperQ Starts)------"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO} 
    fi
fi
if ((echo $* | grep -e "1.3" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 1.3:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'1_bakery/bakery_5procs.smv'
    HQ=${CASEFOLDER}'1_bakery/bakery_phi_sym1_5proc.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'1.3.hq'
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} 10 -pes -bughunt
    echo ""
    if (${COMPAH} -eq "TRUE") then
    echo "------(AutoHyper Starts)------"
    echo "formula: " ${HQAUTO}
    ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} 
    fi
    if (${COMPAHQ} -eq "TRUE") then
    echo "------(AutoHyperQ Starts)------"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO} 
    fi
fi
if ((echo $* | grep -e "1.4" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 1.4:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'1_bakery/bakery_5procs.smv'
    HQ=${CASEFOLDER}'1_bakery/bakery_phi_sym2_5proc.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'1.4.hq'
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} 10 -pes -bughunt
    echo ""
    if (${COMPAH} -eq "TRUE") then
    echo "------(AutoHyper Starts)------"
    echo "formula: " ${HQAUTO}
    ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} 
    fi
    if (${COMPAHQ} -eq "TRUE") then
    echo "------(AutoHyperQ Starts)------"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO} 
    fi
fi
if ((echo $* | grep -e "2.1" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 2.1:" 
    echo ${CASE}
#     SMV1=${CASEFOLDER}'2_snark/snark1_M1_concurrent.smv'
#     SMV2=${CASEFOLDER}'2_snark/snark1_M2_sequential.smv'
#     HQ=${CASEFOLDER}'2_snark/snark1.hq'
#     HQAUTO=${CASEFOLDER}${AHFORMULAS}'2.1.hq'
#     ${TIMEOUT} time ${HYPERQB} ${SMV1} ${SMV2} ${HQ} 10 -pes -find
#     echo ""
#     if (${COMPAH} -eq "TRUE") then
#     echo "------(AutoHyper Starts)------"
#     ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV1} ${SMV2} ${HQAUTO} 
#     fi
#     if (${COMPAHQ} -eq "TRUE") then
#     echo "------(AutoHyperQ Starts)------"
#     ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV1} ${SMV2} ${HQAUTO} 
#     fi
fi
if ((echo $* | grep -e "3.1" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 3.1:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'3_ni/NI_incorrect.smv'
    HQ=${CASEFOLDER}'3_ni/NI_formula.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'3.hq'
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} 57 -hpes -bughunt
    echo ""
    if (${COMPAH} -eq "TRUE") then
    echo "------(AutoHyper Starts)------"
    echo "formula: " ${HQAUTO}
    ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} 
    fi
    if (${COMPAHQ} -eq "TRUE") then
    echo "------(AutoHyperQ Starts)------"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO} 
    fi
fi
if ((echo $* | grep -e "3.2" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 3.2:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'3_ni/NI_correct.smv'
    HQ=${CASEFOLDER}'3_ni/NI_formula.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'3.hq'
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} 57 -hopt -bughunt
    echo ""
    if (${COMPAH} -eq "TRUE") then
    echo "------(AutoHyper Starts)------"
    echo "formula: " ${HQAUTO}
    ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} 
    fi
    if (${COMPAHQ} -eq "TRUE") then
    echo "------(AutoHyperQ Starts)------"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO} 
    fi
fi
if ((echo $* | grep -e "4.1" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 4.1:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'4_nrp/NRP_incorrect.smv'
    HQ=${CASEFOLDER}'4_nrp/NRP_formula.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'4.hq'
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} 15 -hpes -find
    echo ""
    if (${COMPAH} -eq "TRUE") then
    echo "------(AutoHyper Starts)------"
    echo "formula: " ${HQAUTO}
    ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} 
    fi
    if (${COMPAHQ} -eq "TRUE") then
    echo "------(AutoHyperQ Starts)------"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO} 
    fi
fi
if ((echo $* | grep -e "4.2" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 4.2:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'4_nrp/NRP_correct.smv'
    HQ=${CASEFOLDER}'4_nrp/NRP_formula.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'4.hq'
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} 15 -hopt -find
    echo ""
    if (${COMPAH} -eq "TRUE") then
    echo "------(AutoHyper Starts)------"
    echo "formula: " ${HQAUTO}
    ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} 
    fi
    if (${COMPAHQ} -eq "TRUE") then
    echo "------(AutoHyperQ Starts)------"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO} 
    fi
fi
if ((echo $* | grep -e "5.1" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 5.1:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'5_planning/robotic_robustness_100.smv'
    HQ=${CASEFOLDER}'5_planning/robotic_robustness_formula.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'5.hq'
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} 20 -hopt -find
    echo ""
    if (${COMPAH} -eq "TRUE") then
    echo "------(AutoHyper Starts)------"
    echo "formula: " ${HQAUTO}
    ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} 
    fi
    if (${COMPAHQ} -eq "TRUE") then
    echo "------(AutoHyperQ Starts)------"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO} 
    fi
fi
if ((echo $* | grep -e "5.2" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 5.2:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'5_planning/robotic_robustness_400.smv'
    HQ=${CASEFOLDER}'5_planning/robotic_robustness_formula.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'5.hq'
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} 40 -hopt -find
    echo ""
    if (${COMPAH} -eq "TRUE") then
    echo "------(AutoHyper Starts)------"
    echo "formula: " ${HQAUTO}
    ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} 
    fi
    if (${COMPAHQ} -eq "TRUE") then
    echo "------(AutoHyperQ Starts)------"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO} 
    fi
fi
if ((echo $* | grep -e "5.3" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 5.3:" 
    echo ${CASE}
    # SMV=${CASEFOLDER}'5_planning/robotic_robustness_1600.smv'
    # HQ=${CASEFOLDER}'5_planning/robotic_robustness_formula.hq'
    # HQAUTO=${CASEFOLDER}${AHFORMULAS}'5.hq'
    # ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} 80 -hopt -find
    # echo ""
    # if (${COMPAH} -eq "TRUE") then
    # echo "------(AutoHyper Starts)------"
    # echo "formula: " ${HQAUTO}
    # ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} 
    # fi
    # if (${COMPAHQ} -eq "TRUE") then
    # echo "------(AutoHyperQ Starts)------"
    # ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO} 
    # fi
fi
if ((echo $* | grep -e "5.4" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 5.4:" 
    echo ${CASE}
    # SMV=${CASEFOLDER}'5_planning/robotic_robustness_3600.smv'
    # HQ=${CASEFOLDER}'5_planning/robotic_robustness_formula.hq'
    # HQAUTO=${CASEFOLDER}${AHFORMULAS}'5.hq'
    # ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} 120 -hopt -find
    # echo ""
    # if (${COMPAH} -eq "TRUE") then
    # echo "------(AutoHyper Starts)------"
    # echo "formula: " ${HQAUTO}
    # ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} 
    # fi
    # if (${COMPAHQ} -eq "TRUE") then
    # echo "------(AutoHyperQ Starts)------"
    # ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO} 
    # fi
fi
if ((echo $* | grep -e "6.1" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 6.1:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'6_mutation/mutation_testing.smv'
    HQ=${CASEFOLDER}'6_mutation/mutation_testing.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'6.hq'
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} 10 -hopt -find
    echo ""
    if (${COMPAH} -eq "TRUE") then
    echo "------(AutoHyper Starts)------"
    echo "formula: " ${HQAUTO}
    ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} 
    fi
    if (${COMPAHQ} -eq "TRUE") then
    echo "------(AutoHyperQ Starts)------"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO} 
    fi
fi

### NEW ADDED EXPERIMENTS ###
if ((echo $* | grep -e "7.1" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 7.1:" 
    echo ${CASE}
    SMV1=${CASEFOLDER}'7_coterm/coterm1.smv'
    SMV2=${CASEFOLDER}'7_coterm/coterm2.smv'
    HQ=${CASEFOLDER}'7_coterm/coterm.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'7.hq'
    ${TIMEOUT} time ${HYPERQB} ${SMV1} ${SMV2} ${HQ} 102 -hopt -bughunt
    echo ""
    if (${COMPAH} -eq "TRUE") then
    echo "------(AutoHyper Starts)------"
    echo "formula: " ${HQAUTO}
    ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV1} ${SMV2} ${HQAUTO} 
    fi
    if (${COMPAHQ} -eq "TRUE") then
    echo "------(AutoHyperQ Starts)------"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV1} ${SMV2} ${HQAUTO} 
    fi
fi
if ((echo $* | grep -e "8.1" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 8.1:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'8_deniability/den_small.smv'
    HQ=${CASEFOLDER}'8_deniability/den_f1.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'8.hq'
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${SMV} ${HQ} 10 -hopt -debug
    echo ""
    if (${COMPAH} -eq "TRUE") then
    echo "------(AutoHyper Starts)------"
    echo "formula: " ${HQAUTO}
    ${TIMEOUT} ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} 
    fi
    if (${COMPAHQ} -eq "TRUE") then
    echo "------(AutoHyperQ Starts)------"
    ${TIMEOUT} ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO} 
    fi
fi
if ((echo $* | grep -e "8.2" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 8.2:"
    echo ${CASE}
    # SMV=${CASEFOLDER}'8_deniability/den.smv'
    # HQ=${CASEFOLDER}'8_deniability/den_f1.hq'
    # HQAUTO=${CASEFOLDER}${AHFORMULAS}'8.hq' 
    # ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${SMV} ${HQ} 20 -hopt -debug
    # echo ""
    # if (${COMPAH} -eq "TRUE") then
    # echo "------(AutoHyper Starts)------"
    # echo "formula: " ${HQAUTO}
    # ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} 
    # fi
    # if (${COMPAHQ} -eq "TRUE") then
    # echo "------(AutoHyperQ Starts)-----"
    # ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO} 
    # fi
fi
if ((echo $* | grep -e "9.1" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 9.1:"
    echo ${CASE}
    SMV=${CASEFOLDER}'9_buffer/unscheduled_buffer.smv'
    HQ=${CASEFOLDER}'9_buffer/classic_OD.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'9.1.hq' 
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} 10 -pes -find
    echo ""
    if (${COMPAH} -eq "TRUE") then
    echo "------(AutoHyper Starts)------"
    echo "formula: " ${HQAUTO}
    ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} 
    fi
    if (${COMPAHQ} -eq "TRUE") then
    echo "------(AutoHyperQ Starts)-----"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO} 
    fi
fi
if ((echo $* | grep -e "9.2" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 9.2:"
    echo ${CASE}
    SMV=${CASEFOLDER}'9_buffer/scheduled_buffer.smv'
    HQ=${CASEFOLDER}'9_buffer/intrans_OD.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'9.2.hq' 
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} 10 -pes -find
    echo ""
    if (${COMPAH} -eq "TRUE") then
    echo "------(AutoHyper Starts)------"
    echo "formula: " ${HQAUTO}
    ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} 
    fi
    if (${COMPAHQ} -eq "TRUE") then
    echo "------(AutoHyperQ Starts)-----"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO} 
    fi
fi
if ((echo $* | grep -e "9.3" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 9.3:"
    echo ${CASE}
    SMV=${CASEFOLDER}'9_buffer/scheduled_buffer.smv'
    HQ=${CASEFOLDER}'9_buffer/intrans_GMNI.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'9.2.hq' 
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} 10 -pes -find
    echo ""
    if (${COMPAH} -eq "TRUE") then 
    echo "------(AutoHyper Starts)------"
    echo "formula: " ${HQAUTO}
    ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} 
    fi
    if (${COMPAHQ} -eq "TRUE") then
    echo "------(AutoHyperQ Starts)-----"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO} 
    fi
fi
if ((echo $* | grep -e "10.1" -q)|| (echo $* | grep -e "-all" -q)) then 
# Case 10: NIexp
CASE="Case 10.1:"
    echo ${CASE}
    SMV=${CASEFOLDER}'10_NIexp/ni_example.smv'
    HQ=${CASEFOLDER}'10_NIexp/tini.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'10.1.hq'
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} 10 -hopt -find
    echo ""
    if (${COMPAH} -eq "TRUE") then
    echo "------(AutoHyper Starts)------"
    echo "formula: " ${HQAUTO}
    ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO}
    fi
    if (${COMPAHQ} -eq "TRUE") then
    echo "------(AutoHyperQ Starts)-----"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO}
    fi
fi
if ((echo $* | grep -e "10.2" -q)|| (echo $* | grep -e "-all" -q)) then 
CASE="Case 10.2:"
    echo ${CASE}
    SMV=${CASEFOLDER}'10_NIexp/ni_example.smv'
    HQ=${CASEFOLDER}'10_NIexp/tsni.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'10.2.hq'
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} 10 -hopt -find
    echo ""
    if (${COMPAH} -eq "TRUE") then
    echo "------(AutoHyper Starts)------"
    echo "formula: " ${HQAUTO}
    ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO}
    fi
    if (${COMPAHQ} -eq "TRUE") then
    echo "------(AutoHyperQ Starts)-----"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO}
    fi
fi
if ((echo $* | grep -e "11.1" -q)|| (echo $* | grep -e "-all" -q)) then 
# Case 11: ksafety
CASE="Case 11:"
    echo ${CASE}
    SMV=${CASEFOLDER}'11_ksafety/doubleSquare.smv'
    HQ=${CASEFOLDER}'11_ksafety/doubleSquare.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'11.hq'
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} 64 -hopt -find
    echo ""
    if (${COMPAH} -eq "TRUE") then
    echo "------(AutoHyper Starts)------"
    echo "formula: " ${HQAUTO}
    ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO}
    fi
    if (${COMPAHQ} -eq "TRUE") then
    echo "------(AutoHyperQ Starts)-----"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO}
    fi

fi
if ((echo $* | grep -e "12.1" -q)|| (echo $* | grep -e "-all" -q)) then 
# Case 12: Mappying Synthesis
CASE="Case 12.1:"
    echo ${CASE}
    SMV_A=${CASEFOLDER}'12_mapsynth/msynth_MA.smv'
    SMV_B=${CASEFOLDER}'12_mapsynth/msynth_MB.smv'
    SMV_M=${CASEFOLDER}'12_mapsynth/msynth_MM.smv'
    HQ=${CASEFOLDER}'12_mapsynth/msynth.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'12.1.hq'
    ${TIMEOUT} time ${HYPERQB} ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B} ${HQ} 5 -hopt -find
    echo ""
    if (${COMPAH} -eq "TRUE") then
    echo "------(AutoHyper Starts)------"
    echo "formula: " ${HQAUTO}
    ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B}  ${HQAUTO}
    fi
    if (${COMPAHQ} -eq "TRUE") then
    echo "------(AutoHyperQ Starts)-----"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B}  ${HQAUTO}
    fi

fi
if ((echo $* | grep -e "12.2" -q)|| (echo $* | grep -e "-all" -q)) then 
CASE="Case 12.2:"
    echo ${CASE}
    SMV_A=${CASEFOLDER}'12_mapsynth/msynth2_MA.smv'
    SMV_B=${CASEFOLDER}'12_mapsynth/msynth2_MB.smv'
    SMV_M=${CASEFOLDER}'12_mapsynth/msynth2_MM.smv'
    HQ=${CASEFOLDER}'12_mapsynth/msynth2.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'12.2.hq'
    ${TIMEOUT} time ${HYPERQB} ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B} ${HQ} 5 -hopt -find
    echo ""
    if (${COMPAH} -eq "TRUE") then
    echo "------(AutoHyper Starts)------"
    echo "formula: " ${HQAUTO}
    ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B}  ${HQAUTO}
    fi
    if (${COMPAHQ} -eq "TRUE") then
    echo "------(AutoHyperQ Starts)-----"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B}  ${HQAUTO}
    fi

fi
if ((echo $* | grep -e "13.1" -q)|| (echo $* | grep -e "-all" -q)) then 
# Case 13: TeamLTL
CASE="Case 13.1:"
    echo ${CASE}
    SMV=${CASEFOLDER}'13_teamltl/team.smv'
    HQ=${CASEFOLDER}'13_teamltl/team.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'13.1.hq'
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${SMV} ${HQ} 10 -pes -find
    echo ""
    if (${COMPAH} -eq "TRUE") then 
    echo "------(AutoHyper Starts)------"
    echo "formula: " ${HQAUTO}
    ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO}
    fi
    if (${COMPAHQ} -eq "TRUE") then
    echo "------(AutoHyperQ Starts)-----"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO}
    fi

fi
if ((echo $* | grep -e "13.2" -q)|| (echo $* | grep -e "-all" -q)) then 
CASE="Case 13.2:"
    echo ${CASE}
    SMV=${CASEFOLDER}'13_teamltl/team2.smv'
    HQ=${CASEFOLDER}'13_teamltl/team.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'13.2.hq'
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${SMV} ${HQ} 20 -pes -find
    echo ""
    if (${COMPAH} -eq "TRUE") then
    echo "------(AutoHyper Starts)------"
    echo "formula: " ${HQAUTO}
    ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO}
    fi
    if (${COMPAHQ} -eq "TRUE") then
    echo "------(AutoHyperQ Starts)-----"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO}
    fi

fi
if ((echo $* | grep -e "14.1" -q)|| (echo $* | grep -e "-all" -q)) then 
# case 14.1: input non-determinism (overhead: inclusion check)
CASE="Case 14.1:"
    echo ${CASE}
    SMV=${CASEFOLDER}'14_ndet/NI_v2.smv'
    HQ=${CASEFOLDER}'14_ndet/NI.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'14.hq'
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} 5 -pes -find
    echo ""
    if (${COMPAH} -eq "TRUE") then
    echo "------(AutoHyper Starts)------"
    echo "formula: " ${HQAUTO}
    ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO}
    fi
    if (${COMPAHQ} -eq "TRUE") then
    echo "------(AutoHyperQ Starts)-----"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO}
    fi

fi
if ((echo $* | grep -e "14.2" -q)|| (echo $* | grep -e "-all" -q)) then 
# case 14.2: transition non-determinism (overhead: inclusion check)
CASE="Case 14.2:"
    echo ${CASE}
    SMV=${CASEFOLDER}'14_ndet/NI_v3.smv'
    HQ=${CASEFOLDER}'14_ndet/NI.hq'
    HQAUTO=${CASEFOLDER}${AHFORMULAS}'14.hq'
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} 5 -pes -find
    echo ""
    if (${COMPAH} -eq "TRUE") then
    echo "------(AutoHyper Starts)------"
    echo "formula: " ${HQAUTO}
    ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO}
    fi
    if (${COMPAHQ} -eq "TRUE") then
    echo "------(AutoHyperQ Starts)-----"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO}
    fi
fi
