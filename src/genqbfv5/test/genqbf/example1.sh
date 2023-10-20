#!/bin/sh

GENQBF=../../bin/genqbf
I=I1.bool
R=R1.bool
P=P1.bool
k=3
OUT=O1.qcir
QS=EE
SEM=OPT

QUABS=../../../quabs

${GENQBF} -I ${I} -R ${R} -J ${I} -S ${R} -P ${P} -k ${k} -F ${QS} -f qcir -o ${OUT} -sem ${SEM} -n
${QUABS} --statistics --preprocessing 0 --partial-assignment ${OUT}
