#!/bin/sh

UNROLL=../../bin/unroll
I=I1.bool
R=R1.bool
P=P1.bool
TEMPORAL=G
k=5
OUT=O1.qcir
QS=AE

${UNROLL} -I ${I} -R ${R} -P ${P} -k ${k} -T ${TEMPORAL} -F ${QS} -f qcir -o ${OUT}
