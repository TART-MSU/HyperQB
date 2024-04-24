#!/bin/bash
TIMEFORMAT="TOTAL TIME: %Rs"
HYPERQB="./hyperqb.sh"
OLDHYPERQB="./hyperqb_old.sh"
AUTOHYPER='/Users/tzuhan/install/autohyper/app/AutoHyper'
AUTOHYPERQ='/Users/tzuhan/install/autohyperq/app/AutoHyperQ'

# timeout setting: 
TIMEOUT="timeout 30s" # change here with shorter timeout

# A function for comparising instnaces on:
#   - HyperQB (multi-gate)
#   - OldHyperQB (binary-gate)
#   - AutoHyper
#   - AutoHyperQ
function compare {
    echo "[ HyperQB ]"
    ${TIMEOUT} time ${HYPERQB} $1 $1 $2 $3 $4 $5
    echo "[ OldHyperQB ]"
    ${TIMEOUT} time ${OLDHYPERQB} $1 $1 $2 $3 $4 $5
    echo "[ AutoHyper ]"
    ${TIMEOUT} time ${AUTOHYPER} --nusmv $1 $6 -v 4
    echo "[ AutoHyperQ ]"
    ${TIMEOUT} time ${AUTOHYPERQ} --nusmv $1 $6 
}



# case 1:
SMV='benchmarks/14_ndet/NI_v1.smv'
HQ='benchmarks/14_ndet/NI_f1.hq'
HQAUTO='cases_compare/NI_f2.hq'
K=3
SEM='-pes'
MODE='-find'
compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}



# case 2: input non-determinism (overhead: inclusion check)
# SMV='cases_compare/NI_v2.smv'
# HQ='cases_compare/NI_f1.hq'
# HQAUTO='cases_compare/NI_f2.hq'
# K=5
# SEM='-pes'
# MODE='-find'
# compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}


# case 3: transition non-determinism (overhead: inclusion check)
# SMV='cases_compare/NI_v3.smv'
# HQ='cases_compare/NI_f1.hq'
# HQAUTO='cases_compare/NI_f2.hq'
# K=5
# SEM='-pes'
# MODE='-find'
# compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}


# Deniability small: (overhead: on product construction)
# SMV='cases_compare/den_small.smv'
# HQ='cases_compare/den_f1.hq'
# HQAUTO='cases_compare/den_f2.hq'
# K=7
# SEM='-pes'
# MODE='-find'
# compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}


# Deniability large:
# SMV='cases_compare/den.smv'
# HQ='cases_compare/den_f1.hq'
# HQAUTO='cases_compare/den_f2.hq'
# K=7
# SEM='-pes'
# MODE='-find'
# compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}


# # Mappying Synthesis - examples
# SMV_A='cases_compare/msynth_MA.smv'
# SMV_B='cases_compare/msynth_MB.smv'
# SMV_M='cases_compare/msynth_MM.smv'
# HQ='cases_compare/msynth_f1.hq'
# HQAUTO='cases_compare/msynth_f2.hq'
# K=5
# SEM='-pes'
# MODE='-find'
# time ${HYPERQB} ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B} ${HQ} ${K} ${SEM} ${MODE}
# time ${AUTOHYPER} --nusmv ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B}  ${HQAUTO} -v 4


# # Mappying Synthesis - Alice and Bob with non-interference
# SMV_A='cases_compare/msynth2_MA.smv'
# SMV_B='cases_compare/msynth2_MB.smv'
# SMV_M='cases_compare/msynth2_MM.smv'
# HQ='cases_compare/msynth2_f1.hq'
# HQAUTO='cases_compare/msynth2_f2.hq'
# echo "HyperQB total time: "
# # time ${HYPERQB} ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B} ${HQ} 8 -pes -find
# timeout 1200s time ${AUTOHYPER} --nusmv ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B}  ${HQAUTO} -v 4



# Adversarial Planning 3x3:
# SMV='cases_compare/adv_v1.smv'
# HQ='cases_compare/adv_f1.hq'
# HQAUTO='cases_compare/adv_f2.hq'
# K=5
# SEM='-pes'
# MODE='-find'
# compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}


# Adversarial Planning 4x4 (overhead: inclusion checking)
# SMV='cases_compare/adv_v2.smv'
# HQ='cases_compare/adv_f1.hq'
# HQAUTO='cases_compare/adv_f2.hq'
# K=5
# SEM='-pes'
# MODE='-find'
# compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}


# TEAMLTL:
# SMV='cases_compare/team.smv'
# HQ='cases_compare/team_f1.hq'
# HQAUTO='cases_compare/team_f2.hq'
# K=10
# SEM='-pes'
# MODE='-find'
# compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}


# SMV='cases_compare/team2.smv'
# HQ='cases_compare/team2_f1.hq'
# HQAUTO='cases_compare/team2_f2.hq'
# K=20
# SEM='-pes'
# MODE='-find'
# compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}


# Ksafety
# SMV='cases_compare/doubleSquare.smv'
# HQ='cases_compare/doubleSquare_f1.hq'
# HQAUTO='cases_compare/doubleSquare_f2.hq'
# K=64
# SEM='-pes'
# MODE='-find'
# compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}

# SMV='cases_compare/ni_example.smv'
# HQ='cases_compare/tini.hq'
# HQAUTO='cases_compare/tini_f2.hq'
# K=10
# SEM='-pes'
# MODE='-find'
# compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}

# SMV='cases_compare/ni_example.smv'
# HQ='cases_compare/tsni.hq'
# HQAUTO='cases_compare/tsni_f2.hq'
# K=10
# SEM='-pes'
# MODE='-find'
# compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}


# SMV='benchmarks/15_csrf/csrf.smv'
# HQ='benchmarks/15_csrf/csrf.hq'
# HQAUTO='analysis/AH/15.hq'
# K=10
# SEM='-pes'
# MODE='-bughunt'
# compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}


# SMV='benchmarks/15_csrf/csrf.smv'
# HQ='benchmarks/15_csrf/csrf.hq'
# K=10
# SEM="-pes"
# MODE="-bughunt"
# HQAUTO='analysis/AH/15.hq'
# compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}


# SMV='benchmarks/16_bank/bank.smv'
# HQ='benchmarks/16_bank/bank.hq'
# HQAUTO='analysis/AH/16.hq'
# K=20
# SEM='-pes'
# MODE='-bughunt'
# compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}


# SMV='benchmarks/17_atm/atm.smv'
# HQ='benchmarks/17_atm/atm.hq'
# HQAUTO='analysis/AH/17.hq'
# K=10
# SEM='-pes'
# MODE='-bughunt'
# compare ${SMV} ${HQ} ${K} ${SEM} ${MODE} ${HQAUTO}


# echo "[ HyperQB ]"
# time ${HYPERQB} ${SMV} ${SMV} ${HQ} ${K} ${SEM} ${MODE}
# echo "[ OldHyperQB ]"
# time ${OLDHYPERQB} ${SMV} ${SMV} ${HQ} ${K} ${SEM} ${MODE}
# time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} -v 4
# echo "[ AutoHyperQ ]"
# time ${AUTOHYPERQ} --nusmv ${SMV} ${HQAUTO} 