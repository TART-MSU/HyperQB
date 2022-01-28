#!/bin/sh

NUM=7

GENQBF=../../bin/genqbf
I=I${NUM}.bool
R=R${NUM}.bool
J=J${NUM}.bool
S=S${NUM}.bool
P=P${NUM}.bool
k=5
OUT=O${NUM}.qcir
QS=AE

${GENQBF} -a -I ${I} -R ${R} -J ${J} -S ${S} -P ${P} -k ${k} -F ${QS} -f qcir -o ${OUT}
