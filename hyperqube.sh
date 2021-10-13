#!/bin/sh


# echo "First arg: $1"
# echo "Second arg: $2"

echo "\n------(HyperQube START)------\n"

# if wrong number of arguments
if [ "$#" -ne 3 ] && [ "$#" -ne 4 ]; then
  echo "HyperQube error: wrong number of arguments of HyperQube: \n"
  echo "- Simgle-model BMC: $0 {model}.smv {formula}.hq"
  echo "- Multi-model BMC:  $0 {model_1}.smv {model_2}.smv {formula}.hq \n"
  echo "\n------(END HyperQube)------\n"
  exit 1
fi



# single model BMC
if [ "$#" -eq 3 ]; then
    echo "received 3 arguments, perform single model BMC"
    NUSMVFILE=$1
    FORMULA=$2
    k=$3
    # PARSER_AND_TRANSLATOR=model_parser_and_formula_translator.py
    PARSER_AND_TRANSLATOR=dist/parser/parser

    I=I.bool
    R=R.bool
    J=I.bool
    S=R.bool
    P=P.bool


    time python3 ${PARSER_AND_TRANSLATOR} ${NUSMVFILE} ${FORMULA} ${I} ${R} ${P}
fi


#multimodel BMC
if [ "$#" -eq 4 ]; then
    echo "received 4 arguments, doing multi-model BMC"
    M1_NUSMVFILE=$1
    M2_NUSMVFILE=$2
    echo "First arg: $1"
    echo "Second arg: $2"
    FORMULA=$3
    k=$4
    MULTI_PARSER_AND_TRANSLATOR=multimodel_parser_and_formula_translator.py

    I=I.bool
    R=R.bool
    J=J.bool
    S=S.bool
    P=P.bool

    time python3 ${MULTI_PARSER_AND_TRANSLATOR} ${M1_NUSMVFILE} ${I} ${R} ${M2_NUSMVFILE} ${J} ${S} ${FORMULA}  ${P}
fi



# HyperQube
GENQBF=./genqbf
QUABS=./quabs

# output files
QCIR_OUT=HQ.qcir
QUABS_OUT=HQ.quabs

# simple mapping of variable names and values
# TODO: do it in python
MAP=util/util_mapvars.java
MAP_OUT1=OUTPUT_byName.cex
MAP_OUT2=OUTPUT_byTime.cex

PARSE_BOOL=util/util_parsebools.java
PARSE_OUT=OUTPUT_formatted.cex



# make clean
# PARSE=util/util_parsebools.java
# PARSE_OUTPUT=parse_output.py
# PARSE_OUT=OUTPUT.cex


echo "\n============ Parse SMV Model and HyperLTL Formula ============"
make clean
# read QS from formula translation
# To improve: embedded QS into genQBF
source QS.bool
# echo $QS

echo "\n============ Model Checking Info ============"

echo "Quantifiers:" ${QS} "  bound k=" ${k}
echo "Model:            " ${NUSMVFILE}
echo "HyperLTL formula: " ${FORMULA}


echo "\n============ Unrolling with genQBF + Solving with QuAbS ============"
# echo "---Generating QCIR---"
echo "generating QBF BMC..."
time ${GENQBF} -I ${I} -R ${R} -J ${J} -S ${S} -P ${P} -k ${k} -F ${QS}  -f qcir -o ${QCIR_OUT} -sem PES -n --fast



# echo "---QUABS solving---"
echo "solving QBF..."
time  ${QUABS}  --partial-assignment ${QCIR_OUT} 2>&1 | tee ${QUABS_OUT}
# time ${QUABS} --statistics --preprocessing 0 --partial-assignment ${QCIR_OUT} 2>&1 | tee ${QUABS_OUT}

echo "parsing into readable format..."
# # echo "---Counterexample Mapping---"
java ${MAP} ${QCIR_OUT} ${QUABS_OUT} ${MAP_OUT1} ${MAP_OUT2}

# echo "---Parse All Binary Numbers---"
echo "\n============ Get Nice-formatted Output ============"
java ${PARSE_BOOL} ${MAP_OUT2} ${PARSE_OUT}
echo  "(under condtruction...)"
# python3 ${PARSE_OUTPUT} ${MAP_OUT2} ${PARSE_OUT} ${k}
# #by time

echo "\n------(END HyperQube)------\n"
