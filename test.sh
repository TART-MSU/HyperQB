#!/bin/bash
TIMEFORMAT="%Rs"
GENQBF=src/expression/bin/genqbf # classic 1 quants
I=test/I.bool
R=test/R.bool
J=test/I.bool
S=test/R.bool
P=test/P.hq
k=1
QS='EE'
SEM='PES'
QCIR_OUT=test/HQ.qcir
time ${GENQBF} -I ${I} -R ${R} -J ${J} -S ${S} -P ${P} -k ${k} -F ${QS} -f qcir -o ${QCIR_OUT} -sem ${SEM} -n --fast

AUTOHYPER='/Users/tzuhan/install/autohyper/app/AutoHyper'

# case 1:
# SMV='cases_compare/NI_v1.smv'
# HQ='cases_compare/NI_f1.hq'
# HQAUTO='cases_compare/NI_f2.hq'
# time ./hyperqb.sh ${SMV} ${SMV} ${HQ} 3 -pes -find
# echo ""
# echo "------(AutoHyper Starts)------"
# time ${AUTOHYPER} -nusmv ${SMV} ${HQAUTO} -v 4


# case 2: input non-determinism (overhead: inclusion check)
# SMV='cases_compare/NI_v2.smv'
# HQ='cases_compare/NI_f1.hq'
# HQAUTO='cases_compare/NI_f2.hq'
# time ./hyperqb.sh ${SMV} ${SMV} ${HQ} 3 -pes -find
# echo ""
# echo "------(AutoHyper Starts)------"
# time ${AUTOHYPER} -nusmv ${SMV} ${HQAUTO} -v 4


# case 3: transition non-determinism (overhead: inclusion check)
# SMV='cases_compare/NI_v3.smv'
# HQ='cases_compare/NI_f1.hq'
# HQAUTO='cases_compare/NI_f2.hq'
# time ./hyperqb.sh ${SMV} ${SMV} ${HQ} 4 -pes -find
# echo ""
# echo "------(AutoHyper Starts)------"
# time ${AUTOHYPER} -nusmv ${SMV} ${HQAUTO} -v 4


# Adversarial Planning 3x3:
# SMV='cases_compare/adv_v1.smv'
# HQ='cases_compare/adv_f1.hq'
# HQAUTO='cases_compare/adv_f2.hq'
# time ./hyperqb.sh ${SMV} ${SMV} ${HQ} 5 -pes -find
# echo ""
# echo "------(AutoHyper Starts)------"
# time ${AUTOHYPER} -nusmv ${SMV} ${HQAUTO} -v 4


# Adversarial Planning 4x4 (overhead: inclusion checking)
# SMV='cases_compare/adv_v2.smv'
# HQ='cases_compare/adv_f1.hq'
# HQAUTO='cases_compare/adv_f2.hq'
# time ./hyperqb.sh ${SMV} ${SMV} ${HQ} 5 -pes -find
# echo ""
# echo "------(AutoHyper Starts)------"
# time ${AUTOHYPER} -nusmv ${SMV} ${HQAUTO} -v 4


# Deniability small: (overhead: on product construction)
# SMV='cases_compare/den_small.smv'
# HQ='cases_compare/den_f1.hq'
# HQAUTO='cases_compare/den_f2.hq'
# time ./hyperqb.sh ${SMV} ${SMV} ${SMV} ${HQ} 7 -pes -find
# echo ""
# echo "------(AutoHyper Starts)------"
# time ${AUTOHYPER} -nusmv ${SMV} ${HQAUTO} -v 4

# Deniability:
# SMV='cases_compare/den.smv'
# HQ='cases_compare/den_f1.hq'
# HQAUTO='cases_compare/den_f2.hq'
# time ./hyperqb.sh ${SMV} ${SMV} ${SMV} ${HQ} 7 -pes -find
# echo ""
# echo "------(AutoHyper Starts)------"
# time ${AUTOHYPER} -nusmv ${SMV} ${HQAUTO} -v 4
