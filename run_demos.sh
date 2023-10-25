#!/bin/sh
#####################################################################################################
#  This script runs ALL experiments (0.1-11.1) we provided in Table 3 of our tool paper submission  #
#####################################################################################################

### Requirements:
###   1. Before running, please make sure Docker is installed: (https://docs.docker.com/get-docker/)
###   2. Apple Chop Macs might need to use: --platform linux/amd64

HYPERQB="./hyperqb.sh"


### DEMOS, see README for detailed explination

### [demo 0: run simple_krip with simple_krip_formula]
${HYPERQB} demo/mini.smv demo/mini.smv demo/mini.hq 3 -pes -bughunt
# ${HYPERQB} demo/mini.smv demo/mini.smv demo/mini.smv demo/mini.hq 3 -pes
${HYPERQB} demo/mini2.smv demo/mini2.smv demo/mini2.hq 3 -pes -find

### [demo 1: run bakery algorithm with symmetry property]
# ${HYPERQB} demo/bakery.smv demo/bakery.smv demo/symmetry.hq 10 -pes -debug

### [demo 2: run SNARK algorithm with linearizability propoerty, bug1]
# ${HYPERQB} demo/snark1_conc.smv demo/snark1_seq.smv demo/lin.hq 18 -pes -debug

### [demo 3: run info-flow example]
${HYPERQB} demo/infoflow.smv demo/infoflow.smv demo/infoflow1.hq 5 -pes -debug
# ${HYPERQB} demo/infoflow.smv demo/infoflow.smv demo/infoflow.smv demo/infoflow2.hq 5 -pes -debug







