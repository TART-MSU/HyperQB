#!/bin/sh

NUM=5

GENQBF=../../bin/genqbf
I=I5.bool
R=R5.bool
P=P5.bool
k=2
OUT=O${NUM}.qcir
QS=AE

${GENQBF} -I ${I} -R ${R} -P ${P} -k ${k} -F ${QS} -f qcir -o ${OUT}
