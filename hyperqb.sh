#!/bin/bash
TIMEFORMAT="%Rs"
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

ERROR="(!) HyperQB error"

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
I=${OUTFOLDER}I_1.bool
R=${OUTFOLDER}R_1.bool
J=${OUTFOLDER}I_2.bool
S=${OUTFOLDER}R_2.bool
Q=${OUTFOLDER}I_3.bool
W=${OUTFOLDER}R_3.bool
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
echo "------( HyperQB START! )------"
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

printf "NuSMV models and HyperLTL formula parsing..."
# RUN PARSER on Docker
# echo "(using docker for parsing, parsing could become slower)"
# TIME_PARSE=$(docker run --platform linux/amd64 -v ${PWD}:/mnt tzuhanmsu/hyperqube:latest /bin/bash -c "cd mnt/; TIMEFORMAT="%Rs"; time python3 ${ARBITRARY_PARSER} ${OUTFOLDER} ${MODELS[@]} ${FORMULA} ${P} ${QSFILE} ${FLAG}; ")

# RUN PARSER locally, if the setup on your local machine is successful
TIME_PARSE=$(time python3 ${ARBITRARY_PARSER} ${OUTFOLDER} ${MODELS[@]} ${FORMULA} ${P} ${QSFILE} ${FLAG})


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




printf "BMC unrolling with genqbf..................."
QCIR_OUT=${OUTFOLDER}HQ.qcir
n=${#QS}
if [ ${n} -eq 2 ]
then
  GENQBF=exec/genqbf # classic 1 quants
  TIME_GENQBF=$(time ${GENQBF} -I ${I} -R ${R} -J ${J} -S ${S} -P ${P} -k ${k} -F ${QS} -f qcir -o ${QCIR_OUT} -sem ${SEM} -n --fast)
else
  lst_NEW_QUANTS="AAE EAA EEA AEA EEE AEE AAAE EAAE AAAE AAEE EAAEE AAAEEE" #special cases we investigate

  if [[ $lst_NEW_QUANTS =~ (^|[[:space:]])${QS}($|[[:space:]]) ]]; then
    GENQBF=exec/genqbf_v5 # updated genqbf
    TIME_GENQBF=$(time ${GENQBF} -I ${I} -R ${R} -J ${J} -S ${S} -Q ${Q} -W ${W} -Z ${J} -X ${S} -C ${Q} -V ${W} -P ${P} -k ${k} -F ${QS}  -f qcir -o ${QCIR_OUT} -sem ${SEM} -n)
  else
    ALL_I_R=$(find ${OUTFOLDER}*.bool )
    GENQBF=src/cplusplus/genqbf # with arbitrary quantifiers
    # TIME_GENQBF=$(time ${GENQBF} ${k} ${SEM} ${QS} ${ALL_I_R} ${FORMULA})
    time ${GENQBF} ${k} ${SEM} ${QS} ${ALL_I_R} ${P}
  fi
fi


if [ ! -s ${QCIR_OUT} ]; then
        echo "(!) HyperQB error: .qcir file is empty. check if errors are reported."
        exit 1
fi
printf "QBF solving with QuAbS......................"
# time ${QUABS}  --partial-assignment ${QCIR_OUT} 2>&1 | tee ${QUABS_OUT}
TIME_QUABS=$(time ${QUABS}  --partial-assignment ${QCIR_OUT} > ${QUABS_OUT})
OUTCOME=$(grep "r " ${QUABS_OUT})



echo "--------------- Summary of HyperQB ---------------"
echo "|  Models:     " ${MODELS[*]}
echo "|  Formula:    " ${FORMULA}
echo "|  Quantifiers:" ${QS}
echo "|  QBF solving:" ${OUTCOME}
echo "|  Semantics:  " ${SEM}
echo "|  #states:    " ${TIME_PARSE}
echo "|  Bound k:    " ${k}
echo "|  Mode:       " ${FLAG}
echo "----------------------------------------------------"
echo "------(END HyperQB)------"

echo ""
echo ""




# ## TODO: update these two scripts using python
# echo "(Get Nice-formatted Output if witness/counterexample is found)"
# if [ ! -f "$QCIR_OUT" ]; then
#     echo "$QCIR_OUT does not exists"
#     exit 1
#     echo "no QCIR output"
# fi
# echo "parsing into readable format..."
# ${MAP} ${QCIR_OUT} ${QUABS_OUT} ${MAP_OUT1} ${MAP_OUT2}
# ${PARSE_BOOL} ${MAP_OUT2} ${PARSE_OUT}
