#!/bin/sh

GENQBF=../../bin/genqbf
I=I6.bool
R=R6.bool
J=J6.bool
S=S6.bool
P=P6.bool
k=5
OUT=O6.qcir
QS=AE

${GENQBF} -a -I ${I} -R ${R} -J ${J} -S ${S} -P ${P} -k ${k} -F ${QS} -f qcir -o ${OUT}
