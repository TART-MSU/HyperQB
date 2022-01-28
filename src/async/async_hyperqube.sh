#!/bin/sh

## get current location
PWD=$(pwd)
echo "$PWD"

## get all arguments
ALLARG=$@
echo ${ALLARG}

## execute python scripts on docker
docker run -v ${PWD}:/mnt tzuhanmsu/hyperqube:latest /bin/bash -c "cd mnt/; ./parse.sh ${ALLARG}; "

## run BMC
./bmc.sh ${ALLARG}
