#!/bin/bash
TIMEFORMAT="TOTAL: %Rs"
HYPERQB="./hyperqb.sh"
AUTOHYPER='/Users/tzuhan/install/autohyper/app/AutoHyper'

# case 1:
# SMV='cases_compare/NI_v1.smv'
# HQ='cases_compare/NI_f1.hq'
# HQAUTO='cases_compare/NI_f2.hq'
# time ${HYPERQB} ${SMV} ${SMV} ${HQ} 3 -pes -find
# echo ""
# echo "------(AutoHyper Starts)------"
# time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} -v 4


# case 2: input non-determinism (overhead: inclusion check)
# SMV='cases_compare/NI_v2.smv'
# HQ='cases_compare/NI_f1.hq'
# HQAUTO='cases_compare/NI_f2.hq'
# time ${HYPERQB} ${SMV} ${SMV} ${HQ} 5 -pes -find
# echo ""
# echo "------(AutoHyper Starts)------"
# time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} -v 4


# case 3: transition non-determinism (overhead: inclusion check)
# SMV='cases_compare/NI_v3.smv'
# HQ='cases_compare/NI_f1.hq'
# HQAUTO='cases_compare/NI_f2.hq'
# time ${HYPERQB} ${SMV} ${SMV} ${HQ} 5 -pes -find
# echo ""
# echo "------(AutoHyper Starts)------"
# time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} -v 4


# Deniability small: (overhead: on product construction)
# SMV='cases_compare/den_small.smv'
# HQ='cases_compare/den_f1.hq'
# HQAUTO='cases_compare/den_f2.hq'
# time ${HYPERQB} ${SMV} ${SMV} ${SMV} ${HQ} 7 -pes -find
# echo ""
# echo "------(AutoHyper Starts)------"
# time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} -v 4

# Deniability large:
# SMV='cases_compare/den.smv'
# HQ='cases_compare/den_f1.hq'
# HQAUTO='cases_compare/den_f2.hq'
# time ${HYPERQB} ${SMV} ${SMV} ${SMV} ${HQ} 7 -pes -find
# echo ""
# echo "------(AutoHyper Starts)------"
# time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} -v 4

# # Mappying Synthesis - examples
SMV_A='cases_compare/msynth_MA.smv'
SMV_B='cases_compare/msynth_MB.smv'
SMV_M='cases_compare/msynth_MM.smv'
HQ='cases_compare/msynth_f1.hq'
HQAUTO='cases_compare/msynth_f2.hq'
time ${HYPERQB} ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B} ${HQ} 5 -pes -find
echo ""
echo "------(AutoHyper Starts)------"
time ${AUTOHYPER} --nusmv ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B}  ${HQAUTO} -v 4


# # Mappying Synthesis - Alice and Bob with non-interference
# SMV_A='cases_compare/msynth2_MA.smv'
# SMV_B='cases_compare/msynth2_MB.smv'
# SMV_M='cases_compare/msynth2_MM.smv'
# HQ='cases_compare/msynth2_f1.hq'
# HQAUTO='cases_compare/msynth2_f2.hq'
# echo "HyperQB total time: "
# # time ${HYPERQB} ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B} ${HQ} 8 -pes -find
# echo ""
# echo "------(AutoHyper Starts)------"
# timeout 1200s time ${AUTOHYPER} --nusmv ${SMV_M} ${SMV_A} ${SMV_B} ${SMV_A} ${SMV_B}  ${HQAUTO} -v 4



# Adversarial Planning 3x3:
# SMV='cases_compare/adv_v1.smv'
# HQ='cases_compare/adv_f1.hq'
# HQAUTO='cases_compare/adv_f2.hq'
# time ${HYPERQB} ${SMV} ${SMV} ${HQ} 5 -pes -find
# echo ""
# echo "------(AutoHyper Starts)------"
# time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} -v 4


# Adversarial Planning 4x4 (overhead: inclusion checking)
# SMV='cases_compare/adv_v2.smv'
# HQ='cases_compare/adv_f1.hq'
# HQAUTO='cases_compare/adv_f2.hq'
# time ${HYPERQB} ${SMV} ${SMV} ${HQ} 5 -pes -find
# echo ""
# echo "------(AutoHyper Starts)------"
# time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} -v 4

# TEAMLTL:
# SMV='cases_compare/team.smv'
# HQ='cases_compare/team_f1.hq'
# HQAUTO='cases_compare/team_f2.hq'
# time ${HYPERQB} ${SMV} ${SMV} ${SMV} ${HQ} 10 -pes -find
# echo ""
# echo "------(AutoHyper Starts)------"
# time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} -v 4


# SMV='cases_compare/team2.smv'
# HQ='cases_compare/team2_f1.hq'
# HQAUTO='cases_compare/team2_f2.hq'
# time ${HYPERQB} ${SMV} ${SMV} ${SMV} ${HQ} 20 -pes -find
# echo ""
# echo "------(AutoHyper Starts)------"
# time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} -v 4

# Ksafety
# SMV='cases_compare/doubleSquare.smv'
# HQ='cases_compare/doubleSquare_f1.hq'
# HQAUTO='cases_compare/doubleSquare_f2.hq'
# time ${HYPERQB} ${SMV} ${SMV} ${HQ} 64 -pes -find
# echo ""
# echo "------(AutoHyper Starts)------"
# time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} -v 4

# SMV='cases_compare/ni_example.smv'
# HQ='cases_compare/tini.hq'
# HQAUTO='cases_compare/tini_f2.hq'
# time ${HYPERQB} ${SMV} ${SMV} ${HQ} 10 -pes -find
# echo ""
# echo "------(AutoHyper Starts)------"
# time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} -v 4

# SMV='cases_compare/ni_example.smv'
# HQ='cases_compare/tsni.hq'
# HQAUTO='cases_compare/tsni_f2.hq'
# time ${HYPERQB} ${SMV} ${SMV} ${HQ} 10 -pes -find
# echo ""
# echo "------(AutoHyper Starts)------"
# time ${AUTOHYPER} --nusmv ${SMV} ${HQAUTO} -v 4