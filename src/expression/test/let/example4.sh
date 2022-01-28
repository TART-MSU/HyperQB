#!/bin/sh

NUM=3

GENQBF=../../bin/genqbf
I=I1.bool
R=R1.bool
P=P4.bool
k=1
OUT=O${NUM}.qcir
QS=AE

${GENQBF} -I ${I} -R ${R} -P ${P} -k ${k} -F ${QS} -f qcir -o ${OUT}
