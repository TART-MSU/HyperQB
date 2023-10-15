#!/bin/bash
TIMEFORMAT="%Rs"
### Parameters
MULTI_PARSER=exec/model_parser.py
ARBITRARY_PARSER=exec/parser.py

# HyperQB parameters
QUABS=exec/quabs
MAP=exec/util_mapvars
PARSE_BOOL=exec/util_parsebools
ERROR="(!) HyperQB error"

### setup output folder
# \THH_TODO: put this back before submission.
# DATE=`date +"%Y-%m-%d@%T"`
DATE="today"

OUTFOLDER="build_"${DATE}"/"
rm -f -R ${OUTFOLDER}

CEXFOLDER="build_cex/"
# rm -f -R ${CEXFOLDER}

mkdir ${OUTFOLDER}
mkdir ${CEXFOLDER}

QUABS_OUT=${OUTFOLDER}HQ.quabs
MAP_OUT1=${OUTFOLDER}_byName.cex
MAP_OUT2=${OUTFOLDER}_byTime.cex
PARSE_OUT=${CEXFOLDER}formatted.cex
I=${OUTFOLDER}I_1.bool
R=${OUTFOLDER}R_1.bool
J=${OUTFOLDER}I_2.bool
S=${OUTFOLDER}R_2.bool
P=${OUTFOLDER}P.hq
QSFILE=${OUTFOLDER}QS
FORMULA=""


# output files
# \THH_TODO: add this before submission
while getopts "proj:" arg; do
  case $arg in
    n) PROJ=$OPTARG;;
  esac
done

## updated Jan.28:merge parse and bmc
echo ""
echo "-------( HyperQB START! )-------"
## get current location and arguments
PWD=$(pwd)
ALLARG=$@


### fetch model(s) and HP formula
HQFILE='.hq'
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


### Check which <mode> is used (-bughunt or -find) ###
if echo $* | grep -e "-find" -q
then
  FLAG="-find"
elif echo $* | grep -e "-bughunt" -q
then
  FLAG="-bughunt"
else
  echo "*mode is not specified, default to (-bughunt)"
  FLAG="-bughunt"
fi


### check which bunded semantics is used ###
if echo $* | grep -e "-pes" -q
then
  SEM="PES"
  # echo "pessimistic semantics (-pes)"
elif echo $* | grep -e "-opt" -q
then
  SEM="OPT"
  # echo "optimistic semantics (-opt)"
elif echo $* | grep -e "-hpes" -q
then
  SEM="TER_PES"
  # echo "halting-pessimistic semantics (-hpes)"
elif echo $* | grep -e "-hopt" -q
then
  SEM="TER_OPT"
  # echo "halting-optimistic semantics (-opt)"
else
  echo "(!) HyperQB error: incorrect semantic input."
  echo "  please use { -pes | -opt | -hpes | -hopt } semantics of the unrolling from one of the follows:"
  echo "  (pessimistic / optimistic / halting-pessimistic / halting-optimistic)"
  exit 1
fi


### parse the NuSMV models and the given formula ###

printf "NuSMV models parsing...\n" 
printf "HyperLTL formula parsing...\n"
# RUN PARSER on Docker
# echo "(using docker for parsing, parsing could become slower)"
TIME_PARSE=$(docker run --platform linux/amd64 -v ${PWD}:/mnt tzuhanmsu/hyperqube:latest /bin/bash -c "cd mnt/; TIMEFORMAT="%Rs"; time python3 ${ARBITRARY_PARSER} ${OUTFOLDER} ${MODELS[@]} ${FORMULA} ${P} ${QSFILE} ${FLAG}; ")

# RUN PARSER locally, if the setup on your local machine is successful
# TIME_PARSE=$(time python3 ${ARBITRARY_PARSER} ${OUTFOLDER} ${MODELS[@]} ${FORMULA} ${P} ${QSFILE} ${FLAG})


# if any error happens in parsing, exit HyperQB
if [[ "${TIME_PARSE}" == *"$ERROR"* ]]; then
  echo ${TIME_PARSE}
  exit 1
fi

### check what is the quantifier selection
if [ ! -f "${QSFILE}" ]; then
  echo "(!) HyperQB error: no ${QSFILE} exists."
  echo "please check the formula see if the quantifers are corretly specified. "
  exit 1
fi
source "${QSFILE}" # instantiate QS


# diskutil erasevolume apfs "RAMDisk" `hdiutil attach -nomount ram://20000000`
# touch /Volumes/RAMDisk/HQ.qcir
# QCIR_OUT=/Volumes/RAMDisk/HQ.qcir
printf "BMC unrolling with genqbf...\n"

QCIR_OUT=${OUTFOLDER}HQ.qcir
n=${#QS}
if [ ${n} -eq 2 ]
then
  # GENQBF=exec/genqbf # classic 1 quants
  GENQBF=src/expression/bin/genqbf_bingate # classic 1 quants
  TIME_GENQBF=$(time ${GENQBF} -I ${I} -R ${R} -J ${J} -S ${S} -P ${P} -k ${k} -F ${QS} -f qcir -o ${QCIR_OUT} -sem ${SEM} -n --fast -new "NN" )
else
  lst_NEW_QUANTS="AAE EAA EEA AEA EEE AEE AAAE EAAE AAAE AAEE EAAEE AAAEEE" #special cases we investigate
  if [[ $lst_NEW_QUANTS =~ (^|[[:space:]])${QS}($|[[:space:]]) ]]; then
    GENQBF=exec/genqbf_v5 # updated genqbf
    TIME_GENQBF=$(time ${GENQBF} -I ${I} -R ${R} -J ${J} -S ${S} -Q ${J} -W ${S} -Z ${J} -X ${S} -C ${J} -V ${S} -P ${P} -k ${k} -F ${QS}  -f qcir -o ${QCIR_OUT} -sem ${SEM} -n)
    # printf "-I ${I} -R ${R} -J ${J} -S ${S} -Q ${J} -W ${S} -Z ${J} -X ${S} -C ${J} -V ${S} -P ${P} -k ${k} -F ${QS}  -f qcir -o ${QCIR_OUT} -sem ${SEM} -n"
  else
    ALL_I_R=$(find ${OUTFOLDER}*.bool )
    GENQBF=src/cplusplus/genqbf # with arbitrary quantifiers
    TIME_GENQBF=$(time ${GENQBF} ${k} ${SEM} ${QS} ${ALL_I_R} ${FORMULA})
    time ${GENQBF} ${k} ${SEM} ${QS} ${ALL_I_R} ${P}
    # printf "-I ${I} -R ${R} -J ${J} -S ${S} -Q ${J} -W ${S} -Z ${J} -X ${S} -C ${J} -V ${S} -P ${P} -k ${k} -F ${QS}  -f qcir -o ${QCIR_OUT} -sem ${SEM} -n" 
  fi
fi


if [ ! -s ${QCIR_OUT} ]; then
        echo "(!) HyperQB error: .qcir file is empty. check if errors are reported."
        exit 1
fi
printf "QBF solving with QuAbS...\n"
# time ${QUABS}  --partial-assignment ${QCIR_OUT} 2>&1 | tee ${QUABS_OUT}
TIME_QUABS=$(time ${QUABS}  --partial-assignment ${QCIR_OUT} > ${QUABS_OUT})
OUTCOME=$(grep "r " ${QUABS_OUT})

# diskutil unmount /Volumes/RAMDisk




echo "------ Summary of HyperQB ------"
# echo "|  Models:     " ${MODELS[@]} 
printf '|  Model:       %s\n' "${MODELS[@]}"
echo "|  Formula:    " ${FORMULA}
echo "|  Quants:     " ${QS}
echo "|  QBFsolving: " ${OUTCOME}
echo "|  Semantics:  " ${SEM}
echo "|  #States:    " ${TIME_PARSE}
echo "|  Bound k:    " ${k}
echo "|  Mode:       " ${FLAG}
echo "--------------------------------"
echo "--------( HyperQB END )---------"

echo ""

# exit 1

# ## TODO: update these two scripts using python
echo "=== witness/counterexample ==="
echo "Given "${QS} "with" ${OUTCOME} "result: "
if grep -q UNSAT "${QUABS_OUT}"; 
then
  echo "no witness/counterexample."
else
  echo "witness/counterexample available!"
  # echo "parsing into well-formatted file..."
  ${MAP} ${QCIR_OUT} ${QUABS_OUT} ${MAP_OUT1} ${MAP_OUT2}
  ${PARSE_BOOL} ${MAP_OUT2} ${PARSE_OUT}
fi

# if [ ! -f "$QCIR_OUT" ]; then
#     echo "$QCIR_OUT does not exists"
#     exit 1
# fi
# echo "parsing into readable format..."
# ${MAP} ${QCIR_OUT} ${QUABS_OUT} ${MAP_OUT1} ${MAP_OUT2}
# ${PARSE_BOOL} ${MAP_OUT2} ${PARSE_OUT}














##### UNUSED
# time docker run --platform linux/amd64 -v ${PWD}:/mnt tzuhanmsu/hyperqube:latest /bin/bash -c "cd mnt/; python3 ${MULTI_PARSER} ${M1_NUSMVFILE} ${I} ${R} ${M2_NUSMVFILE} ${J} ${S} ${FORMULA} ${P} ${QSFILE} ${FLAG}; "
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

# I=${OUTFOLDER}I.bool
# R=${OUTFOLDER}R.bool
# J=${OUTFOLDER}J.bool
# S=${OUTFOLDER}S.bool

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
