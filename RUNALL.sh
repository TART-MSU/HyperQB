#!/bin/sh

# RUNALL executs all emperical results in table 4 and  table 5, including
# (1) All experimental results presented on Table 4;
#	(2) All experimental results presented on Table 5; and
#	(3) All experimental results from cited paper [26].

# # repare for a new report
# rm REPORT.txt
# ## get current location
# PWD=$(pwd)
#
# rm alllog.txt
# touch alllog.txt
#
# ## case 1.1
# ALLARG="cases/bakery_3procs.smv cases/bakery_formula_sym1_3proc.hq 10 pes"
# ## execute python scripts on docker
# (time docker run -v ${PWD}:/mnt tzuhanmsu/hyperqube:latest /bin/bash -c "cd mnt/; ./parse.sh ${ALLARG}; ") 2>&1 | tee -a outlog.txt
# # (time docker run -v ${PWD}:/mnt tzuhanmsu/hyperqube:latest /bin/bash -c "cd mnt/; ./parse.sh ${ALLARG}; ") &>> outlog.txt
# ## run BMC
# (./bmc.sh ${ALLARG}) 2>&1 | tee -a outlog.txt
# # (./bmc.sh ${ALLARG}) &>> outlog.txt
# ## get nice formatted  output
# echo "\n-------------------- Summary of Case 1.1 ---------------------" 2>&1 | tee -a REPORT.txt
# python3 extract_log.py 2>&1 | tee -a REPORT.txt
#
# cat outlog.txt >> alllog.txt
# rm outlog.txt
#
#
# ## case 1.2
# ALLARG="cases/bakery_3procs.smv cases/bakery_formula_sym2_3proc.hq 10 pes"
# ## execute python scripts on docker
# (time docker run -v ${PWD}:/mnt tzuhanmsu/hyperqube:latest /bin/bash -c "cd mnt/; ./parse.sh ${ALLARG}; ") 2>&1 | tee -a outlog.txt
# # (time docker run -v ${PWD}:/mnt tzuhanmsu/hyperqube:latest /bin/bash -c "cd mnt/; ./parse.sh ${ALLARG}; ") &>> outlog.txt
# ## run BMC
# (./bmc.sh ${ALLARG}) 2>&1 | tee -a outlog.txt
# # (./bmc.sh ${ALLARG}) &>> outlog.txt
# ## get nice formatted  output
# echo "\n-------------------- Summary of Case 1.2 ---------------------" 2>&1 | tee -a REPORT.txt
# python3 extract_log.py 2>&1 | tee -a REPORT.txt
#
# cat outlog.txt >> alllog.txt
# rm outlog.txt

(time -p sleep 30) 2>&1 | tee -a test_time.txt
