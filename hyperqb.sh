#!/bin/bash
TIMEFORMAT="%R"
# echo "(hyperqb with updated genqbf)"
echo "-------( HyperQB START! )-------"
PWD=$(pwd)
ALLARG=$@


##########################
### HyperQB parameters ###
##########################
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
  BINLOCATION="exec/linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
  BINLOCATION="exec/mac"
else
  echo "sorry, current OS not supported yet :("
  exit 1 
fi


###################
### executables ###
###################
ARBITRARY_PARSER=${BINLOCATION}/parser.py
GENQBF=${BINLOCATION}/genqbf_partialmulti # new - multigate
# GENQBF=src/expression/bin/genqbf # testing
# GENQBF=${BINLOCATION}/genqbf # new - multigate 
QUABS=${BINLOCATION}/quabs


#########################
### Output file names ###
#########################
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



######################################
### create output files containers ###
######################################
  if [ ! -d "${OUTFOLDER}" ]; then
      mkdir -p "${OUTFOLDER}"
      echo "Directory '${OUTFOLDER}' created."
  else
      rm -f -R ${OUTFOLDER}
      mkdir -p "${OUTFOLDER}"
  fi
  # if [ ! -d "${CEXFOLDER}" ]; then
  #     mkdir -p "${CEXFOLDER}"
  #     echo "Directory '${CEXFOLDER}' created."
  # else
  #     rm -f -R ${CEXFOLDER}
  #     mkdir -p "${CEXFOLDER}"
  # fi


#####################################
### fetch model(s) and HP formula ###
#####################################
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

######################################################
### Check which <mode> is used (-bughunt or -find) ###
######################################################
  if echo $* | grep -e "-find" -q
  then
    FLAG="-find"
  elif echo $* | grep -e "-bughunt" -q
  then
    FLAG="-bughunt"
  else
    echo "default to mode (-bughunt)"; FLAG="-bughunt"
  fi

###########################################################
### Check which <encoding> is used (-NN, -YY, -YN, -NY) ###
###########################################################
  ENCODING=""
  if echo $* | grep -e "-YY" -q
  then
    ENCODING="YY"
  elif echo $* | grep -e "-YN" -q
  then
    ENCODING="YN"
  elif echo $* | grep -e "-NY" -q
  then
    ENCODING="NY"  
  else
    ENCODING="NN" # default to encoding -NN 
  fi

############################################
### check which bunded semantics is used ###
############################################
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
    SEM="PES" # default to semantics -pes
  fi


####################################################
### parse the NuSMV models and the given formula ###
####################################################
# echo "(docker for stable parsing)"
echo -n "NuSMV and HyperLTL parsing..." 
PARSE_OUTCOME=$(docker run --rm --platform linux/amd64 -v ${PWD}:/mnt tzuhanmsu/hyperqube:latest /bin/bash -c "cd mnt/; TIMEFORMAT="%Rs"; python3 ${ARBITRARY_PARSER} ${OUTFOLDER} ${MODELS[*]} ${FORMULA} ${P} ${QSFILE} ${FLAG};")

TIME_PARSE=${PARSE_OUTCOME%:*}
echo ${TIME_PARSE}

STATENUM=${PARSE_OUTCOME#*:}

# echo "(local parsing)"
# TRANSLATE=${BINLOCATION}/"translate.py"
# python3 ${TRANSLATE} ${OUTFOLDER} ${MODELS[@]} ${FORMULA}  ${P} ${QSFILE} ${FLAG}
# echo "(local pip-built)"
# PARSE_OUTCOME=$(time python3 ${ARBITRARY_PARSER} ${OUTFOLDER} ${MODELS[@]} ${FORMULA} ${P} ${QSFILE} ${FLAG})
### if any error happens in parsing, exit HyperQB ###
if [[ "${PARSE_OUTCOME}" == *"$ERROR"* ]]; then
  echo ${PARSE_OUTCOME}
  exit 1
fi

##################################
### check quantifier selection ###
##################################
  if [ ! -f "${QSFILE}" ]; then
    echo ${ERROR} "no ${QSFILE} exists."
    echo "please check the formula see if the quantifers are corretly specified. "
    exit 1
  fi
  source "${QSFILE}" # instantiate QS

#####################
### QBF unrolling ###
#####################
echo -n "BMC unrolling with genqbf...."
n=${#QS}
if [ ${n} -eq 2 ]
then
  GENQBF_OUT=$(time ${GENQBF} -I ${I} -R ${R} -J ${J} -S ${S} -P ${P} -k ${k} -F ${QS} -f qcir -o ${QCIR_OUT} -sem ${SEM} -n --fast -new ${ENCODING})

elif [ ${n} -eq 5 ]
then
  Q=${OUTFOLDER}I_3.bool
  W=${OUTFOLDER}R_3.bool
  Z=${OUTFOLDER}I_4.bool
  X=${OUTFOLDER}R_4.bool
  C=${OUTFOLDER}I_5.bool
  V=${OUTFOLDER}R_5.bool
  GENQBF=${BINLOCATION}/genqbf_v5 # updated genqbf
  GENQBF_OUT=$(time ${GENQBF} -I ${I} -R ${R} -J ${J} -S ${S} -Q ${Q} -W ${W} -Z ${Z} -X ${X} -C ${C} -V ${V} -P ${P} -k ${k} -F ${QS}  -f qcir -o ${QCIR_OUT} -sem ${SEM} -n )
else
  lst_NEW_QUANTS="AAE EAA EEA AEA EEE AEE AAAE EAAE AAAE AAEE EAAEE AAAEEE" #special cases we investigate
  if [[ $lst_NEW_QUANTS =~ (^|[[:space:]])${QS}($|[[:space:]]) ]]; then
    GENQBF=${BINLOCATION}/genqbf_v5 # updated genqbf
    GENQBF_OUT=$(time ${GENQBF} -I ${I} -R ${R} -J ${J} -S ${S} -Q ${J} -W ${S} -Z ${J} -X ${S} -C ${J} -V ${S} -P ${P} -k ${k} -F ${QS}  -f qcir -o ${QCIR_OUT} -sem ${SEM} -n)
  else
    ALL_I_R=$(find ${OUTFOLDER}*.bool )
    GENQBF=src/cplusplus/genqbf # with arbitrary quantifiers
    GENQBF_OUT=$(time ${GENQBF} ${k} ${SEM} ${QS} ${ALL_I_R} ${FORMULA})
    time ${GENQBF} ${k} ${SEM} ${QS} ${ALL_I_R} ${P}
  fi
fi
TIME_GENQBF=${GENQBF_OUT#*': '}
TIME_GENQBF=${TIME_GENQBF%' secs'*}

if [ ! -s ${QCIR_OUT} ]; then
        echo ${ERROR} ".qcir file is empty, please check if genqbf error is reported."
        exit 1
fi
size=`du -k "$QCIR_OUT" | cut -f1` # extracting the size of QCIR 


###################
### QBF solving ###
###################
printf "QBF solving with QuAbS......."
START=$(date +%s.%N)
QUABS_OUTPUT=$((time ${QUABS} --partial-assignment ${QCIR_OUT}) > ${QUABS_OUT})
END=$(date +%s.%N)
TIME_QUABS=$(echo "$END - $START" | bc -l)
OUTCOME=$(grep "r " ${QUABS_OUT})

#############################
### print HyperQB summary ###
#############################
echo " ------ Summary of HyperQB ------"
printf '|  Model:       %s\n' "${MODELS[@]}"
echo   "|  Formula:    " ${FORMULA}
echo   "|  Quantifiers:" ${QS}
echo   "|  QCIR size:  " $size "KB"
echo   "|  QBF solving:" ${OUTCOME}
echo   "|  Mode:       " ${FLAG}
echo   "|  Semantics:  " ${SEM}
echo   "|  #States:    " ${STATENUM}
echo   "|  Bound k:    " ${k}
echo   "|  Encoding:   " ${ENCODING}
echo   " --------------------------------"
echo -n "TOTAL TIME: "
echo -n ${TIME_PARSE} + ${TIME_GENQBF} + ${TIME_QUABS} | bc | awk '{printf "%.3fs \n", $0}'
echo   " --------( HyperQB END )---------"
