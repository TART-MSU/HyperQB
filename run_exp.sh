#!/bin/sh
TIMEFORMAT="time: %Rs"
OUTFOLDER="build_today/"
# QUABS_OUT=${OUTFOLDER}HQ.quabs
MAP_OUT1=${OUTFOLDER}_byName.cex
MAP_OUT2=${OUTFOLDER}_byTime.cex
PARSE_OUT=${OUTFOLDER}_formatted.cex
I=${OUTFOLDER}I_1.bool
R=${OUTFOLDER}R_1.bool
J=${OUTFOLDER}I_2.bool
S=${OUTFOLDER}R_2.bool
P=${OUTFOLDER}P.hq
QSFILE=${OUTFOLDER}QS

SEM="PES"
QUABS=./exec/quabs
GENQBF=src/expression/bin/genqbf
k=18
NUM="-n"
# NUM=""
QS="EA"
NEW="NN"

# NEW="YY"
# ./hyperqb.sh demo/bakery.smv demo/bakery.smv demo/symmetry.hq ${k} -pes -find
# ./hyperqb.sh demo/SNARK1_M1_concurrent.smv demo/SNARK1_M2_sequential.smv demo/lin.hq ${k} -pes -debug
# ./hyperqb.sh demo/SNARK_conc.smv demo/SNARK_seq.smv demo/lin.hq ${k} -pes
# ./hyperqb.sh demo/bakery.smv demo/bakery.smv demo/temp.hq 1 -pes -find
# ./hyperqb.sh demo/mini.smv demo/mini.smv demo/mini.hq ${k} -pes -find 
# ./hyperqb.sh demo/small.smv demo/small.smv demo/small.hq ${k} -pes -find 
# ./hyperqb.sh demo/simple.smv demo/simple.smv demo/simple.hq ${k} -pes -find
# ./hyperqb.sh demo/robotic_sp_100.smv demo/robotic_sp_100.smv demo/robotic_sp.hq ${k} -pes -find 
# ./hyperqb.sh demo/robotic_sp_400.smv demo/robotic_sp_400.smv demo/robotic_sp.hq ${k} -pes -find 
# ./hyperqb.sh demo/robotic_sp3.smv demo/robotic_sp3.smv demo/robotic_sp.hq ${k} -pes -find 
# ./hyperqb.sh demo/robotic_sp10.smv demo/robotic_sp10.smv demo/robotic_sp.hq ${k} -pes -find 
# ./hyperqb.sh demo/robotic_sp13.smv demo/robotic_sp13.smv demo/robotic_sp.hq ${k} -pes -find 
# ./hyperqb.sh demo/robotic_sp15.smv demo/robotic_sp15.smv demo/robotic_sp.hq ${k} -pes -find 
# ./hyperqb.sh demo/robotic_sp20.smv demo/robotic_sp20.smv demo/robotic_sp.hq ${k} -pes -find 
# ./hyperqb.sh demo/robotic_sp.smv demo/robotic_sp.smv demo/robotic_sp.hq ${k} -pes -find 

### build new genqbf
cd src/expression
make clean
make 
echo "\n(run HyperQB)"
cd ..
cd ..

echo "formula quantification: " ${QS}
# src/expression/bin/genqbf -I ${I} -R ${R} -J ${J} -S ${S} -P ${P} -k ${k} -F ${QS} -f qcir -o ${QCIR_OUT} -sem ${SEM} -n --fast --debug -new "YN"
echo "\n=== old unrolling (NN) ===" 
QCIR_OUT=${OUTFOLDER}HQ_old.qcir
${GENQBF} -I ${I} -R ${R} -J ${J} -S ${S} -P ${P} -k ${k} -F ${QS} -f qcir -o ${QCIR_OUT} -sem ${SEM} ${NUM} --fast -new "NN"
echo "File size: "
stat -f%z ${QCIR_OUT}
time ${QUABS} --partial-assignment ${QCIR_OUT}


### New encoding, under development ###

# echo "\n===new encoding:" ${NEW} "==="
# QCIR_OUT=${OUTFOLDER}HQ_new.qcir
# ${GENQBF} -I ${I} -R ${R} -J ${J} -S ${S} -P ${P} -k ${k} -F ${QS} -f qcir -o ${QCIR_OUT} -sem ${SEM} ${NUM} --fast --debug -new ${NEW}
# echo "File size: "
# stat -f%z ${QCIR_OUT}
# time ${QUABS} --partial-assignment ${QCIR_OUT}

# echo "\n=== apply new encoding on M1 (YN) ==="
# QCIR_OUT=${OUTFOLDER}HQ_YN.qcir
# ${GENQBF} -I ${I} -R ${R} -J ${J} -S ${S} -P ${P} -k ${k} -F ${QS} -f qcir -o ${QCIR_OUT} -sem ${SEM} ${NUM} --fast --debug -new "YN"
# echo "File size: "
# stat -f%z ${QCIR_OUT}
# time ${QUABS} ${QCIR_OUT}


# echo "\n=== apply new encoding on M2 (NY) ==="
# QCIR_OUT=${OUTFOLDER}HQ_NY.qcir
# ${GENQBF} -I ${I} -R ${R} -J ${J} -S ${S} -P ${P} -k ${k} -F ${QS} -f qcir -o ${QCIR_OUT} -sem ${SEM} ${NUM} --fast --debug -new "NY"
# echo "File size: "
# stat -f%z ${QCIR_OUT}
# time ${QUABS} ${QCIR_OUT}


# echo "\n=== apply new encoding on M1&M2 (YY) ==="
# QCIR_OUT=${OUTFOLDER}HQ_YY.qcir
# ${GENQBF} -I ${I} -R ${R} -J ${J} -S ${S} -P ${P} -k ${k} -F ${QS} -f qcir -o ${QCIR_OUT} -sem ${SEM} ${NUM} --fast --debug -new "YY"
# echo "File size: "
# stat -f%z ${QCIR_OUT}
# time ${QUABS} ${QCIR_OUT}


# src/expression/bin/genqbf -I ${I} -R ${R} -J ${J} -S ${S} -P ${P} -k ${k} -F ${QS} -f qcir -o ${QCIR_OUT} -sem ${SEM} -n --fast --debug -new "YN"

# ./hyperqb.sh demo/mini.smv demo/mini.smv demo/mini.hq 2 -pes -find

# ./hyperqb.sh demo/small.smv demo/small.smv demo/small.hq 5 -pes -find

# ./hyperqb.sh demo/robotic_sp10.smv demo/robotic_sp10.smv demo/robotic_sp.hq 10 -pes -find
# ./hyperqb.sh demo/robotic_sp.smv demo/robotic_sp.smv demo/robotic_sp.hq 3 -pes -find
# ./hyperqb.sh demo/robotic_sp5.smv demo/robotic_sp5.smv demo/robotic_sp.hq 5 -pes -find
# ./hyperqb.sh demo/robotic_sp10.smv demo/robotic_sp10.smv demo/robotic_sp.hq 10 -pes -find
# ./hyperqb.sh demo/robotic_sp13.smv demo/robotic_sp13.smv demo/robotic_sp.hq 13 -pes -find
# ./hyperqb.sh demo/robotic_sp15.smv demo/robotic_sp15.smv demo/robotic_sp.hq 15 -pes -find
# ./hyperqb.sh demo/robotic_sp20.smv demo/robotic_sp20.smv demo/robotic_sp.hq 20 -pes -find

# cp build_today/HQ.qcir gen_qcir/test/AB.qcir
# ./hyperqb.sh demo/robotic_sp15.smv demo/robotic_sp15.smv demo/robotic_sp.hq 15 -pes -find