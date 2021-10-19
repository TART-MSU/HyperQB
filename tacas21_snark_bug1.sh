#!/bin/sh

# HyperQube
GENQBF=./genqbf
QUABS=./quabs

#### TACAS21 BMC PAPER CASES ###

# # ### SNARK
# NUSMVFILE="./tacas21_cases/snark.smv"
# FORMULA="./tacas21_cases/snark_formula.hq"
# QS=EE 	# quantifier selections
# k=5		# bound of traces


# # ### TEST_SNARK
# NUSMVFILE="./tacas21_cases/snark.smv"
# FORMULA="./tacas21_cases/snark_formula.hq"
# QS=EE 	# quantifier selections
# k=10		# bound of traces
# NUSMVFILE="./tacas21_cases/snark_overflow.smv"
# FORMULA="./tacas21_cases/snark_formula.hq"
# QS=EE 	# quantifier selections
# k=3		# bound of traces


# ### SNARK_concurrent
M1_NUSMVFILE="./tacas21_cases/snark1_M1_concurrent.smv"
# FORMULA="./tacas21_cases/snark_formula.hq"
# QS=EE 	# quantifier selections
# k=2		# bound of traces


# ### SNARK_sequential
M2_NUSMVFILE="./tacas21_cases/snark1_M2_sequential.smv"



FORMULA="./tacas21_cases/snark1_formula.hq"
# QS=AE	# quantifier selections
k=18		# bound of traces

M1_PARSED_MODEL_I=SNARK_M1_I.bool
M1_PARSED_MODEL_R=SNARK_M1_R.bool

M2_PARSED_MODEL_I=SNARK_M2_I.bool
M2_PARSED_MODEL_R=SNARK_M2_R.bool

TRANSLATED_FORMULA=SNARK_linearizability.bool

# G(history <-> history)
#G((proc1.popRightSTART[A] <-> proc1.popRightSTART[B]) /\ (proc1.popRightEND[A] <-> proc1.popRightEND[B]) /\ (proc1.pushRightSTART[A] <-> proc1.pushRightSTART[B]) /\ (proc1.pushRightEND[A] <-> proc1.pushRightEND[B]) /\ (proc1.popLeftSTART[A] <-> proc1.popLeftSTART[B]) /\  (proc1.popLeftEND[A] <-> proc1.popLeftEND[B]) /\ (proc2.popRightSTART[A] <-> proc2.popRightSTART[B]) /\ (proc2.popRightEND[A] <-> proc2.popRightEND[B]) /\ (proc2.pushRightSTART[A] <-> proc2.pushRightSTART[B]) /\ (proc2.pushRightEND[A] <-> proc2.pushRightEND[B]) /\ (proc2.popLeftSTART[A] <-> proc2.popLeftSTART[B]) /\  (proc2.popLeftEND[A] <-> proc2.popLeftEND[B]))/\ G(((proc1.popRightEND[A] <-> proc1.popRightEND[B]) \/ (proc1.popLeftEND[A] <-> proc1.popLeftEND[B]) \/ (proc2.popLeftEND[A] <-> proc2.popLeftEND[B]))-> ( (GlobalQueue-popRightFAIL[A] <-> GlobalQueue-popRightFAIL[B]) /\ (*AllNodes[0][0][A] = AllNodes[0][0][B]*)))


# test_
# forall A. exists B. F(GlobalQueue-popRightFAIL[B] /\ *RightHat[B]=2*)

I=${M1_PARSED_MODEL_I}
R=${M1_PARSED_MODEL_R}
J=${M2_PARSED_MODEL_I}
S=${M2_PARSED_MODEL_R}

P=${TRANSLATED_FORMULA}

# output files
QCIR_OUT=test.qcir
QUABS_OUT=test.quabs

# simple mapping of variable names and values
# TODO: do it in python
MAP=util/util_mapvars.java
MAP_OUT1=mapping_byName.cex
MAP_OUT2=mapping_byTime.cex

PARSE_BOOL=util/util_parsebools.java
PARSE_OUT=formatted.cex

# global instances construction
# GLOB=glob.py

# PARSE_FORMULA=parse_formula.py
# FORMULA=formula.hq
MODEL_PARSER=model_parser.py
FORMULA_TRANSLATOR=formula_translator.py
PARSER_AND_TRANSLATOR=model_parser_and_formula_translator.py
# MULTI_PARSER_AND_TRANSLATOR=multimodel_parser_and_formula_translator.py
MULTI_PARSER_AND_TRANSLATOR=multi_model_parser.py

# make clean
# PARSE=util/util_parsebools.java
PARSE_OUTPUT=parse_output.py
PARSE_OUT=OUTPUT.cex


echo "\n------(HyperQube START)------\n"
# echo "QS:" ${QS} "k=" ${k}
# echo "Model:            " ${NUSMVFILE}
# echo "HyperLTL formula: " ${FORMULA}


echo "\n============ Parse SMV Model and HyperLTL Formula ============"
make clean

# M1_smv_file_name = sys.argv[1]
# M1_parsed_madel_file_I_name = sys.argv[2]
# M1_parsed_madel_file_R_name = sys.argv[3]
# M2_smv_file_name = sys.argv[4]
# M2_parsed_madel_file_I_name = sys.argv[5]
# M2_parsed_madel_file_R_name = sys.argv[6]
# fomula_file_name = sys.argv[7]
# translated_formula_file_name = sys.argv[8]

time python3 ${MULTI_PARSER_AND_TRANSLATOR} ${M1_NUSMVFILE} ${M1_PARSED_MODEL_I} ${M1_PARSED_MODEL_R} ${M2_NUSMVFILE} ${M2_PARSED_MODEL_I} ${M2_PARSED_MODEL_R} ${FORMULA}  ${TRANSLATED_FORMULA}
# read QS from formula translation
# To improve: embedded QS into genQBF
source QS.bool
# echo $QS

echo "\n============ Model Checking Info ============"

echo "Quantifiers:" ${QS} "  bound k=" ${k}
echo "Model:            " ${NUSMVFILE}
echo "HyperLTL formula: " ${FORMULA}


#
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
echo  "(python version formatter under construction)"
# python3 ${PARSE_OUTPUT} ${MAP_OUT2} ${PARSE_OUT} ${k}
# #by time

echo "\n------(END HyperQube)------\n"
