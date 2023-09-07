#!/bin/sh
#####################################################################################################
#  This script runs ALL experiments (0.1-11.1) we provided in Table 3 of our tool paper submission  #
#####################################################################################################

### Requirements:
###   1. Before running, please make sure Docker is installed: (https://docs.docker.com/get-docker/)
###   2. Apple Chop Macs might need to use: --platform linux/amd64

### DEMOS, see README for detailed explination

### [demo 1: run bakery algorithm with symmetry property]
# ./hyperqb.sh demo/bakery.smv demo/bakery.smv demo/symmetry.hq 10 -pes
### [dem0 2: run SNARK algorithm with linearizability propoerty]
# ./hyperqb.sh demo/SNARK_conc.smv demo/SNARK_seq.smv demo/lin.hq 18 -pes
### [demo 3: run simple_krip with simple_krip_formula]
# ./hyperqb.sh demo/mini.smv demo/mini.smv demo/mini.hq 1 -pes -hops -find
# ./hyperqb.sh demo/mini.smv demo/mini.smv demo/mini.smv demo/mini.smv  demo/mini_try.hq 10 -pes -find
# ./hyperqb.sh demo/mini.smv demo/mini.smv demo/mini.smv demo/mini.hq 3 -pes


./hyperqb.sh demo/bakery.smv demo/bakery.smv demo/temp.hq 1 -pes -find