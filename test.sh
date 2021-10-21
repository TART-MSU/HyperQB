#!/bin/sh

PWD=$(pwd)
echo "$PWD"

ALLARG=$@
echo ${ALLARG}


docker run -v ${PWD}:/mnt tzuhanmsu/hyperqube:latest /bin/bash -c "cd mnt/; ./hyperqube_parse.sh ${ALLARG}; "


./hyperqube_bmc.sh bakery.smv symmetry 10 pes
