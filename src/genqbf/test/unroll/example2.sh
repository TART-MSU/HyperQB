#!/bin/sh

EX=2

UNROLL=../../bin/unroll
I=I${EX}.bool
R=R${EX}.bool
P=P${EX}.bool
TEMPORAL=G
k=5
OUT=O${EX}.qcir
QS=AE

${UNROLL} -I ${I} -R ${R} -P ${P} -k ${k} -T ${TEMPORAL} -F ${QS} -f qcir -n -o ${OUT}
