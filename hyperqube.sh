#!/bin/sh
#coding:utf-8
#coding:utf-8

echo "\n------( HyperQube START! )------\n"


# install pynusmv
pip3 install pynusmv

## clean up previous generated
make clean

# if wrong number of arguments
if [ "$#" -ne 4 ] && [ "$#" -ne 5 ] && [ "$#" -ne 6 ]; then
  echo "HyperQube error: wrong number of arguments of HyperQube: \n"
  echo "- Simgle-model BMC: $0 {model}.smv {formula}.hq"
  echo "- Multi-model BMC:  $0 {model_1}.smv {model_2}.smv {formula}.hq \n"
  echo "\n------(END HyperQube)------\n"
  exit 1
fi


# SINGLE MODEL BMC
if [ "$#" -eq 4 ]; then
    MODE=single
    echo "received 4 arguments, perform single model BMC"
    echo "\n============ Parse SMV Model and HyperLTL Formula ============"
    NUSMVFILE=$1
    FORMULA=$2
    k=$3
    SEMANTICS=$4
    PARSER_AND_TRANSLATOR=single_model_parser.py
    # PARSER_AND_TRANSLATOR=dist/parser/parser
    I=I.bool
    R=R.bool
    J=I.bool
    S=R.bool
    P=P.bool
    time python3 ${PARSER_AND_TRANSLATOR} ${NUSMVFILE} ${FORMULA} ${I} ${R} ${P}
    # read QS from formula translation
    source QS.bool
    # echo $QS
fi


# SINGLE MODEL BMC with a flag or MULTIMODEL without a flag
if [ "$#" -eq 5 ] ; then
    FLAG=$5
    # echo "DEBBBBUUUUUUUUUUUUUUUUUG"
    echo $FLAG
    if  [ "$FLAG" = "-bughunt" ]; then
            MODE=single
            # echo "DEBBBBUUUUUUUUUUUUUUUUUG"
            echo "received 5 arguments with a -debug flag, doing single-model BMC in bughuntingmode."
            echo "(input formula is negated.)"
            echo "\n============ Parse SMV Model and HyperLTL Formula ============"
            NUSMVFILE=$1
            FORMULA=$2
            k=$3
            SEMANTICS=$4
            PARSER_AND_TRANSLATOR=single_model_parser.py
            # PARSER_AND_TRANSLATOR=dist/parser/parser
            I=I.bool
            R=R.bool
            J=I.bool
            S=R.bool
            P=P.bool
            time python3 ${PARSER_AND_TRANSLATOR} ${NUSMVFILE} ${FORMULA} ${I} ${R} ${P} $FLAG
            # read QS from formula translation
            source QS.bool
            # echo $QS
    elif  [ "$FLAG" = "-find" ]; then
            MODE=single
            # echo "DEBBBBUUUUUUUUUUUUUUUUUG"
            echo "received 5 arguments with a -find flag, doing single-model BMC in find mode."
            echo "(input formula is negated.)"
            echo "\n============ Parse SMV Model and HyperLTL Formula ============"
            NUSMVFILE=$1
            FORMULA=$2
            k=$3
            SEMANTICS=$4
            # PARSER_AND_TRANSLATOR=model_parser_and_formula_translator.py
            PARSER_AND_TRANSLATOR=single_model_parser.py
            # PARSER_AND_TRANSLATOR=dist/parser/parser
            I=I.bool
            R=R.bool
            J=I.bool
            S=R.bool
            P=P.bool
            time python3 ${PARSER_AND_TRANSLATOR} ${NUSMVFILE} ${FORMULA} ${I} ${R} ${P} $FLAG
            # read QS from formula translation
            source QS.bool
            # echo $QS
    else
      # MULTI-MODEL BMC
          echo "received 5 arguments, doing multi-model BMC"
          echo "\n============ Parse SMV Model and HyperLTL Formula ============"
          M1_NUSMVFILE=$1
          M2_NUSMVFILE=$2
          FORMULA=$3
          k=$4
          SEMANTICS=$5
          # SEM=-$4
          # echo $SEM
          MULTI_PARSER_AND_TRANSLATOR=multt_model_parser.py
          I=I.bool
          R=R.bool
          J=J.bool
          S=S.bool
          P=P.bool
          time python3 ${MULTI_PARSER_AND_TRANSLATOR} ${M1_NUSMVFILE} ${I} ${R} ${M2_NUSMVFILE} ${J} ${S} ${FORMULA}  ${P}
          # read QS from formula translation
          source QS.bool
          # echo $QS
    fi
fi




# MULTIMODEL with a flag
if [ "$#" -eq 6 ] ; then
    FLAG=$6
    # echo "DEBBBBUUUUUUUUUUUUUUUUUG"
    # echo $FLAG
    if  [ "$FLAG" = "-bughunt" ]; then
            MODE=multi
            echo "received 6 arguments, doing multi-model BMC"
            echo "\n============ Parse SMV Model and HyperLTL Formula ============"
            M1_NUSMVFILE=$1
            M2_NUSMVFILE=$2
            FORMULA=$3
            k=$4
            SEMANTICS=$5
            # SEM=-$4
            # echo $SEM
            MULTI_PARSER_AND_TRANSLATOR=multt_model_parser.py
            I=I.bool
            R=R.bool
            J=J.bool
            S=S.bool
            P=P.bool
            time python3 ${MULTI_PARSER_AND_TRANSLATOR} ${M1_NUSMVFILE} ${I} ${R} ${M2_NUSMVFILE} ${J} ${S} ${FORMULA}  ${P} ${FLAG}
            # read QS from formula translation
            source QS.bool
            # echo $QS
            # echo $QS
    elif  [ "$FLAG" = "-find" ]; then
            MODE=multi
            echo "received 6 arguments, doing multi-model BMC"
            echo "\n============ Parse SMV Model and HyperLTL Formula ============"
            M1_NUSMVFILE=$1
            M2_NUSMVFILE=$2
            FORMULA=$3
            k=$4
            SEMANTICS=$5
            # SEM=-$4
            # echo $SEM
            MULTI_PARSER_AND_TRANSLATOR=multimodel_parser_and_formula_translator.py
            I=I.bool
            R=R.bool
            J=J.bool
            S=S.bool
            P=P.bool
            time python3 ${MULTI_PARSER_AND_TRANSLATOR} ${M1_NUSMVFILE} ${I} ${R} ${M2_NUSMVFILE} ${J} ${S} ${FORMULA}  ${P} ${FLAG}
            # read QS from formula translation
            source QS.bool
            # echo $QS
    fi
fi

# # MULTI-MODEL BMC
# if [ "$#" -eq 5 ]; then
#     echo "received 5 arguments, doing multi-model BMC"
#     echo "\n============ Parse SMV Model and HyperLTL Formula ============"
#     M1_NUSMVFILE=$1
#     M2_NUSMVFILE=$2
#     FORMULA=$3
#     k=$4
#     SEMANTICS=$5
#     # SEM=-$4
#     # echo $SEM
#     MULTI_PARSER_AND_TRANSLATOR=multimodel_parser_and_formula_translator.py
#     I=I.bool
#     R=R.bool
#     J=J.bool
#     S=S.bool
#     P=P.bool
#     time python3 ${MULTI_PARSER_AND_TRANSLATOR} ${M1_NUSMVFILE} ${I} ${R} ${M2_NUSMVFILE} ${J} ${S} ${FORMULA}  ${P}
#     # read QS from formula translation
#     source QS.bool
#     # echo $QS
# fi






# Syntax check SEMANTICS
if [ "$SEMANTICS" = "pes" ]; then
  SEM="PES"
  echo $SEM
elif [ "$SEMANTICS" = "opt" ]; then
  SEM="OPT"
  echo $SEM
elif [ "$SEMANTICS" = "hpes" ]; then
  SEM="TER_PES"
  echo $SEM
elif [ "$SEMANTICS" = "hopt" ]; then
  SEM="TER_OPT"
  echo $SEM
else
  echo "HyperQube error: incorrect semantic input. "
  echo " use { pes | opt | hpes | hopt } semantics of the unrolling from one of the follows:"
  echo "             (pessimistic / optimistic / halting-pessimistic / halting-optimistic)"
  exit 1
fi




# HyperQube subTools
GENQBF=exec/genqbf
QUABS=exec/quabs


# output files
QCIR_OUT=HQ.qcir
QUABS_OUT=HQ.quabs



# simple mapping of variable names and values
# TODO: do it in python
MAP=util/util_mapvars.java
MAP_OUT1=OUTPUT_byName.cex
MAP_OUT2=OUTPUT_byTime.cex
# simple parser for convering binaries to digits
PARSE_BOOL=util/util_parsebools.java
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
time ${GENQBF} -I ${I} -R ${R} -J ${J} -S ${S} -P ${P} -k ${k} -F ${QS}  -f qcir -o ${QCIR_OUT} -sem ${SEM} -n --fast



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
# echo  "(under condtruction...)"
# python3 ${PARSE_OUTPUT} ${MAP_OUT2} ${PARSE_OUT} ${k}
# #by time

echo "\n------(END HyperQube)------\n"
