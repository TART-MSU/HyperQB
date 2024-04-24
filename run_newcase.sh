#!/bin/sh
FOLDER="_develop/newcases/"
HYPERQB="./hyperqb.sh"
SEMANTICS="-pes"
MODE="-bughunt"

K=15
CASE=${FOLDER}setA/case_medium/nested_loop/nested_loop.smv
PROP=${FOLDER}setA/case_medium/nested_loop/nested_loop.hq

# K=5
# CASE=${FOLDER}setA/case_medium/fig2/fig2.smv
# PROP=${FOLDER}setA/case_medium/fig2/fig2.hq


${HYPERQB} ${CASE} ${CASE} ${PROP} ${K} ${SEMANTICS} ${MODE}

