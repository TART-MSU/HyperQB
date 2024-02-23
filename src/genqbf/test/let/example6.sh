#!/bin/sh

GENQBF=../../bin/genqbf
I=I3.bool
R=R1.bool
J=I3.bool
S=R1.bool
P=P1.bool
k=5
OUT=O6.qcir
QS=AE
SEM=PES

${GENQBF} -I ${I} -J ${J} -S ${S} -R ${R} -P ${P} -k ${k} -F ${QS} -sem ${SEM} -f qcir -o  ${OUT}
