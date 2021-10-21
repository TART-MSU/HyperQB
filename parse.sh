#!/bin/sh
#coding:utf-8
#coding:utf-8

echo "\n------( HyperQube START! )------\n"


SINGLE_PARSER=src/single_model_parser.py
MULTI_PARSER=src/multi_model_parser.py


## clean up previous generated
echo "(clean up previous generated files)"
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
    # PARSER_AND_TRANSLATOR=src/single_model_parser.py

    if [ ! -f "$NUSMVFILE" ]; then
        echo "error: $NUSMVFILE does not exist"
        exit 1
    fi

    if [ ! -f "$FORMULA" ]; then
        echo "error: $FORMULA does not exist"
        exit 1
    fi

    # if [  -f "${NUSMVFILE}" ]; then
    #   echo ${NUSMVFILE}
    #   echo "File " ${NUSMVFILE} "not found, please check the name of the file."
    #   exit 1
    # fi


    # PARSER_AND_TRANSLATOR=dist/parser/parser
    I=I.bool
    R=R.bool
    J=I.bool
    S=R.bool
    P=P.bool
     python3 ${SINGLE_PARSER} ${NUSMVFILE} ${FORMULA} ${I} ${R} ${P}
    # read QS from formula translation
    # if [ ! -f "QS.bool" ]; then
    #     exit 1
    # fi
    # source QS.bool
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
            # PARSER_AND_TRANSLATOR=src/single_model_parser.py
            # PARSER_AND_TRANSLATOR=dist/parser/parser

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
             python3 ${SINGLE_PARSER} ${NUSMVFILE} ${FORMULA} ${I} ${R} ${P} $FLAG
            # read QS from formula translation
            # if [ ! -f "QS.bool" ]; then
            #     exit 1
            # fi
            # source QS.bool
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
            # PARSER_AND_TRANSLATOR=src/single_model_parser.py
            # PARSER_AND_TRANSLATOR=dist/parser/parser
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
             python3 ${SINGLE_PARSER} ${NUSMVFILE} ${FORMULA} ${I} ${R} ${P} $FLAG
            # read QS from formula translation
            # if [ ! -f "QS.bool" ]; then
            #     exit 1
            # fi
            # source QS.bool
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
          # MULTI_PARSER_AND_TRANSLATOR=multi_model_parser.py
          ## check if file not found
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
           python3 ${MULTI_PARSER} ${M1_NUSMVFILE} ${I} ${R} ${M2_NUSMVFILE} ${J} ${S} ${FORMULA}  ${P}
          # read QS from formula translation
          # if [ ! -f "QS.bool" ]; then
          #     exit 1
          # fi
          # source QS.bool
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
            # MULTI_PARSER_AND_TRANSLATOR=multi_model_parser.py
            ## check if file not found
            if [ ! -f "$M1_NUSMVFILE" ]; then
                echo "error:$M1_NUSMVFILE does not exist"
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
            # read QS from formula translation
            # if [ ! -f "QS.bool" ]; then
            #     exit 1
            # fi
            # source QS.bool
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
            # MULTI_PARSER_AND_TRANSLATOR=multi_model_parser.py
            ## check if file not found
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
            # read QS from formula translation
            # if [ ! -f "QS.bool" ]; then
            #     exit 1
            # fi
            # source QS.bool
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
#      python3 ${MULTI_PARSER_AND_TRANSLATOR} ${M1_NUSMVFILE} ${I} ${R} ${M2_NUSMVFILE} ${J} ${S} ${FORMULA}  ${P}
#     # read QS from formula translation
#     source QS.bool
#     # echo $QS
# fi
