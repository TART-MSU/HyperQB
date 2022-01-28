#!/bin/sh

GENQBF=../../bin/genqbf
I=I3.bool
R=R1.bool
P=P1.bool
k=5
OUT=O1.qcir
QS=AE

${GENQBF} -I ${I} -R ${R} -P ${P} -k ${k} -F ${QS} -f qcir -o ${OUT}
