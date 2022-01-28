#!/bin/sh
#coding:utf-8
#coding:utf-8

echo "\n------( HyperQube START! )------\n"


SINGLE_PARSER=exec/single_model_parser.py
MULTI_PARSER=exec/multi_model_parser.py


## clean up previous generated
echo "(clean up previous generated files)"
make clean

# if wrong number of arguments
if [ "$#" -ne 4 ] && [ "$#" -ne 5 ] && [ "$#" -ne 6 ] && [ "$#" -ne 7 ]; then
  echo "HyperQube error: wrong number of arguments of HyperQube: \n"
  echo "- Simgle-model BMC: $0 {model}.smv {formula}.hq"
  echo "- Multi-model BMC:  $0 {model_1}.smv {model_2}.smv {formula}.hq \n"
  echo "\n------(END HyperQube)------\n"
  exit 1
fi



if echo $* | grep -e "-find" -q
then
  echo "[ Running with find witness mode ]"
  FLAG="-find"
elif echo $* | grep -e "-bughunt" -q
then
  echo "[ Running with bug hunting mode ]"
  FLAG="-bughunt"
else
  echo FLAG=""
fi



if echo $* | grep -e "-single" -q
then
  echo "[ Running with single model mode ]"
  MODE=SINGLE
  echo "perform single model BMC"
  NUSMVFILE=$1
  FORMULA=$2
  k=$3
  SEMANTICS=$4

  if [ ! -f "$NUSMVFILE" ]; then
      echo "error: $NUSMVFILE does not exist"
      exit 1
  fi

  if [ ! -f "$FORMULA" ]; then
      echo "error: $FORMULA does not exist"
      exit 1
  fi

  I=I.bool
  R=R.bool
  J=I.bool
  S=R.bool
  P=P.bool

  python3 ${SINGLE_PARSER} ${NUSMVFILE} ${FORMULA} ${I} ${R} ${P} ${FLAG}
elif echo $* | grep -e "-multi" -q
then
  echo "[ Running with multi model mode ]"
  MODE=MULTI
  M1_NUSMVFILE=$1
  M2_NUSMVFILE=$2
  FORMULA=$3
  k=$4
  SEMANTICS=$5
  if [ ! -f "$M1_NUSMVFILE" ]; then
      echo "error: $M1_NUSMVFILE does not exist"
      exit 1
  fi
  if [ ! -f "$M2_NUSMVFILE" ]; then
      echo "error: $M2_NUSMVFILE does not exist"
      exit 1
  fi
  if [ ! -f "$FORMULA" ]; then
      echo "error: $FORMULA does not exist"
      exit 1
  fi
  I=I.bool
  R=R.bool
  J=J.bool
  S=S.bool
  P=P.bool
  python3 ${MULTI_PARSER} ${M1_NUSMVFILE} ${I} ${R} ${M2_NUSMVFILE} ${J} ${S} ${FORMULA}  ${P} ${FLAG}
else
  echo "HyperQube error: please specify mode: -single | -multi \n"
  exit 1
fi
