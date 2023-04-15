#!/bin/sh
TIMEFORMAT="[time took: "%R"s]"
### Parameters
# SINGLE_PARSER=exec/single_model_parser.py
MULTI_PARSER=exec/model_parser.py
ARBITRARY_PARSER=exec/parser.py

# HyperQB parameters
# GENQBF=exec/genqbf
# GENQBF=src/cplusplus/genqbf
QUABS=exec/quabs
MAP=exec/util_mapvars
PARSE_BOOL=exec/util_parsebools



### setup output folder
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
# I=${OUTFOLDER}I.bool
# R=${OUTFOLDER}R.bool
# J=${OUTFOLDER}J.bool
# S=${OUTFOLDER}S.bool
I=${OUTFOLDER}I_1.bool
R=${OUTFOLDER}R_1.bool
J=${OUTFOLDER}I_2.bool
S=${OUTFOLDER}R_2.bool
P=${OUTFOLDER}P.hq
QSFILE=${OUTFOLDER}QS.bool
FORMULA=""


# output files
# \THH_TODO: add this before submission
while getopts "proj:" arg; do
  case $arg in
    n) PROJ=$OPTARG;;
  esac
done



## updated Jan.28:merge parse and bmc
echo "\n------( HyperQB START! )------\n"
## get current location and arguments
PWD=$(pwd)
ALLARG=$@


### fetch model(s) and HP formula
HQFILE='.hq'
# echo "Model(s)"
COUNTER=1
declare -a MODELS
for var in "$@"
do
  if [[ "${var}" =~ ^[0-9]+$ ]]; then
      k=${var}
      break
  elif [ ! -f "$var" ]; then
      echo "error: the file $var does not exist" # check the file is there
      exit 1
  fi

  if [[ "${var}" =~ .*"$HQFILE".* ]]; then
    FORMULA=${var}
  else
    MODELS[${COUNTER}]=${var}
  fi
  let COUNTER++
done
# echo "receiving models:  " ${MODELS[*]}


### Check which <mode> is used (-bughunt or -find) ###
if echo $* | grep -e "-find" -q
then
  echo "find witness mode (-find)"
  FLAG="-find"
elif echo $* | grep -e "-bughunt" -q
then
  echo "bug hunting mode (-bughunt)"
  FLAG="-bughunt"
else
  echo "mode is not specified, default to (-bughunt)"
  FLAG="-bughunt"
fi


### check which bunded semantics is used ###
if echo $* | grep -e "-pes" -q
then
  SEM="PES"
  echo "pessimistic semantics (-pes)"
elif echo $* | grep -e "-opt" -q
then
  SEM="OPT"
  echo "optimistic semantics (-opt)"
elif echo $* | grep -e "-hpes" -q
then
  SEM="TER_PES"
  echo "halting-pessimistic semantics (-hpes)"
elif echo $* | grep -e "-hopt" -q
then
  SEM="TER_OPT"
  echo "halting-optimistic semantics (-opt)"
else
  echo "(!) HyperQB error: incorrect semantic input."
  echo "  please use { -pes | -opt | -hpes | -hopt } semantics of the unrolling from one of the follows:"
  echo "  (pessimistic / optimistic / halting-pessimistic / halting-optimistic)"
  exit 1
fi


ERROR="(!) HyperQB error"
### parse the NuSMV models and the given formula ###
echo "parsing NuSMV models and HyperLTL formula..."
# time docker run --platform linux/amd64 -v ${PWD}:/mnt tzuhanmsu/hyperqube:latest /bin/bash -c "cd mnt/; python3 ${MULTI_PARSER} ${M1_NUSMVFILE} ${I} ${R} ${M2_NUSMVFILE} ${J} ${S} ${FORMULA} ${P} ${QSFILE} ${FLAG}; "
TIME_PARSE=$(time docker run --platform linux/amd64 -v ${PWD}:/mnt tzuhanmsu/hyperqube:latest /bin/bash -c "cd mnt/; python3 ${ARBITRARY_PARSER} ${OUTFOLDER} ${MODELS[@]} ${FORMULA} ${P} ${QSFILE} ${FLAG}; ")
echo ${TIME_PARSE}

# if any error happens in parsing, exit HyperQB
if [[ "${TIME_PARSE}" == *"$ERROR"* ]]; then
  exit 1
fi


### check what is the quantifier selection
# echo ${QSFILE}
if [ ! -f "${QSFILE}" ]; then
  echo "(!) HyperQB error: no ${QSFILE} exists."
  echo "please check the formula see if the quantifers are corretly specified. "
  exit 1
fi
source "${QSFILE}" # instantiate QS

echo "\ngenerating QBF BMC unrolling..."
QCIR_OUT=${OUTFOLDER}HQ.qcir
n=${#QS}
if [ ${n} -eq 2 ]
then
  GENQBF=exec/genqbf
  TIME_GENQBF=$(time ${GENQBF} -I ${I} -R ${R} -J ${J} -S ${S} -P ${P} -k ${k} -F ${QS} -f qcir -o ${QCIR_OUT} -sem ${SEM} -n --fast)
else
  GENQBF=exec/genqbf_v5 # updated genqbf with arbitrary quantifiers
  TIME_GENQBF=$(time ${GENQBF} -I ${I} -R ${R} -J ${J} -S ${S} -Q ${J} -W ${S} -Z ${J} -X ${S} -C ${J} -V ${S} -P ${P} -k ${k} -F ${QS}  -f qcir -o ${QCIR_OUT} -sem ${SEM} -n --fast)
fi

# GENQBF=src/cplusplus/genqbf ### insert this
echo "\nsolving QBF with QuAbS..."
# time ${QUABS}  --partial-assignment ${QCIR_OUT} 2>&1 | tee ${QUABS_OUT}
TIME_QUABS=$(time ${QUABS}  --partial-assignment ${QCIR_OUT} > ${QUABS_OUT})
OUTCOME=$(grep "r " ${QUABS_OUT})
# echo "\nQuABs outcome: "${OUTCOME}

echo "\n--------------- Summary of HyperQB ---------------"
echo "|  Models:     " ${MODELS[*]}
echo "|  Formula:    " ${FORMULA}
echo "|  Quantifiers:" ${QS}
echo "|  Bound k:    " ${k}
echo "|  Semantics:  " ${SEM}
echo "|  Mode:       " ${FLAG}
echo "|  QBF solving:" ${OUTCOME}
echo "----------------------------------------------------\n"
echo "\n------(END HyperQB)------\n"


exit 1

# ## TODO: update these two scripts using python
# echo "\n=== Get Nice-formatted Output if witness/counterexample is found ==="
# if [ ! -f "$QCIR_OUT" ]; then
#     echo "$QCIR_OUT does not exists"
#     exit 1
# fi
# echo "parsing into readable format..."
# ${MAP} ${QCIR_OUT} ${QUABS_OUT} ${MAP_OUT1} ${MAP_OUT2}
# ${PARSE_BOOL} ${MAP_OUT2} ${PARSE_OUT}














##### UNUSED
# if [ "$MODE" = "single" ]; then
# echo "|  Model:      " ${NUSMVFILE}
# elif [ "$MODE" = "multi" ]; then
# echo "|  Models:     " ${M1_NUSMVFILE} ", " ${M2_NUSMVFILE}
# fi
# # if wrong number of arguments
# if [ "$#" -ne 4 ] && [ "$#" -ne 5 ] && [ "$#" -ne 6 ] && [ "$#" -ne 7 ]; then
#   echo "HyperQB error: wrong number of arguments of HyperQB: \n"
#   echo "- Simgle-model BMC: $0 {model}.smv {formula}.hq"
#   echo "- Multi-model BMC:  $0 {model_1}.smv {model_2}.smv {formula}.hq \n"
#   echo "\n------(END HyperQB)------\n"
#   exit 1
# fi
#
#
# ### Check if multiple model is used
# if echo $* | grep -e "-single" -q
# then
#   echo "Running with single model semantic (-single)"
#   MODE=single
#   NUSMVFILE=$1
#   FORMULA=$2
#   k=$3
#   SEMANTICS=$4
#   if [ ! -f "$NUSMVFILE" ]; then
#       echo "error: $NUSMVFILE does not exist"
#       exit 1
#   fi
#   if [ ! -f "$FORMULA" ]; then
#       echo "error: $FORMULA does not exist"
#       exit 1
#   fi
#   ### using local python build
#   # python3 ${SINGLE_PARSER} ${NUSMVFILE} ${FORMULA} ${I} ${R} ${P} ${FLAG}
#   ### using docker
#   # --platform linux/amd64
#   time docker run --platform linux/amd64 -v ${PWD}:/mnt tzuhanmsu/hyperqube:latest /bin/bash -c "cd mnt/; python3  ${SINGLE_PARSER} ${NUSMVFILE} ${FORMULA} ${I} ${R} ${P} ${QSFILE} ${FLAG}; "
# elif echo $* | grep -e "-multi" -q
# then
#   echo "Running with multi model semantics (-multi)"
#   MODE=multi
#   M1_NUSMVFILE=$1
#   M2_NUSMVFILE=$2
#   FORMULA=$3
#   k=$4
#   SEMANTICS=$5
#   if [ ! -f "$M1_NUSMVFILE" ]; then
#       echo "error: $M1_NUSMVFILE does not exist"
#       exit 1
#   fi
#   if [ ! -f "$M2_NUSMVFILE" ]; then
#       echo "error: $M2_NUSMVFILE does not exist"
#       exit 1
#   fi
#   if [ ! -f "$FORMULA" ]; then
#       echo "error: $FORMULA does not exist"
#       exit 1
#   fi
#   ### using local python build
#   # python3 ${MULTI_PARSER} ${M1_NUSMVFILE} ${I} ${R} ${M2_NUSMVFILE} ${J} ${S} ${FORMULA}  ${P} ${FLAG}
#   ### using docker
#
#   echo "parsing models and formulas..."
#   time docker run --platform linux/amd64 -v ${PWD}:/mnt tzuhanmsu/hyperqube:latest /bin/bash -c "cd mnt/; python3 ${MULTI_PARSER} ${M1_NUSMVFILE} ${I} ${R} ${M2_NUSMVFILE} ${J} ${S} ${FORMULA} ${P} ${QSFILE} ${FLAG}; "
#
# else
#   echo "HyperQB error: please specify mode: { -single | -multi }\n"
#   exit 1
# fi


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
