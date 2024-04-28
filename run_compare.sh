#!/bin/bash
TIMEFORMAT='TOTAL TIME: %Rs'
HYPERQB="./hyperqb.sh"
OLDHYPERQB="./hyperqb_old.sh"
AUTOHYPER='/Users/tzuhan/install/autohyper/app/AutoHyper'
AUTOHYPERQ='/Users/tzuhan/install/autohyperq/app/AutoHyperQ'

### timeout setting ### 
TIMEOUT='timeout 1200s' # change here for timeout

### A function for comparising instnaces on: ###
#   - HyperQB (multi-gate)
#   - OldHyperQB (binary-gate)
#   - AutoHyper
#   - AutoHyperQ
function compare {
    echo "[[ HyperQB ]]"
    ${TIMEOUT} time ${HYPERQB} $1 $1 $2 $3 $4 $5 
    sleep 1 # buffering for file R/W
    echo ""
    echo "[[ OldHyperQB ]]"
    ${TIMEOUT} time ${OLDHYPERQB} $1 $1 $2 $3 $4 $5
    echo ""
    echo "[[ AutoHyper ]]"
    ${TIMEOUT} time ${AUTOHYPER} --nusmv $1 $6 --debug
    echo ""
    echo "[[ AutoHyperQ ]]"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv $1 $6 
}



###############
# 0.1 Bakery3, S1 #
###############
    # SMV='benchmarks/1_bakery/bakery_3procs.smv'
    # HQ='benchmarks/1_bakery/S1_3procs.hq'
    # HQAUTO='benchmarks/1_bakery/AH/0.1.hq'
    # K=10
    # SEM='-pes'
    # MODE='-find'
    # compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}


###############
# 0.2 Bakery3, S2 #
###############
    # SMV='benchmarks/1_bakery/bakery_3procs.smv'
    # HQ='benchmarks/1_bakery/S2_3procs.hq'
    # HQAUTO='benchmarks/1_bakery/AH/0.2.hq'
    # K=10
    # SEM='-pes'
    # MODE='-find'
    # compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}

###############
# 0.3 Bakery3, S3 #
###############
    # SMV='benchmarks/1_bakery/bakery_3procs.smv'
    # HQ='benchmarks/1_bakery/S3_3procs.hq'
    # HQAUTO='benchmarks/1_bakery/AH/0.3.hq'
    # K=10
    # SEM='-pes'
    # MODE='-find'
    # compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}

#################
# 1.1 Bakery3, sym1 #
#################
    # SMV='benchmarks/1_bakery/bakery_3procs.smv'
    # HQ='benchmarks/1_bakery/sym1_3procs.hq'
    # HQAUTO='benchmarks/1_bakery/AH/1.1.hq'
    # K=10
    # SEM='-pes'
    # MODE='-find'
    # compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}


#################
# 1.2 Bakery3, sym2 #
#################
    # SMV='benchmarks/1_bakery/bakery_3procs.smv'
    # HQ='benchmarks/1_bakery/sym2_3procs.hq'
    # HQAUTO='benchmarks/1_bakery/AH/1.2.hq'
    # K=10
    # SEM='-pes'
    # MODE='-find'
    # compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}

#################
# 1.3 Bakery5, sym1 #
#################
    # SMV='benchmarks/1_bakery/bakery_5procs.smv'
    # HQ='benchmarks/1_bakery/sym1_5procs.hq'
    # HQAUTO='benchmarks/1_bakery/AH/1.3.hq'
    # K=10
    # SEM='-pes'
    # MODE='-find'
    # compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}

#################
# 1.4 Bakery5, sym2 #
#################
    # SMV='benchmarks/1_bakery/bakery_5procs.smv'
    # HQ='benchmarks/1_bakery/sym2_5procs.hq'
    # HQAUTO='benchmarks/1_bakery/AH/1.4.hq'
    # K=10
    # SEM='-pes'
    # MODE='-find'
    # compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}




##########
# 2.1 SNARK1 #
##########
    # SMV1='benchmarks/2_snark/snark1_M1_concurrent.smv'
    # SMV2='benchmarks/2_snark/snark1_M2_sequential.smv'
    # HQ='benchmarks/2_snark/snark1.hq'
    # HQAUTO='benchmarks/2_snark/AH/2.1_neg.hq'
    # K=18
    # SEM='-pes'
    # MODE='-bughunt'
    # echo "[ HyperQB ]"
    # ${TIMEOUT} time ${HYPERQB} ${SMV1} ${SMV2} ${HQ} ${K} ${SEM} ${MODE} "-NN"
    # sleep 1 # buffering for file R/W
    # echo "[ OldHyperQB ]"
    # ${TIMEOUT} time ${OLDHYPERQB} ${SMV1} ${SMV2} ${HQ} ${K} ${SEM} ${MODE}
    # echo "[ AutoHyper ]"
    # ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV1} ${SMV2} ${HQAUTO} --debug
    # echo "[ AutoHyperQ ]"
    # ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV1} ${SMV2} ${HQAUTO} 



####################
# 3.1 3T_incorrect, NI #
####################
    # SMV='benchmarks/3_ni/NI_incorrect.smv'
    # HQ='benchmarks/3_ni/NI.hq'
    # HQAUTO='benchmarks/3_ni/AH/3.hq'
    # K=50
    # SEM='-opt'
    # MODE='-find'
    # compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}


####################
# 3.2 3T_correct, NI #
####################
    # SMV='benchmarks/3_ni/NI_correct.smv'
    # HQ='benchmarks/3_ni/NI.hq'
    # HQAUTO='benchmarks/3_ni/AH/3.hq'
    # K=50
    # SEM='-opt'
    # MODE='-find'
    # compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}


#######################
# NRP_incorrect, fair #
#######################
    # SMV='benchmarks/4_nrp/NRP_incorrect.smv'
    # HQ='benchmarks/4_nrp/NRP.hq'
    # HQAUTO='benchmarks/4_nrp/AH/4.hq'
    # K=15
    # SEM='-opt'
    # MODE='-find'
    # compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}


#######################
# 4.1 NRP_incorrect, fair #
#######################
    # SMV='benchmarks/4_nrp/NRP_correct.smv'
    # HQ='benchmarks/4_nrp/NRP.hq'
    # HQAUTO='benchmarks/4_nrp/AH/4.hq'
    # K=15
    # SEM='-opt'
    # MODE='-find'
    # compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}


#######################
# 4.2 NRP_correct, fair #
#######################
    # SMV='benchmarks/4_nrp/NRP_correct.smv'
    # HQ='benchmarks/4_nrp/NRP.hq'
    # HQAUTO='benchmarks/4_nrp/AH/4.hq'
    # K=15
    # SEM='-opt'
    # MODE='-find'
    # compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}



###################
# 5.1 Planning-rb #
###################
    SMV='benchmarks/5_planning/rb_100.smv'
    HQ='benchmarks/5_planning/rb_formula.hq'
    HQAUTO='benchmarks/5_planning/AH/5.hq'
    K=20; SEM='-opt' ; MODE='-find'
    compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}

###################
# 5.2 Planning-rb #
###################
    # SMV='benchmarks/5_planning/rb_400.smv'
    # HQ='benchmarks/5_planning/rb_formula.hq'
    # HQAUTO='benchmarks/5_planning/AH/5.hq'
    # K=40; SEM='-opt' ; MODE='-find'
    # compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}


###################
# 5.3 Planning-rb #
###################
    # SMV='benchmarks/5_planning/rb_1600.smv'
    # HQ='benchmarks/5_planning/rb_formula.hq'
    # HQAUTO='benchmarks/5_planning/AH/5.hq'
    # K=80; SEM='-opt' ; MODE='-find'
    # compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}


###################
# 5.4 Planning-rb #
###################
    # SMV='benchmarks/5_planning/rb_3600.smv'
    # HQ='benchmarks/5_planning/rb_formula.hq'
    # HQAUTO='benchmarks/5_planning/AH/5.hq'
    # K=120; SEM='-opt' ; MODE='-find'
    # compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}



##############
# 6.1 Mutant #
##############
    # SMV='benchmarks/6_mutation/mutation.smv'
    # HQ='benchmarks/6_mutation/mutation.hq'
    # HQAUTO='benchmarks/6_mutation/AH/6.hq'
    # K=10; SEM='-opt' ; MODE='-find'
    # compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}


######################
# 7.1 2Progs, coterm #
######################
    # SMV1='benchmarks/7_2Progs/prog1.smv'
    # SMV2='benchmarks/7_2Progs/prog2.smv'
    # HQ='benchmarks/7_2Progs/coterm.hq'
    # HQAUTO='benchmarks/7_2Progs/AH/7.hq'
    # K=102
    # SEM='-pes'
    # MODE='-find'
    # echo "[ HyperQB ]"
    # ${TIMEOUT} time ${HYPERQB} ${SMV1} ${SMV2} ${HQ} ${K} ${SEM} ${MODE}
    # sleep 1 # buffering for file R/W
    # # echo "[ OldHyperQB ]"
    # # ${TIMEOUT} time ${OLDHYPERQB} ${SMV1} ${SMV2} ${HQ} ${K} ${SEM} ${MODE}
    # echo "[ AutoHyper ]"
    # ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV1} ${SMV2} ${HQAUTO} --debug
    # # echo "[ AutoHyperQ ]"
    # # ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV1} ${SMV2} ${HQAUTO} 


############################
# 8.1 Wallet1, deniability #
############################
    # SMV='benchmarks/8_deniability/den_small.smv'
    # HQ='benchmarks/8_deniability/den.hq'
    # HQAUTO='benchmarks/8_deniability/AH/8.hq'
    # K=10
    # SEM='-pes'
    # MODE='-find'
    # echo "[ HyperQB ]"
    # ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${SMV} ${HQ} ${K} ${SEM} ${MODE}
    # sleep 1 # buffering for file R/W
    # echo "[ OldHyperQB ]"
    # ${TIMEOUT} time ${OLDHYPERQB} ${SMV} ${SMV} ${SMV} ${HQ} ${K} ${SEM} ${MODE}
    # echo "[ AutoHyper ]"
    # ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} --debug
    # echo "[ AutoHyperQ ]"
    # ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO} 


############################
# 8.2 Wallet2, deniability #
############################
    # SMV='benchmarks/8_deniability/den.smv'
    # HQ='benchmarks/8_deniability/den.hq'
    # HQAUTO='benchmarks/8_deniability/AH/8.hq'
    # K=20
    # SEM='-pes'
    # MODE='-find'
    # echo "[ HyperQB ]"
    # ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${SMV} ${HQ} ${K} ${SEM} ${MODE}
    # sleep 1 # buffering for file R/W
    # echo "[ OldHyperQB ]"
    # ${TIMEOUT} time ${OLDHYPERQB} ${SMV} ${SMV} ${SMV} ${HQ} ${K} ${SEM} ${MODE}
    # echo "[ AutoHyper ]"
    # ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} --debug
    # echo "[ AutoHyperQ ]"
    # ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO} 




##################
# 9.1 Buffer, OD #
#################
    # SMV='benchmarks/9_buffer/buffer.smv'
    # HQ='benchmarks/9_buffer/OD.hq'
    # HQAUTO='benchmarks/9_buffer/AH/9.1.hq'
    # K=10; SEM='-opt' ; MODE='-find'
    # echo "[ HyperQB ]"
    # ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} ${K} ${SEM} ${MODE}
    # sleep 1 # buffering for file R/W
    # echo "[ OldHyperQB ]"
    # ${TIMEOUT} time ${OLDHYPERQB} ${SMV} ${SMV} ${HQ} ${K} ${SEM} ${MODE}
    # echo "[ AutoHyper ]"
    # ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${SMV} ${HQAUTO} --debug
    # echo "[ AutoHyperQ ]"
    # ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${SMV} ${HQAUTO} 


##############################
# 9.2 Buffer_sched, OD_intra #
##############################
    # SMV='benchmarks/9_buffer/buffer_sched.smv'
    # HQ='benchmarks/9_buffer/OD_intra.hq'
    # HQAUTO='benchmarks/9_buffer/AH/9.2.hq'
    # K=10; SEM='-opt' ; MODE='-find'
    # echo "[ HyperQB ]"
    # ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} ${K} ${SEM} ${MODE}
    # sleep 1 # buffering for file R/W
    # echo "[ OldHyperQB ]"
    # ${TIMEOUT} time ${OLDHYPERQB} ${SMV} ${SMV} ${HQ} ${K} ${SEM} ${MODE}
    # echo "[ AutoHyper ]"
    # ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${SMV} ${HQAUTO} --debug
    # echo "[ AutoHyperQ ]"
    # ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${SMV} ${HQAUTO} 



##############################
# 9.3 Buffer_sched, NI_intra #
##############################
    # SMV='benchmarks/9_buffer/buffer_sched.smv'
    # HQ='benchmarks/9_buffer/NI_intra.hq'
    # HQAUTO='benchmarks/9_buffer/AH/9.3.hq'
    # K=10; SEM='-opt' ; MODE='-find'
    # echo "[ HyperQB ]"
    # ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${HQ} ${K} ${SEM} ${MODE}
    # sleep 1 # buffering for file R/W
    # echo "[ OldHyperQB ]"
    # ${TIMEOUT} time ${OLDHYPERQB} ${SMV} ${SMV} ${HQ} ${K} ${SEM} ${MODE}
    # echo "[ AutoHyper ]"
    # ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${SMV} ${HQAUTO} --debug
    # echo "[ AutoHyperQ ]"
    # ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${SMV} ${HQAUTO} 




##############
# 10.1 NIexp #
##############
    # SMV='benchmarks/10_NIexp/NIexp.smv'
    # HQ='benchmarks/10_NIexp/tini.hq'
    # HQAUTO='benchmarks/10_NIexp/AH/10.1.hq'
    # K=10
    # SEM='-pes'
    # MODE='-find'
    # compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}


##############
# 10.2 NIexp #
##############
    # SMV='benchmarks/10_NIexp/NIexp2.smv'
    # HQ='benchmarks/10_NIexp/tsni.hq'
    # HQAUTO='benchmarks/10_NIexp/AH/10.2.hq'
    # K=10
    # SEM='-pes'
    # MODE='-find'
    # compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}



###############
# 11.1 2Square #
###############
    # SMV='benchmarks/11_2Square/2Square.smv'
    # HQ='benchmarks/11_2Square/2Square.hq'
    # HQAUTO='benchmarks/11_2Square/AH/11.hq'
    # K=64
    # SEM='-pes'
    # MODE='-find'
    # compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}





######################################
# 12.1 Mappying Synthesis - examples #
######################################
    # SMV_A='benchmarks/12_mapsynth/msynth1_MA.smv'
    # SMV_B='benchmarks/12_mapsynth/msynth1_MB.smv'
    # SMV_M='benchmarks/12_mapsynth/msynth1_MM.smv'
    # HQ='benchmarks/12_mapsynth/msynth.hq'
    # HQAUTO='benchmarks/12_mapsynth/AH/12.1.hq'
    # K=5
    # SEM='-pes'
    # MODE='-find'
    # echo "[ HyperQB ]"
    # ${TIMEOUT} time ${HYPERQB} ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B} ${HQ} ${K} ${SEM} ${MODE}
    # sleep 1 # buffering for file R/W
    # echo "[ OldHyperQB ]"
    # ${TIMEOUT} time ${OLDHYPERQB} ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B} ${HQ} ${K} ${SEM} ${MODE}
    # echo "[ AutoHyper ]"
    # ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B} ${HQAUTO} --debug
    # echo "[ AutoHyperQ ]"
    # ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B} ${HQAUTO} 



#################################################################
# 12.2 Mappying Synthesis - Alice and Bob with non-interference #
#################################################################
    # SMV_A='benchmarks/12_mapsynth/msynth2_MA.smv'
    # SMV_B='benchmarks/12_mapsynth/msynth2_MB.smv'
    # SMV_M='benchmarks/12_mapsynth/msynth2_MM.smv'
    # HQ='benchmarks/12_mapsynth/msynth2.hq'
    # HQAUTO='benchmarks/12_mapsynth/AH/12.2.hq'
    # K=8
    # SEM='-pes'
    # MODE='-find'
    # echo "[ HyperQB ]"
    # ${TIMEOUT} time ${HYPERQB} ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B} ${HQ} ${K} ${SEM} ${MODE}
    # sleep 1 # buffering for file R/W
    # echo "[ OldHyperQB ]"
    # ${TIMEOUT} time ${OLDHYPERQB} ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B} ${HQ} ${K} ${SEM} ${MODE}
    # echo "[ AutoHyper ]"
    # ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B} ${HQAUTO} --debug
    # echo "[ AutoHyperQ ]"
    # ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B} ${HQAUTO} 




#################
# 13.1 TEAMLTL1 #
#################
    # SMV='benchmarks/13_teamltl/team1.smv'
    # HQ='benchmarks/13_teamltl/team.hq'
    # HQAUTO='benchmarks/13_teamltl/AH/13.1.hq'
    # K=10
    # SEM='-pes'
    # MODE='-find'
    # echo "[ HyperQB ]"
    # ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${SMV} ${HQ} ${K} ${SEM} ${MODE}
    # sleep 1 # buffering for file R/W
    # echo "[ OldHyperQB ]"
    # ${TIMEOUT} time ${OLDHYPERQB} ${SMV} ${SMV} ${SMV} ${HQ} ${K} ${SEM} ${MODE}
    # echo "[ AutoHyper ]"
    # ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${SMV} ${SMV} ${HQAUTO} --debug
    # echo "[ AutoHyperQ ]"
    # ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${SMV} ${SMV} ${HQAUTO} 


#################
# 13.2 TEAMLTL2 # 
#################
    # SMV='benchmarks/13_teamltl/team2.smv'
    # HQ='benchmarks/13_teamltl/team.hq'
    # HQAUTO='benchmarks/13_teamltl/AH/13.2.hq'
    # K=20
    # SEM='-pes'
    # MODE='-find'
    # echo "[ HyperQB ]"
    # ${TIMEOUT} time ${HYPERQB} ${SMV} ${SMV} ${SMV} ${HQ} ${K} ${SEM} ${MODE}
    # sleep 1 # buffering for file R/W
    # echo "[ OldHyperQB ]"
    # ${TIMEOUT} time ${OLDHYPERQB} ${SMV} ${SMV} ${SMV} ${HQ} ${K} ${SEM} ${MODE}
    # echo "[ AutoHyper ]"
    # ${TIMEOUT} time ${AUTOHYPER} --nusmv ${SMV} ${SMV} ${SMV} ${HQAUTO} --debug
    # echo "[ AutoHyperQ ]"
    # ${TIMEOUT} time ${AUTOHYPERQ} --nusmv ${SMV} ${SMV} ${SMV} ${HQAUTO} 



########################################
# 14.1 Non-det1: input non-determinism #
########################################
    # SMV='benchmarks/14_ndet/NI_v2.smv'
    # HQ='benchmarks/14_ndet/NI.hq'
    # HQAUTO='benchmarks/14_ndet/AH/14.hq'
    # K=5
    # SEM='-pes'
    # MODE='-find'
    # compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}

#############################################
# 14.2 Non-det2: transition non-determinism #
#############################################
    # SMV='benchmarks/14_ndet/NI_v3.smv'
    # HQ='benchmarks/14_ndet/NI.hq'
    # HQAUTO='benchmarks/14_ndet/AH/14.hq'
    # K=5
    # SEM='-pes'
    # MODE='-find'
    # compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}





###########
# 15 CSRF #
###########
# SMV='benchmarks/15_csrf/csrf.smv'
# HQ='benchmarks/15_csrf/csrf.hq'
# HQAUTO='benchmarks/15_csrf/AH/15.hq'
# K=10
# SEM='-pes'
# MODE='-find'
# compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}


###########
# 16 Bank #
###########
# SMV='benchmarks/16_bank/bank.smv'
# HQ='benchmarks/16_bank/bank.hq'
# HQAUTO='benchmarks/16_bank/AH/16.hq'
# K=15
# SEM='-pes'
# MODE='-find'
# compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO} 

##########
# 17 ATM #
##########
# SMV='benchmarks/17_atm/atm.smv'
# HQ='benchmarks/17_atm/atm.hq'
# HQAUTO='benchmarks/17_atm/AH/17.hq'
# K=15
# SEM='-opt'
# MODE='-find'
# compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}





#################
#   (unused)    #
#################

############################
# Adversarial Planning 3x3 #
############################
# SMV='benchmarks/5_planning/adv_v1.smv'
# HQ='cases_compare/adv_f1.hq'
# HQAUTO='cases_compare/adv_f2.hq'
# K=5
# SEM='-pes'
# MODE='-find'
# compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}


###########################################################
# Adversarial Planning 4x4 (overhead: inclusion checking) #
###########################################################
# SMV='cases_compare/adv_v2.smv'
# HQ='cases_compare/adv_f1.hq'
# HQAUTO='cases_compare/adv_f2.hq'
# K=5
# SEM='-pes'
# MODE='-find'
# compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}


##################
# Non-det0: test #
##################
# SMV='benchmarks/14_ndet/NI_v1.smv'
# HQ='benchmarks/14_ndet/NI.hq'
# HQAUTO='benchmarks/14_ndet/AH/14.hq'
# K=3
# SEM='-pes'
# MODE='-find'
# compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}
