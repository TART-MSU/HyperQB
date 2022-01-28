#!/bin/sh
#coding:utf-8
#coding:utf-8

# HyperQube subTools
GENQBF=exec/genqbf
QUABS=exec/quabs
MAP=exec/util_mapvars
PARSE_BOOL=exec/util_parsebools



if echo $* | grep -e "pes" -q
then
  SEM="PES"
elif echo $* | grep -e "opt" -q
then
  SEM="OPT"
elif echo $* | grep -e "hpes" -q
then
  SEM="TER_PES"
elif echo $* | grep -e "hopt" -q
then
  SEM="TER_OPT"
else
  echo "HyperQube error: incorrect semantic input. "
  echo " use { pes | opt | hpes | hopt } semantics of the unrolling from one of the follows:"
  echo "             (pessimistic / optimistic / halting-pessimistic / halting-optimistic)"
  exit 1
fi


if echo $* | grep -e "-single" -q
then
  # echo "[ Running with single model mode ]"
  # MODE=SINGLE
  # echo "perform single model BMC"
  NUSMVFILE=$1
  FORMULA=$2
  k=$3
  SEMANTICS=$4

  I=I.bool
  R=R.bool
  J=I.bool
  S=R.bool
  P=P.bool

elif echo $* | grep -e "-multi" -q
then
  # echo "[ Running with multi model mode ]"
  # MODE=MULTI
  M1_NUSMVFILE=$1
  M2_NUSMVFILE=$2
  FORMULA=$3
  k=$4
  SEMANTICS=$5

  I=I.bool
  R=R.bool
  J=J.bool
  S=S.bool
  P=P.bool

else
  echo "HyperQube error: please specify mode: -single | -multi \n"
  exit 1
fi


if [ ! -f "QS.bool" ]; then
    exit 1
fi
source QS.bool


# output files
QCIR_OUT=HQ.qcir
QUABS_OUT=HQ.quabs

# simple mapping of variable names and values
# TODO: do it in python
# MAP=util/util_mapvars.java
# MAP=util_mapvars
MAP_OUT1=OUTPUT_byName.cex
MAP_OUT2=OUTPUT_byTime.cex
# simple parser for convering binaries to digits
# PARSE_BOOL=util/util_parsebools.java
# PARSE_BOOL=util_parsebools
PARSE_OUT=OUTPUT_formatted.cex




echo "\n--------------- Summary of Model Checking Info ---------------"
echo "|  Quantifiers:" ${QS}
echo "|  Bound k:    " ${k}
echo "|  Semantics:  " ${SEMANTICS}
if [ "$MODE" = "single" ]; then
  echo "|  Model:      " ${NUSMVFILE}
elif [ "$MODE" = "multi" ]; then
  echo "|  Models:     " ${M1_NUSMVFILE} ", " ${M2_NUSMVFILE}
fi
echo "|  HyperLTL formula: " ${FORMULA}
echo "-------------------------------------------------------------- \n\n"


echo "\n============ Unrolling with genQBF + Solving with QuAbS ============"
# echo "---Generating QCIR---"
echo "generating QBF BMC..."
 ${GENQBF} -I ${I} -R ${R} -J ${J} -S ${S} -P ${P} -k ${k} -F ${QS}  -f qcir -o ${QCIR_OUT} -sem ${SEM} -n --fast



# echo "---QUABS solving---"
echo "solving QBF..."
  ${QUABS}  --partial-assignment ${QCIR_OUT} 2>&1 | tee ${QUABS_OUT}
#  ${QUABS} --statistics --preprocessing 0 --partial-assignment ${QCIR_OUT} 2>&1 | tee ${QUABS_OUT}


# echo "---Parse All Binary Numbers---"
echo "\n============ Get Nice-formatted Output if Output is avaialbe ============"

if [ ! -f "$QCIR_OUT" ]; then
    echo "$QCIR_OUT not exists"
    exit 1
fi

echo "parsing into readable format..."
# # echo "---Counterexample Mapping---"
# javac ${MAP}.java
# java ${MAP}.java ${QCIR_OUT} ${QUABS_OUT} ${MAP_OUT1} ${MAP_OUT2}
${MAP} ${QCIR_OUT} ${QUABS_OUT} ${MAP_OUT1} ${MAP_OUT2}

# javac ${PARSE_BOOL}.java
# java ${PARSE_BOOL}.java ${MAP_OUT2} ${PARSE_OUT}
${PARSE_BOOL} ${MAP_OUT2} ${PARSE_OUT}
# echo  "(under condtruction...)"
# python3 ${PARSE_OUTPUT} ${MAP_OUT2} ${PARSE_OUT} ${k}
# #by time

echo "\n------(END HyperQube)------\n"
