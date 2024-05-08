#!/bin/bash
TIMEFORMAT="TOTAL TIME: %Rs"
### tool parameters ###
HYPERQB="./hyperqb.sh"
OLDHYPERQB="./hyperqb_old.sh"
AUTOHYPER='/Users/tzuhan/install/autohyper/app/AutoHyper'
AUTOHYPERQ='/Users/tzuhan/install/autohyperq/app/AutoHyperQ'
### path setting ###
BENCHMARK="benchmarks/"
### timeout setting ### 
TIMEOUT="timeout 10s" 
echo ${TIMEOUT}
### argument setting ###
HQB=false
OLDHQB=false
AH=false
AHQ=false
ALLCASES=false
ALLTOOLS=false
COMPARE=false

for i in "$@" ; do
    if [[ $i == "-HQB" ]] ; then
        HQB=true
    fi
    if [[ $i == "-OLDHQB" ]] ; then
        OLDHQB=true
    fi
    if [[ $i == "-AH" ]] ; then
        AH=true
    fi
    if [[ $i == "-AHQ" ]] ; then
        AHQ=true
    fi
    if [[ $i == "-allcases" ]] ; then
        ALLCASES=true
    fi
done


function run_case {
    echo "running benchmarks: $7"
    if (${HQB} == "true") || (${ALLTOOLS} -eq "true")  then
        echo "[[ HyperQB ]]"
        time ${HYPERQB} ${1} ${1} ${2} ${3} ${4} ${5} 
    fi
    sleep 1 # buffering for file R/W

    if (${OLDHQB} == "true") || (${ALLTOOLS} -eq "true") then
        echo ""
        echo "[[ OldHyperQB ]]"
        ${TIMEOUT} time ${OLDHYPERQB} $1 $1 $2 $3 $4 $5
    fi

    if (${AH} == "true") || (${ALLTOOLS} -eq "true") then
        echo ""
        echo "[[ AutoHyper ]]"
        ${TIMEOUT} time ${AUTOHYPER} --nusmv $1 $6 --debug
    fi

    if (${AHQ} == "true") || (${ALLTOOLS} -eq "true") then
        echo ""
        echo "[[ AutoHyperQ ]]"
        ${TIMEOUT} time ${AUTOHYPERQ} --nusmv $1 $6 
    fi
}


################
### 0.1--6.1 ###
################
if (echo $* | grep -e "0.1" -q) || (${ALLCASES} -eq "true") then 
    CASE="Case-#0.1--Bakery"
    SMV='benchmarks/1_bakery/3procs.smv'
    HQ='benchmarks/1_bakery/S1_3procs.hq'
    HQAUTO='benchmarks/1_bakery/AH/0.1.hq'
    K=10
    SEM='-pes'
    MODE='-find'
    echo ""
    run_case ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO} ${CASE}
fi

if [[ "$1" == "0.2" || $ALLCASES ]]; then
    CASE="Case-#0.2--Bakery"
    SMV='benchmarks/1_bakery/3procs.smv'
    HQ='benchmarks/1_bakery/S2_3procs.hq'
    HQAUTO='benchmarks/1_bakery/AH/0.2.hq'
    K=10
    SEM='-pes'
    MODE='-find'
    run_case ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO} ${CASE}
fi


if ((echo $* | grep -e "0.3" -q) || $ALLCASES) then 
CASE="Case 0.3:" 
    SMV='benchmarks/1_bakery/3procs.smv'
    HQ='benchmarks/1_bakery/S3_3procs.hq'
    HQAUTO='benchmarks/1_bakery/AH/0.3.hq'
    K=10
    SEM='-pes'
    MODE='-find'
    # run_case ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

if ((echo $* | grep -e "1.1" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 1.1:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'1_bakery/3procs.smv'
    HQ=${CASEFOLDER}'1_bakery/bakery_phi_sym1_3proc.hq'
    K=
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} ${K} -pes -bughunt
fi

if ((echo $* | grep -e "1.2" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 1.2:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'1_bakery/3procs.smv'
    HQ=${CASEFOLDER}'1_bakery/bakery_phi_sym2_3proc.hq'
    K=
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} ${K} -pes -bughunt
fi

if ((echo $* | grep -e "1.3" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 1.3:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'1_bakery/5procs.smv'
    HQ=${CASEFOLDER}'1_bakery/bakery_phi_sym1_5proc.hq'
    K=
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} ${K} -pes -bughunt
fi

if ((echo $* | grep -e "1.4" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 1.4:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'1_bakery/5procs.smv'
    HQ=${CASEFOLDER}'1_bakery/bakery_phi_sym2_5proc.hq'
    K=
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} ${K} -pes -bughunt
fi

if ((echo $* | grep -e "2.1" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 2.1:" 
    echo ${CASE}
    SMV1=${CASEFOLDER}'2_snark/snark1_M1_concurrent.smv'
    SMV2=${CASEFOLDER}'2_snark/snark1_M2_sequential.smv'
    HQ=${CASEFOLDER}'2_snark/snark1.hq'
    K=
    ${TIMEOUT} time ${HYPERQB} ${SMV1} ${SMV2} ${HQ} ${K} -pes -find
fi 

if ((echo $* | grep -e "2.2" -q) || (echo $* | grep -e ${K} -all -q)) then 
CASE="Case 2.2:" 
    echo "Case 2.2 is huge, is recommended to test separately"
    echo "please go uncommand section "CASE2.2" in run_bencchmark.sh"
    # echo ${CASE}
    # SMV1=${CASEFOLDER}'2_snark/snark2_M1_concurrent.smv'
    # SMV2=${CASEFOLDER}'2_snark/snark2_M2_sequential.smv'
    # HQ=${CASEFOLDER}'2_snark/snark2.hq'
    K=
    # ${TIMEOUT} time ${HYPERQB} ${SMV1} ${SMV2} ${HQ} ${K} -pes -find
fi


if ((echo $* | grep -e "3.1" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 3.1:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'3_ni/NI_incorrect.smv'
    HQ=${CASEFOLDER}'3_ni/NI_formula.hq'
    K=
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} ${K} -hpes -bughunt
fi

if ((echo $* | grep -e "3.2" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 3.2:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'3_ni/NI_correct.smv'
    HQ=${CASEFOLDER}'3_ni/NI_formula.hq'
    K=
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} ${K} -hopt -bughunt
fi

if ((echo $* | grep -e "4.1" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 4.1:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'4_nrp/NRP_incorrect.smv'
    HQ=${CASEFOLDER}'4_nrp/NRP_formula.hq'
    K=
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} ${K} -hpes -bughunt
fi

if ((echo $* | grep -e "4.2" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 4.2:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'4_nrp/NRP_correct.smv'
    HQ=${CASEFOLDER}'4_nrp/NRP_formula.hq'
    K=
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} ${K} -hpes -bughunt
fi

if ((echo $* | grep -e "5.1" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 5.1:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'5_planning/robotic_robustness_100.smv'
    HQ=${CASEFOLDER}'5_planning/robotic_robustness_formula.hq'
    K=
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} ${K} -hopt -find
fi

if ((echo $* | grep -e "5.2" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 5.2:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'5_planning/robotic_robustness_400.smv'
    HQ=${CASEFOLDER}'5_planning/robotic_robustness_formula.hq'
    K=
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} ${K} -hopt -find
fi

if ((echo $* | grep -e "5.3" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 5.3:" 
    echo "Case5.3 is huge, is recommended to test separately"
    echo "please go uncommand section "CASE5.3" in run_bencchmark.sh"
    # echo ${CASE}
    # SMV=${CASEFOLDER}'5_planning/robotic_robustness_1600.smv'
    # HQ=${CASEFOLDER}'5_planning/robotic_robustness_formula.hq'
    K=
    # ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} ${K} -hopt -find
fi

if ((echo $* | grep -e "5.4" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 5.4:" 
    echo "Case 5.4 is huge, is recommended to test separately"
    echo "please go uncommand section "CASE5.4" in run_bencchmark.sh"
    # echo ${CASE}
    # SMV=${CASEFOLDER}'5_planning/robotic_robustness_3600.smv'
    # HQ=${CASEFOLDER}'5_planning/robotic_robustness_formula.hq'
    K=
    # ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} 1${K} -hopt -find
fi

if ((echo $* | grep -e "6.1" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 6.1:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'6_mutation/mutation_testing.smv'
    HQ=${CASEFOLDER}'6_mutation/mutation_testing.hq'
    K=
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} ${K} -hopt -find
fi


### NEW ADDED EXPERIMENTS ###
if ((echo $* | grep -e "7.1" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 7.1:" 
    echo ${CASE}
    SMV1=${CASEFOLDER}'7_coterm/coterm1.smv'
    SMV2=${CASEFOLDER}'7_coterm/coterm2.smv'
    HQ=${CASEFOLDER}'7_coterm/coterm.hq'
    K=
    ${TIMEOUT} time ${HYPERQB} ${SMV1} ${SMV2} ${HQ} 1${K} -hopt -bughunt
fi

if ((echo $* | grep -e "8.1" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 8.1:" 
    echo ${CASE}
    SMV=${CASEFOLDER}'8_deniability/den_small.smv'
    HQ=${CASEFOLDER}'8_deniability/den_f1.hq'
    K=
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${SMV} ${HQ} ${K} -hopt -debug
fi

if ((echo $* | grep -e "8.2" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 8.2:"
    echo ${CASE}
    SMV=${CASEFOLDER}'8_deniability/den.smv'
    HQ=${CASEFOLDER}'8_deniability/den_f1.hq'
    K=
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${SMV} ${HQ} ${K} -hopt -debug
fi

if ((echo $* | grep -e "9.1" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 9.1:"
    echo ${CASE}
    SMV=${CASEFOLDER}'9_buffer/unscheduled_buffer.smv'
    HQ=${CASEFOLDER}'9_buffer/classic_OD.hq'
    K=
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} ${K} -pes -find
fi

if ((echo $* | grep -e "9.2" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 9.2:"
    echo ${CASE}
    SMV=${CASEFOLDER}'9_buffer/scheduled_buffer.smv'
    HQ=${CASEFOLDER}'9_buffer/intrans_OD.hq'
    K=
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} ${K} -pes -find
fi

if ((echo $* | grep -e "9.3" -q) || (echo $* | grep -e "-all" -q)) then 
CASE="Case 9.3:"
    echo ${CASE}
    SMV=${CASEFOLDER}'9_buffer/scheduled_buffer.smv'
    HQ=${CASEFOLDER}'9_buffer/intrans_GMNI.hq'
    K=
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} ${K} -pes -find
fi

if ((echo $* | grep -e "10.1" -q)|| (echo $* | grep -e "-all" -q)) then 
# Case 10: NIexp
CASE="Case 10.1:"
    echo ${CASE}
    SMV=${CASEFOLDER}'10_NIexp/ni_example.smv'
    HQ=${CASEFOLDER}'10_NIexp/tini.hq'
    K=
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} ${K} -hopt -find
fi

if ((echo $* | grep -e "10.2" -q)|| (echo $* | grep -e "-all" -q)) then 
CASE="Case 10.2:"
    echo ${CASE}
    SMV=${CASEFOLDER}'10_NIexp/ni_example.smv'
    HQ=${CASEFOLDER}'10_NIexp/tsni.hq'
    K=
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} ${K} -hopt -find
fi

if ((echo $* | grep -e "11.1" -q)|| (echo $* | grep -e "-all" -q)) then 
# Case 11: ksafety
CASE="Case 11:"
    echo ${CASE}
    SMV=${CASEFOLDER}'11_ksafety/doubleSquare.smv'
    HQ=${CASEFOLDER}'11_ksafety/doubleSquare.hq'
    K=
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} ${K} -hopt -find


fi
if ((echo $* | grep -e "12.1" -q)|| (echo $* | grep -e "-all" -q)) then 
# Case 12: Mappying Synthesis
CASE="Case 12.1:"
    echo ${CASE}
    SMV_A=${CASEFOLDER}'12_mapsynth/msynth_MA.smv'
    SMV_B=${CASEFOLDER}'12_mapsynth/msynth_MB.smv'
    SMV_M=${CASEFOLDER}'12_mapsynth/msynth_MM.smv'
    HQ=${CASEFOLDER}'12_mapsynth/msynth.hq'
    K=
    ${TIMEOUT} time ${HYPERQB} ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B} ${HQ}${K} -hopt -find


fi
if ((echo $* | grep -e "12.2" -q)|| (echo $* | grep -e "-all" -q)) then 
CASE="Case 12.2:"
    echo ${CASE}
    SMV_A=${CASEFOLDER}'12_mapsynth/msynth2_MA.smv'
    SMV_B=${CASEFOLDER}'12_mapsynth/msynth2_MB.smv'
    SMV_M=${CASEFOLDER}'12_mapsynth/msynth2_MM.smv'
    HQ=${CASEFOLDER}'12_mapsynth/msynth2.hq'
    K=
    ${TIMEOUT} time ${HYPERQB} ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B} ${HQ}${K} -hopt -find


fi
if ((echo $* | grep -e "13.1" -q)|| (echo $* | grep -e "-all" -q)) then 
# Case 13: TeamLTL
CASE="Case 13.1:"
    echo ${CASE}
    SMV=${CASEFOLDER}'13_teamltl/team.smv'
    HQ=${CASEFOLDER}'13_teamltl/team.hq'
    K=
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${SMV} ${HQ} ${K} -pes -find


fi
if ((echo $* | grep -e "13.2" -q)|| (echo $* | grep -e "-all" -q)) then 
CASE="Case 13.2:"
    echo ${CASE}
    SMV=${CASEFOLDER}'13_teamltl/team2.smv'
    HQ=${CASEFOLDER}'13_teamltl/team.hq'
    K=
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${SMV} ${HQ} ${K} -pes -find


fi
if ((echo $* | grep -e "14.1" -q)|| (echo $* | grep -e "-all" -q)) then 
# case 14.1: input non-determinism (overhead: inclusion check)
CASE="Case 14.1:"
    echo ${CASE}
    SMV=${CASEFOLDER}'14_ndet/NI_v2.smv'
    HQ=${CASEFOLDER}'14_ndet/NI.hq'
    K=
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ}${K} -pes -find


fi
if ((echo $* | grep -e "14.2" -q)|| (echo $* | grep -e "-all" -q)) then 
# case 14.2: transition non-determinism (overhead: inclusion check)
CASE="Case 14.2:"
    echo ${CASE}
    SMV=${CASEFOLDER}'14_ndet/NI_v3.smv'
    HQ=${CASEFOLDER}'14_ndet/NI.hq'
    K=
    ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ}${K} -pes -find
fi

