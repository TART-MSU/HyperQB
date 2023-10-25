#!/bin/bash
TIMEFORMAT="%Rs"
echo "(hyperqb with updated genqbf)"

### HyperQB parameters ###
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
  BINLOCATION="exec_linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
  BINLOCATION="exec_mac"
else
  echo "sorry, current OS not supported yet :("
  exit 1 
fi

### executables
ARBITRARY_PARSER=${BINLOCATION}/parser.py
GENQBF=${BINLOCATION}/genqbf_partialmulti # new - multigate 
QUABS=${BINLOCATION}/quabs

### Output file names ###
DATE="today"
OUTFOLDER="build_"${DATE}"/"
CEXFOLDER="build_cex/"
MAP_OUT1=${OUTFOLDER}_byName.cex
MAP_OUT2=${OUTFOLDER}_byTime.cex
PARSE_OUT=${CEXFOLDER}formatted_2.0.cex
I=${OUTFOLDER}I_1.bool
R=${OUTFOLDER}R_1.bool
J=${OUTFOLDER}I_2.bool
S=${OUTFOLDER}R_2.bool
P=${OUTFOLDER}P.hq
QSFILE=${OUTFOLDER}QS
FORMULA=""
QUABS_OUT=${OUTFOLDER}HQ.quabs
QCIR_OUT=${OUTFOLDER}HQ.qcir
ERROR="(!) HyperQB error: "


if [ ! -d "${OUTFOLDER}" ]; then
    mkdir -p "${OUTFOLDER}"
    echo "Directory '${OUTFOLDER}' created."
else
    # echo "Directory '${OUTFOLDER}' already exists."
    rm -f -R ${OUTFOLDER}
    mkdir -p "${OUTFOLDER}"
fi
if [ ! -d "${CEXFOLDER}" ]; then
    mkdir -p "${CEXFOLDER}"
    echo "Directory '${CEXFOLDER}' created."
else
    # echo "Directory '${CEXFOLDER}' already exists."
    rm -f -R ${CEXFOLDER}
    mkdir -p "${CEXFOLDER}"
fi


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
  echo "default to mode (-bughunt)"; FLAG="-bughunt"
fi


### check which bunded semantics is used ###
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
  echo ${ERROR} "incorrect semantic input."
  echo "please use { -pes | -opt | -hpes | -hopt } semantics of the unrolling."
  exit 1
fi


### parse the NuSMV models and the given formula ###
printf "NuSMV and HyperLTL parsing...\n" 

echo "(docker for stable parsing)"
TIME_PARSE=$(docker run --platform linux/amd64 -v ${PWD}:/mnt tzuhanmsu/hyperqube:latest /bin/bash -c "cd mnt/; TIMEFORMAT="%Rs"; time python3 ${ARBITRARY_PARSER} ${OUTFOLDER} ${MODELS[*]} ${FORMULA} ${P} ${QSFILE} ${FLAG}; ")

# echo "(local parsing)"
# TRANSLATE="exec/translate.py"
# python3 ${TRANSLATE} ${OUTFOLDER} ${MODELS[@]} ${FORMULA}  ${P} ${QSFILE} ${FLAG}
# echo "(local pip-built)"
# TIME_PARSE=$(time python3 ${ARBITRARY_PARSER} ${OUTFOLDER} ${MODELS[@]} ${FORMULA} ${P} ${QSFILE} ${FLAG})


### if any error happens in parsing, exit HyperQB
if [[ "${TIME_PARSE}" == *"$ERROR"* ]]; then
  echo ${TIME_PARSE}
  exit 1
fi

### check what is the quantifier selection
if [ ! -f "${QSFILE}" ]; then
  echo ${ERROR} "no ${QSFILE} exists."
  echo "please check the formula see if the quantifers are corretly specified. "
  exit 1
fi
source "${QSFILE}" # instantiate QS

printf "BMC unrolling with genqbf...\n"
n=${#QS}
if [ ${n} -eq 2 ]
then
  TIME_GENQBF=$(time ${GENQBF} -I ${I} -R ${R} -J ${J} -S ${S} -P ${P} -k ${k} -F ${QS} -f qcir -o ${QCIR_OUT} -sem ${SEM} -n --fast -new "NN" )
elif [ ${n} -eq 5 ]
then
  Q=${OUTFOLDER}I_3.bool
  W=${OUTFOLDER}R_3.bool
  Z=${OUTFOLDER}I_4.bool
  X=${OUTFOLDER}R_4.bool
  C=${OUTFOLDER}I_5.bool
  V=${OUTFOLDER}R_5.bool
  GENQBF=${BINLOCATION}/genqbf_v5 # updated genqbf
  TIME_GENQBF=$(time ${GENQBF} -I ${I} -R ${R} -J ${J} -S ${S} -Q ${Q} -W ${W} -Z ${Z} -X ${X} -C ${C} -V ${V} -P ${P} -k ${k} -F ${QS}  -f qcir -o ${QCIR_OUT} -sem ${SEM} -n )
else
  lst_NEW_QUANTS="AAE EAA EEA AEA EEE AEE AAAE EAAE AAAE AAEE EAAEE AAAEEE" #special cases we investigate
  if [[ $lst_NEW_QUANTS =~ (^|[[:space:]])${QS}($|[[:space:]]) ]]; then
    GENQBF=${BINLOCATION}/genqbf_v5 # updated genqbf
    TIME_GENQBF=$(time ${GENQBF} -I ${I} -R ${R} -J ${J} -S ${S} -Q ${J} -W ${S} -Z ${J} -X ${S} -C ${J} -V ${S} -P ${P} -k ${k} -F ${QS}  -f qcir -o ${QCIR_OUT} -sem ${SEM} -n)
  else
    ALL_I_R=$(find ${OUTFOLDER}*.bool )
    GENQBF=src/cplusplus/genqbf # with arbitrary quantifiers
    TIME_GENQBF=$(time ${GENQBF} ${k} ${SEM} ${QS} ${ALL_I_R} ${FORMULA})
    time ${GENQBF} ${k} ${SEM} ${QS} ${ALL_I_R} ${P}
  fi
fi


if [ ! -s ${QCIR_OUT} ]; then
        echo ${ERROR} ".qcir file is empty, please check if genqbf error is reported."
        exit 1
fi

printf "QBF solving with QuAbS...\n"
TIME_QUABS=$(time ${QUABS}  --partial-assignment ${QCIR_OUT} > ${QUABS_OUT})
OUTCOME=$(grep "r " ${QUABS_OUT})

echo "------ Summary of HyperQB ------"
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