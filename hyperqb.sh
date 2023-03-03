#!/bin/sh
# TIMEFORMAT=%R
## THHTODO: make sure the format is: /hyperqb.sh  bakery.smv bakery.smv symmetry.hq 10 -pes -bughunt
### Parameters
SINGLE_PARSER=exec/single_model_parser.py
MULTI_PARSER=exec/multi_model_parser.py

# HyperQube subTools

# GENQBF=exec/genqbf
# GENQBF=src/cplusplus/genqbf
QUABS=exec/quabs
MAP=exec/util_mapvars
PARSE_BOOL=exec/util_parsebools

# output files
# \THH_TODO: add this before submission
while getopts "proj:" arg; do
  case $arg in
    n) PROJ=$OPTARG;;
  esac
done

# \THH_TODO: put this back before submission.
# DATE=`date +"%Y-%m-%d@%T"`
DATE="today"
OUTFOLDER="build_"${DATE}"/"

rm -f -R "build_today/"
mkdir ${OUTFOLDER}

QCIR_OUT=${OUTFOLDER}HQ.qcir
QUABS_OUT=${OUTFOLDER}HQ.quabs
MAP_OUT1=${OUTFOLDER}_byName.cex
MAP_OUT2=${OUTFOLDER}_byTime.cex
PARSE_OUT=${OUTFOLDER}_formatted.cex
I=${OUTFOLDER}I.bool
R=${OUTFOLDER}R.bool
J=${OUTFOLDER}J.bool
S=${OUTFOLDER}S.bool
P=${OUTFOLDER}P.hq
QSFILE=${OUTFOLDER}QS.bool

## updated Jan.28:merge parse and bmc
echo "\n------( HyperQB START! )------\n"
## get current location
PWD=$(pwd)
## get all arguments
ALLARG=$@


# if wrong number of arguments
if [ "$#" -ne 4 ] && [ "$#" -ne 5 ] && [ "$#" -ne 6 ] && [ "$#" -ne 7 ]; then
  echo "HyperQB error: wrong number of arguments of HyperQB: \n"
  echo "- Simgle-model BMC: $0 {model}.smv {formula}.hq"
  echo "- Multi-model BMC:  $0 {model_1}.smv {model_2}.smv {formula}.hq \n"
  echo "\n------(END HyperQB)------\n"
  exit 1
fi


if echo $* | grep -e "-find" -q
then
  echo "Running with find witness mode (-find)"
  FLAG="-find"
elif echo $* | grep -e "-bughunt" -q
then
  echo "Running with bug hunting mode (-bughunt)"
  FLAG="-bughunt"
else
  echo "error: please enter mode with: -bughunt | -find "
fi


if echo $* | grep -e "-single" -q
then
  echo "Running with single model semantic (-single)"
  MODE=single
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
  ### using local python build
  # python3 ${SINGLE_PARSER} ${NUSMVFILE} ${FORMULA} ${I} ${R} ${P} ${FLAG}
  ### using docker
  # --platform linux/amd64
  time docker run --platform linux/amd64 -v ${PWD}:/mnt tzuhanmsu/hyperqube:latest /bin/bash -c "cd mnt/; python3  ${SINGLE_PARSER} ${NUSMVFILE} ${FORMULA} ${I} ${R} ${P} ${QSFILE} ${FLAG}; "
elif echo $* | grep -e "-multi" -q
then
  echo "Running with multi model semantics (-multi)"
  MODE=multi
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
  ### using local python build
  # python3 ${MULTI_PARSER} ${M1_NUSMVFILE} ${I} ${R} ${M2_NUSMVFILE} ${J} ${S} ${FORMULA}  ${P} ${FLAG}
  ### using docker

  echo "parsing models and formulas..."
  time docker run --platform linux/amd64 -v ${PWD}:/mnt tzuhanmsu/hyperqube:latest /bin/bash -c "cd mnt/; python3 ${MULTI_PARSER} ${M1_NUSMVFILE} ${I} ${R} ${M2_NUSMVFILE} ${J} ${S} ${FORMULA} ${P} ${QSFILE} ${FLAG}; "



else
  echo "HyperQB error: please specify mode: -single | -multi \n"
  exit 1
fi

## run BMC
## updated Jan.28
if echo $* | grep -e "-pes" -q
then
  SEM="PES"
elif echo $* | grep -e "-opt" -q
then
  SEM="OPT"
elif echo $* | grep -e "-hpes" -q
then
  SEM="TER_PES"
elif echo $* | grep -e "-hopt" -q
then
  SEM="TER_OPT"
else
  echo "HyperQB error: incorrect semantic input. "
  echo " use { -pes | -opt | -hpes | -hopt } semantics of the unrolling from one of the follows:"
  echo "             (pessimistic / optimistic / halting-pessimistic / halting-optimistic)"
  exit 1
fi


# cd ${OUTFOLDER}
if [ ! -f "$QSFILE" ]; then
  echo "HyperQB error: no QS.bool exists."
  exit 1
fi
# source "QS.bool"
source "${QSFILE}"
# cd ..
# source QS.bool
# cd ..

echo "\n--------------- Summary of Model Checking Info ---------------"
echo "|  Quantifiers:" ${QS}
echo "|  Bound k:    " ${k}
echo "|  Semantics:  " ${SEMANTICS}
echo "|  Mode:       " ${FLAG}
if [ "$MODE" = "single" ]; then
  echo "|  Model:      " ${NUSMVFILE}
elif [ "$MODE" = "multi" ]; then
  echo "|  Models:     " ${M1_NUSMVFILE} ", " ${M2_NUSMVFILE}
fi
echo "|  HyperLTL formula: " ${FORMULA}
echo "-------------------------------------------------------------- \n\n"

echo "\n=== Unrolling with genQBF + Solving with QuAbS ==="
echo "generating QBF BMC..."


#old genqbf
QCIR_OUT=${OUTFOLDER}HQ.qcir
echo "\n(ocaml genqbf)"


n=${#QS}

if [ ${n} -eq 2 ]
then
  # old genqbf, two quantifiers
  GENQBF=exec/genqbf
  time ${GENQBF} -I ${I} -R ${R} -J ${J} -S ${S} -P ${P} -k ${k} -F ${QS} -f qcir -o ${QCIR_OUT} -sem ${SEM} -n --fast
  # time ${GENQBF} -I ${I} -R ${R} -J ${J} -S ${S} -P ${P} -k ${k} -F ${QS} -f qcir -o ${QCIR_OUT} -sem ${SEM} -n
else
  # updated genqbf, arbitrary quantifiers
  GENQBF=exec/genqbf_v5
  time ${GENQBF} -I ${I} -R ${R} -J ${J} -S ${S} -Q ${J} -W ${S} -Z ${J} -X ${S} -C ${J} -V ${S} -P ${P} -k ${k} -F ${QS}  -f qcir -o ${QCIR_OUT} -sem PES -n
fi
echo "\nsolving QBF..."
# time ${QUABS}  --partial-assignment ${QCIR_OUT} 2>&1 | tee ${QUABS_OUT}
time ${QUABS}  --partial-assignment ${QCIR_OUT} > ${QUABS_OUT}
OUTCOME=$(grep "r " ${QUABS_OUT})
echo "\nQuABs outcome: "${OUTCOME}



# new_genqbf
# echo "\n(c++ genqbf)"
# GENQBF=src/cplusplus/old_genqbf
# time ${GENQBF} ${k} ${I} ${R} ${J} ${S} ${SEM} demo/mini_P.hq
# echo "\nsolving QBF..."
# time ${QUABS}  --partial-assignment ${QCIR_OUT} > ${QUABS_OUT}
# OUTCOME=$(grep "r " ${QUABS_OUT})
# echo "\nQuABs outcome: "${OUTCOME}
# rm ${QCIR_OUT}
# rm ${QUABS_OUT}

# TODO add QS quantifier for GENQBF, and use ${P}
# QCIR_OUT=${OUTFOLDER}HQ-cpp.qcir
# echo "\n(upgraded c++ genqbf)"
# GENQBF=src/cplusplus/genqbf
# time ${GENQBF} ${k} ${I} ${R} ${J} ${S} ${SEM} ${FORMULA}
# echo "\nsolving QBF..."
# time ${QUABS}  --partial-assignment ${QCIR_OUT} > ${QUABS_OUT}
# OUTCOME=$(grep "r " ${QUABS_OUT})
# echo "\nQuABs outcome: "${OUTCOME}

# put back
# echo "\nsolving QBF..."
# # time ${QUABS}  --partial-assignment ${QCIR_OUT} 2>&1 | tee ${QUABS_OUT}
# time ${QUABS}  --partial-assignment ${QCIR_OUT} > ${QUABS_OUT}
#  # ${QUABS} --statistics --preprocessing 0 --partial-assignment ${QCIR_OUT} 2>&1 | tee ${QUABS_OUT}
# OUTCOME=$(grep "r " ${QUABS_OUT})
# echo "\nQuABs outcome: "${OUTCOME}


## THHTODO: update these two scripts
echo "\n=== Get Nice-formatted Output if witness/counterexample is found ==="
if [ ! -f "$QCIR_OUT" ]; then
    echo "$QCIR_OUT does not exists"
    exit 1
fi

echo "parsing into readable format..."
${MAP} ${QCIR_OUT} ${QUABS_OUT} ${MAP_OUT1} ${MAP_OUT2}
${PARSE_BOOL} ${MAP_OUT2} ${PARSE_OUT}


echo "\n------(END HyperQB)------\n"


##### UNUSED
# echo ${ALLARG}
## execute python scripts on docker
# docker run -v ${PWD}:/mnt tzuhanmsu/hyperqube:latest /bin/bash -c "cd mnt/; ./parse.sh ${ALLARG}; "
## clean up previous generated
# echo "(clean up previous generated files..)\n"
# find . -name "*.bool"   -delete
# find . -name "*.cex"    -delete
# find . -name "*.quabs"  -delete
# find . -name "*.qcir"   -delete
# find . -name "*.cex"    -delete
# make clean

# # echo "---Counterexample Mapping---"
# javac ${MAP}.java
# java ${MAP}.java ${QCIR_OUT} ${QUABS_OUT} ${MAP_OUT1} ${MAP_OUT2}

# javac ${PARSE_BOOL}.java
# java ${PARSE_BOOL}.java ${MAP_OUT2} ${PARSE_OUT}
# echo  "(under condtruction...)"
# python3 ${PARSE_OUTPUT} ${MAP_OUT2} ${PARSE_OUT} ${k}
# #by time
