#!/bin/sh

# HyperQube
GENQBF=./genqbf
QUABS=./quabs

#### TACAS21 BMC PAPER CASES ###

## simple krip
NUSMVFILE="./tacas21_cases/simple_krip.smv"
FORMULA="./tacas21_cases/simple_krip_formula.hq"
QS=EE 	# quantifier selections
k=5		# bound of traces

### BAKERY

# ### BAKERY 3PROC S1
# NUSMVFILE="./tacas21_cases/bakery_3procs.smv"
# FORMULA="./tacas21_cases/bakery_formula_S1_3proc.hq"
# QS=EE 	# quantifier selections
# k=10		# bound of traces

### BAKERY 3PROC S2
# NUSMVFILE="./tacas21_cases/bakery_3procs.smv"
# FORMULA="./tacas21_cases/bakery_formula_S2_3proc.hq"
# QS=EE 	# quantifier selections
# k=10		# bound of traces

# ### BAKERY 3PROC S3
# NUSMVFILE="./tacas21_cases/bakery_3procs.smv"
# FORMULA="./tacas21_cases/bakery_formula_S3_3proc.hq"
# QS=EE 	# quantifier selections
# k=10		# bound of traces

# ### BAKERY 3PROC SYM1
# NUSMVFILE="./tacas21_cases/bakery_3procs.smv"
# FORMULA="./tacas21_cases/bakery_formula_sym1_3proc.hq"
# QS=EA 	# quantifier selections
# k=10		# bound of traces

# ### BAKERY 3PROC SYM2
# NUSMVFILE="./tacas21_cases/bakery_3procs.smv"
# FORMULA="./tacas21_cases/bakery_formula_sym2_3proc.hq"
# QS=EA 	# quantifier selections
# k=10		# bound of traces


# ### BAKERY 5PROC SYM1
# NUSMVFILE="./tacas21_cases/bakery_5procs.smv"
# FORMULA="./tacas21_cases/bakery_formula_sym1_5proc.hq"
# QS=EA 	# quantifier selections
# k=10		# bound of traces


# ### BAKERY 5PROC SYM2
# NUSMVFILE="./tacas21_cases/bakery_5procs.smv"
# FORMULA="./tacas21_cases/bakery_formula_sym2_5proc.hq"
# QS=EA 	# quantifier selections
# k=10		# bound of traces




# ### NRP_incorrect
# NUSMVFILE="./tacas21_cases/NRP_incorrect.smv"
# FORMULA="./tacas21_cases/NRP_formula.hq"
# QS=EA 	# quantifier selections
# k=15		# bound of traces
# ## SAT

### NRP_correct
# NUSMVFILE="./tacas21_cases/NRP_correct.smv"
# FORMULA="./tacas21_cases/NRP_formula.hq"
# QS=EA 	# quantifier selections
# k=15		# bound of traces
## UNSAT



# ### NI_incorrect
# NUSMVFILE="./tacas21_cases/NI_incorrect.smv"
# FORMULA="./tacas21_cases/NI_formula.hq"
# QS=AE 	# quantifier selections
# k=50		# bound of traces
### UNSAT, counterexample

### NI_correct
# NUSMVFILE="./tacas21_cases/NI_correct.smv"
# FORMULA="./tacas21_cases/NI_formula.hq"
# QS=AE 	# quantifier selections
# k=50		# bound of traces
# ## SAT

# # ### SNARK-- see tacas21_snark_bug1.sh and  tacas21_snark_bug2.sh



### ROBITIC: write program to generate SMV models
# NUSMVFILE="./tacas21_cases/robotic_test.smv"

## Shortest Path 10x10
# NUSMVFILE="./tacas21_cases/robotic_sp_100.smv"
# FORMULA="./tacas21_cases/robotic_sp_formula.hq"
# # QS=EA 	# quantifier selections
# k=20		# bound of traces
# (~GOAL[B]) U (GOAL[A])

# ## Shortest Path 20x20
# NUSMVFILE="./tacas21_cases/robotic_sp_400.smv"
# FORMULA="./tacas21_cases/robotic_sp_formula.hq"
# # QS=EA 	# quantifier selections
# k=40		# bound of traces

# ## Shortest Path 40x40
# NUSMVFILE="./tacas21_cases/robotic_sp_1600.smv"
# FORMULA="./tacas21_cases/robotic_sp_formula.hq"
# # QS=EA 	# quantifier selections
# k=80		# bound of traces

# ## Shortest Path 60x60 ------ ???
# NUSMVFILE="./tacas21_cases/robotic_sp_3600.smv"
# FORMULA="./tacas21_cases/robotic_sp_formula.hq"
# # QS=EA 	# quantifier selections
# k=120		# bound of traces



### ROBUSTNESS 10x10
# NUSMVFILE="./tacas21_cases/robotic_robustness_100.smv"
# FORMULA="./tacas21_cases/robotic_robustness_formula.hq"
# # QS=EA 	# quantifier selections
# k=20		# bound of traces

# ### ROBUSTNESS 20x20
# NUSMVFILE="./tacas21_cases/robotic_robustness_400.smv"
# FORMULA="./tacas21_cases/robotic_robustness_formula.hq"
# # QS=EA 	# quantifier selections
# k=40		# bound of traces

# ### ROBUSTNESS 40x40
# NUSMVFILE="./tacas21_cases/robotic_robustness_1600.smv"
# FORMULA="./tacas21_cases/robotic_robustness_formula.hq"
# # QS=EA 	# quantifier selections
# k=80		# bound of traces

# ### ROBUSTNESS 60x60
# NUSMVFILE="./tacas21_cases/robotic_robustness_3600.smv"
# FORMULA="./tacas21_cases/robotic_robustness_formula.hq"
# # QS=EA 	# quantifier selections
# k=120		# bound of traces


### mutation testing
NUSMVFILE="./tacas21_cases/mutation_testing.smv"
FORMULA="./tacas21_cases/mutation_testing.hq"
k=10


## find . -name "*.cex" -delete


I=test_I.bool
R=test_R.bool
J=test_I.bool
S=test_R.bool

P=test_P.bool


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
# PARSER_AND_TRANSLATOR=model_parser_and_formula_translator.py
PARSER_AND_TRANSLATOR=single_model_parser.py


# make clean
# PARSE=util/util_parsebools.java
PARSE_OUTPUT=parse_output.py
PARSE_OUT=OUTPUT.cex


echo "\n------(HyperQube START)------\n"

echo "\n============ Parse SMV Model and HyperLTL Formula ============"
make clean
# python3 ${MODEL_PARSER} ${NUSMVFILE}
# python3 ${FORMULA_TRANSLATOR} ${NUSMVFILE} ${FORMULA} ${k}
time python3 ${PARSER_AND_TRANSLATOR} ${NUSMVFILE} ${FORMULA} ${I} ${R} ${P}

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
