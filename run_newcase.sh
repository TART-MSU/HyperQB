#!/bin/sh
FOLDER="_develop/newcases/"
HYPERQB="./hyperqb.sh"
SEMANTICS="-pes"
MODE="-bughunt"
K=5

CASE=${FOLDER}setA/case_large/csrf.smv
PROP=${FOLDER}setA/case_large/csrf.hq











${HYPERQB} ${CASE} ${CASE} ${PROP} ${K} ${SEMANTICS} ${MODE}

