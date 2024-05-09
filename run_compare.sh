#!/bin/bash
# TIMEFORMAT=%R
TIMEFORMAT=">>> TOTAL TIME: %Rs <<<"
HYPERQB="./hyperqb.sh"
OLDHYPERQB="./hyperqb_old.sh"

##########################
### HyperQB parameters ###
##########################
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
  BINLOCATION="exec/linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
  BINLOCATION="exec/mac"
else
  echo "sorry, current OS not supported yet :("
  exit 1 
fi
AUTOHYPER="${BINLOCATION}/compare/AH/AutoHyper" 
AUTOHYPERQ="${BINLOCATION}/compare/AHQ/AutoHyperQ"

HQB=false
OLDHQB=false
AH=false
AHQ=false
ALLCASES=false
ALLTOOLS=false
COMPARE=false
### timeout setting ### 
TIMEOUT="3s" 

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
    if [[ $i == "-alltools" ]] ; then
        ALLTOOLS=true
    fi
done

### A function for comparising instnaces on: ###
#   - HyperQB (multi-gate)
#   - previous HyperQB (binary-gate) (binary-gate)
#   - AutoHyper
#   - AutoHyperQ
function compare {
    # echo "running benchmarks: $7"
    if (${HQB} == "true") || (${ALLTOOLS} -eq "true")  then
        echo "[[ HyperQB ]]"
        time timeout ${TIMEOUT} ${HYPERQB} ${1} ${1} ${2} ${3} ${4} ${5} 
    fi
    sleep 1 # buffering for file R/W

    if (${OLDHQB} == "true") || (${ALLTOOLS} -eq "true") then
        echo ""
        echo "[[ OldHyperQB ]]"
        time timeout ${TIMEOUT} ${OLDHYPERQB} ${1} ${1} ${2} ${3} ${4} ${5}
    fi

    if (${AH} == "true") || (${ALLTOOLS} -eq "true") then
        echo ""
        echo "[[ AutoHyper ]]"
        time timeout ${TIMEOUT} ${AUTOHYPER} --nusmv ${1} ${6} --debug
    fi

    if (${AHQ} == "true") || (${ALLTOOLS} -eq "true") then
        echo ""
        echo "[[ AutoHyperQ ]]"
        time timeout ${TIMEOUT} ${AUTOHYPERQ} --nusmv ${1} ${6} 
    fi
}

function compare2 {
    # echo "running benchmarks: $7"
    if (${HQB} == "true") || (${ALLTOOLS} -eq "true")  then
        echo "[[ HyperQB ]]"
        time timeout ${TIMEOUT} ${HYPERQB} ${1} ${2} ${3} ${4} ${5} ${6} 
    fi
    sleep 1 # buffering for file R/W

    if (${OLDHQB} == "true") || (${ALLTOOLS} -eq "true") then
        echo "[[ OldHyperQB ]]"
        time timeout ${TIMEOUT} ${OLDHYPERQB} ${1} ${2} ${3} ${4} ${5} ${6}
    fi

    if (${AH} == "true") || (${ALLTOOLS} -eq "true") then
        echo ""
        echo "[[ AutoHyper ]]"
        time timeout ${TIMEOUT} ${AUTOHYPER} --nusmv ${1} ${7} --debug
    fi

    if (${AHQ} == "true") || (${ALLTOOLS} -eq "true") then
        echo ""
        echo "[[ AutoHyperQ ]]"
        time timeout ${TIMEOUT} ${AUTOHYPERQ} --nusmv ${1} ${7} 
    fi
}

function compare3 {
    # echo "running benchmarks: $7"
    if (${HQB} == "true") || (${ALLTOOLS} -eq "true")  then
        echo "[[ HyperQB ]]"
        time timeout ${TIMEOUT} ${HYPERQB} ${1} ${2} ${3} ${4} ${5} ${6} ${7}
    fi
    sleep 1 # buffering for file R/W

    if (${OLDHQB} == "true") || (${ALLTOOLS} -eq "true") then
        echo ""
        echo "[[ OldHyperQB ]]"
        time timeout ${TIMEOUT} ${OLDHYPERQB} ${1} ${2} ${3} ${4} ${5} ${6} ${7}
    fi

    if (${AH} == "true") || (${ALLTOOLS} -eq "true") then
        echo ""
        echo "[[ AutoHyper ]]"
        $time timeout ${TIMEOUT} ${AUTOHYPER} --nusmv ${1} ${8} --debug
    fi

    if (${AHQ} == "true") || (${ALLTOOLS} -eq "true") then
        echo ""
        echo "[[ AutoHyperQ ]]"
        time timeout ${TIMEOUT} ${AUTOHYPERQ} --nusmv ${1} ${8} 
    fi
}

###################
# 0.1 Bakery3, S1 #
###################
if ((echo $* | grep -e "0.1" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/1_bakery/3procs.smv'
    HQ='benchmarks/1_bakery/S1_3procs.hq'
    HQAUTO='benchmarks/1_bakery/AH/0.1.hq'
    K=10
    SEM='-pes'
    MODE='-find'
    compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

###################
# 0.2 Bakery3, S2 #
###################
if ((echo $* | grep -e "0.2" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/1_bakery/3procs.smv'
    HQ='benchmarks/1_bakery/S2_3procs.hq'
    HQAUTO='benchmarks/1_bakery/AH/0.2.hq'
    K=10
    SEM='-pes'
    MODE='-find'
    compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

###################
# 0.3 Bakery3, S3 #
###################
if ((echo $* | grep -e "0.3" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/1_bakery/3procs.smv'
    HQ='benchmarks/1_bakery/S3_3procs.hq'
    HQAUTO='benchmarks/1_bakery/AH/0.3.hq'
    K=10
    SEM='-pes'
    MODE='-find'
    compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

#####################
# 1.1 Bakery3, sym1 #
#####################
if ((echo $* | grep -e "1.1" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/1_bakery/3procs.smv'
    HQ='benchmarks/1_bakery/sym1_3procs.hq'
    HQAUTO='benchmarks/1_bakery/AH/1.1.hq'
    K=10
    SEM='-pes'
    MODE='-find'
    compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

#####################
# 1.2 Bakery3, sym2 #
#####################
if ((echo $* | grep -e "1.2" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/1_bakery/3procs.smv'
    HQ='benchmarks/1_bakery/sym2_3procs.hq'
    HQAUTO='benchmarks/1_bakery/AH/1.2.hq'
    K=10
    SEM='-pes'
    MODE='-find'
    compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

#####################
# 1.3 Bakery5, sym1 #
#####################
if ((echo $* | grep -e "1.3" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/1_bakery/5procs.smv'
    HQ='benchmarks/1_bakery/sym1_5procs.hq'
    HQAUTO='benchmarks/1_bakery/AH/1.3.hq'
    K=10
    SEM='-pes'
    MODE='-find'
    compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi    

#####################
# 1.4 Bakery5, sym2 #
#####################
if ((echo $* | grep -e "1.4" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/1_bakery/5procs.smv'
    HQ='benchmarks/1_bakery/sym2_5procs.hq'
    HQAUTO='benchmarks/1_bakery/AH/1.4.hq'
    K=10
    SEM='-pes'
    MODE='-find'
    compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

##############
# 2.1 SNARK1 #
##############
if ((echo $* | grep -e "2.1" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV1='benchmarks/2_snark/snark1_M1_concurrent.smv'
    SMV2='benchmarks/2_snark/snark1_M2_sequential.smv'
    HQ='benchmarks/2_snark/snark1.hq'
    HQAUTO='benchmarks/2_snark/AH/2.1_neg.hq'
    K=18
    SEM='-pes'
    MODE='-bughunt'
    compare2 ${SMV1} ${SMV2} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
    # echo "[ HyperQB ]"
    # ${TIMEOUT} time ${HYPERQB} ${SMV1} ${SMV2} ${HQ} ${K} ${SEM} ${MODE} "-NN"
    # sleep 1 # buffering for file R/W
    # echo "[ previous HyperQB (binary-gate) ]"
    # ${TIMEOUT} time ${OLDHYPERQB} ${SMV1} ${SMV2} ${HQ} ${K} ${SEM} ${MODE}
    # echo "[ AutoHyper ]"
    # ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV1} ${SMV2} ${HQAUTO} --debug
    # echo "[ AutoHyperQ ]"
    # ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV1} ${SMV2} ${HQAUTO} 
fi


########################
# 3.1 3T_incorrect, NI #
########################
if ((echo $* | grep -e "3.1" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/3_ni/NI_incorrect.smv'
    HQ='benchmarks/3_ni/NI.hq'
    HQAUTO='benchmarks/3_ni/AH/3.hq'
    K=50
    SEM='-opt'
    MODE='-find'
    compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

######################
# 3.2 3T_correct, NI #
######################
if ((echo $* | grep -e "3.2" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/3_ni/NI_correct.smv'
    HQ='benchmarks/3_ni/NI.hq'
    HQAUTO='benchmarks/3_ni/AH/3.hq'
    K=50
    SEM='-opt'
    MODE='-find'
    compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

###########################
# 4.1 NRP_incorrect, fair #
###########################
if ((echo $* | grep -e "4.1" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/4_nrp/NRP_incorrect.smv'
    HQ='benchmarks/4_nrp/NRP.hq'
    HQAUTO='benchmarks/4_nrp/AH/4.hq'
    K=15
    SEM='-opt'
    MODE='-find'
    compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

#########################
# 4.2 NRP_correct, fair #
#########################
if ((echo $* | grep -e "4.2" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/4_nrp/NRP_correct.smv'
    HQ='benchmarks/4_nrp/NRP.hq'
    HQAUTO='benchmarks/4_nrp/AH/4.hq'
    K=15
    SEM='-opt'
    MODE='-find'
    compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

###################
# 5.1 Planning-rb #
###################
if ((echo $* | grep -e "5.1" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/5_planning/rb_100.smv'
    HQ='benchmarks/5_planning/rb_formula.hq'
    HQAUTO='benchmarks/5_planning/AH/5.hq'
    K=20; SEM='-opt' ; MODE='-find'
    compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

###################
# 5.2 Planning-rb #
###################
if ((echo $* | grep -e "5.2" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/5_planning/rb_400.smv'
    HQ='benchmarks/5_planning/rb_formula.hq'
    HQAUTO='benchmarks/5_planning/AH/5.hq'
    K=40; SEM='-opt' ; MODE='-find'
    compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

###################
# 5.3 Planning-rb #
###################
if ((echo $* | grep -e "5.3" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/5_planning/rb_1600.smv'
    HQ='benchmarks/5_planning/rb_formula.hq'
    HQAUTO='benchmarks/5_planning/AH/5.hq'
    K=80; SEM='-opt' ; MODE='-find'
    compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

###################
# 5.4 Planning-rb #
###################
if ((echo $* | grep -e "5.4" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/5_planning/rb_3600.smv'
    HQ='benchmarks/5_planning/rb_formula.hq'
    HQAUTO='benchmarks/5_planning/AH/5.hq'
    K=120; SEM='-opt' ; MODE='-find'
    compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

##############
# 6.1 Mutant #
#############
if ((echo $* | grep -e "6.1" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/6_mutation/mutation.smv'
    HQ='benchmarks/6_mutation/mutation.hq'
    HQAUTO='benchmarks/6_mutation/AH/6.hq'
    K=10; SEM='-opt' ; MODE='-find'
    compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

######################
# 7.1 2Progs, coterm #
######################
if ((echo $* | grep -e "7.1" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV1='benchmarks/7_2Progs/prog1.smv'
    SMV2='benchmarks/7_2Progs/prog2.smv'
    HQ='benchmarks/7_2Progs/coterm.hq'
    HQAUTO='benchmarks/7_2Progs/AH/7.hq'
    K=102
    SEM='-pes'
    MODE='-find'
    compare2 ${SMV1} ${SMV2} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi    

############################
# 8.1 Wallet1, deniability #
############################
if ((echo $* | grep -e "8.1" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/8_deniability/den_small.smv'
    HQ='benchmarks/8_deniability/den.hq'
    HQAUTO='benchmarks/8_deniability/AH/8.hq'
    K=10
    SEM='-pes'
    MODE='-find'
    echo "[ HyperQB ]"
    compare3 ${SMV} ${SMV} ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

############################
# 8.2 Wallet2, deniability #
############################
if ((echo $* | grep -e "8.2" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/8_deniability/den.smv'
    HQ='benchmarks/8_deniability/den.hq'
    HQAUTO='benchmarks/8_deniability/AH/8.hq'
    K=20
    SEM='-pes'
    MODE='-find'
    compare3 ${SMV} ${SMV} ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

##################
# 9.1 Buffer, OD #
##################
if ((echo $* | grep -e "9.1" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/9_buffer/buffer.smv'
    HQ='benchmarks/9_buffer/OD.hq'
    HQAUTO='benchmarks/9_buffer/AH/9.1.hq'
    K=10; SEM='-opt' ; MODE='-find'
    compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

##############################
# 9.2 Buffer_sched, OD_intra #
##############################
if ((echo $* | grep -e "9.2" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/9_buffer/buffer_sched.smv'
    HQ='benchmarks/9_buffer/OD_intra.hq'
    HQAUTO='benchmarks/9_buffer/AH/9.2.hq'
    K=10; SEM='-opt' ; MODE='-find'
    compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

##############################
# 9.3 Buffer_sched, NI_intra #
##############################
if ((echo $* | grep -e "9.3" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/9_buffer/buffer_sched.smv'
    HQ='benchmarks/9_buffer/NI_intra.hq'
    HQAUTO='benchmarks/9_buffer/AH/9.3.hq'
    K=10; SEM='-opt' ; MODE='-find'
    compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

##############
# 10.1 NIexp #
##############
if ((echo $* | grep -e "10.1" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/10_NIexp/NIexp.smv'
    HQ='benchmarks/10_NIexp/tini.hq'
    HQAUTO='benchmarks/10_NIexp/AH/10.1.hq'
    K=10
    SEM='-pes'
    MODE='-find'
    compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

##############
# 10.2 NIexp #
##############
if ((echo $* | grep -e "10.2" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/10_NIexp/NIexp2.smv'
    HQ='benchmarks/10_NIexp/tsni.hq'
    HQAUTO='benchmarks/10_NIexp/AH/10.2.hq'
    K=10
    SEM='-pes'
    MODE='-find'
    compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

################
# 11.1 2Square #
################
if ((echo $* | grep -e "11.1" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/11_2Square/2Square.smv'
    HQ='benchmarks/11_2Square/2Square.hq'
    HQAUTO='benchmarks/11_2Square/AH/11.hq'
    K=64
    SEM='-pes'
    MODE='-find'
    compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

######################################
# 12.1 Mappying Synthesis - examples #
######################################
if ((echo $* | grep -e "12.1" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV_A='benchmarks/12_mapsynth/msynth1_MA.smv'
    SMV_B='benchmarks/12_mapsynth/msynth1_MB.smv'
    SMV_M='benchmarks/12_mapsynth/msynth1_MM.smv'
    HQ='benchmarks/12_mapsynth/msynth.hq'
    HQAUTO='benchmarks/12_mapsynth/AH/12.1.hq'
    K=5
    SEM='-pes'
    MODE='-find'
    echo "[ HyperQB ]"
    ${TIMEOUT} time ${HYPERQB} ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B} ${HQ} ${K} ${SEM} ${MODE}
    sleep 1 # buffering for file R/W
    echo "[ previous HyperQB (binary-gate) ]"
    ${TIMEOUT} time ${OLDHYPERQB} ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B} ${HQ} ${K} ${SEM} ${MODE}
    echo "[ AutoHyper ]"
    ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B} ${HQAUTO} --debug
    echo "[ AutoHyperQ ]"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B} ${HQAUTO} 
fi

#################################################################
# 12.2 Mappying Synthesis - Alice and Bob with non-interference #
#################################################################
if ((echo $* | grep -e "12.2" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV_A='benchmarks/12_mapsynth/msynth2_MA.smv'
    SMV_B='benchmarks/12_mapsynth/msynth2_MB.smv'
    SMV_M='benchmarks/12_mapsynth/msynth2_MM.smv'
    HQ='benchmarks/12_mapsynth/msynth2.hq'
    HQAUTO='benchmarks/12_mapsynth/AH/12.2.hq'
    K=8
    SEM='-pes'
    MODE='-find'
    echo "[ HyperQB ]"
    ${TIMEOUT} time ${HYPERQB} ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B} ${HQ} ${K} ${SEM} ${MODE}
    sleep 1 # buffering for file R/W
    echo "[ previous HyperQB (binary-gate) ]"
    ${TIMEOUT} time ${OLDHYPERQB} ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B} ${HQ} ${K} ${SEM} ${MODE}
    echo "[ AutoHyper ]"
    ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B} ${HQAUTO} --debug
    echo "[ AutoHyperQ ]"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B} ${HQAUTO} 
fi

#################
# 13.1 TEAMLTL1 #
#################
if ((echo $* | grep -e "13.1" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/13_teamltl/team1.smv'
    HQ='benchmarks/13_teamltl/team.hq'
    HQAUTO='benchmarks/13_teamltl/AH/13.1.hq'
    K=10
    SEM='-pes'
    MODE='-find'
    compare3 ${SMV} ${SMV} ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

#################
# 13.2 TEAMLTL2 # 
#################
if ((echo $* | grep -e "13.2" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/13_teamltl/team2.smv'
    HQ='benchmarks/13_teamltl/team.hq'
    HQAUTO='benchmarks/13_teamltl/AH/13.2.hq'
    K=20
    SEM='-pes'
    MODE='-find'
    compare3 ${SMV} ${SMV} ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

########################################
# 14.1 Non-det1: input non-determinism #
########################################
if ((echo $* | grep -e "14.1" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/14_ndet/NI_v2.smv'
    HQ='benchmarks/14_ndet/NI.hq'
    HQAUTO='benchmarks/14_ndet/AH/14.hq'
    K=5
    SEM='-pes'
    MODE='-find'
    compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

#############################################
# 14.2 Non-det2: transition non-determinism #
#############################################
if ((echo $* | grep -e "14.2" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/14_ndet/NI_v3.smv'
    HQ='benchmarks/14_ndet/NI.hq'
    HQAUTO='benchmarks/14_ndet/AH/14.hq'
    K=5
    SEM='-pes'
    MODE='-find'
    compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

###########
# 15.1 CSRF #
###########
if ((echo $* | grep -e "15.1" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/15_csrf/csrf.smv'
    HQ='benchmarks/15_csrf/csrf.hq'
    HQAUTO='benchmarks/15_csrf/AH/15.hq'
    K=10
    SEM='-pes'
    MODE='-find'
    compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

###########
# 16.1 Bank #
###########
if ((echo $* | grep -e "16.1" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/16_bank/bank.smv'
    HQ='benchmarks/16_bank/bank.hq'
    HQAUTO='benchmarks/16_bank/AH/16.hq'
    K=15
    SEM='-pes'
    MODE='-find'
    compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO} 
fi

##########
# 17.1 ATM #
##########
if ((echo $* | grep -e "17.1" -q) || (echo $* | grep -e "-allcases" -q)) then
    SMV='benchmarks/17_atm/atm.smv'
    HQ='benchmarks/17_atm/atm.hq'
    HQAUTO='benchmarks/17_atm/AH/17.hq'
    K=15
    SEM='-opt'
    MODE='-find'
    compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
fi

