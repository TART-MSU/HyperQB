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

# ./hyperqb.sh demo/bakery.smv demo/bakery.smv demo/temp.hq 1 -pes -find


# ./hyperqb.sh demo/cmu1.smv demo/cmu1.smv demo/cmu1.smv demo/cmu1.hq 5 -pes -find

# ./hyperqb.sh demo/mini.smv demo/mini.smv demo/mini.hq 2 -pes -find

# ./hyperqb.sh demo/small.smv demo/small.smv demo/small.hq 5 -pes -find

# ./hyperqb.sh demo/robotic_sp10.smv demo/robotic_sp10.smv demo/robotic_sp.hq 10 -pes -find
# ./hyperqb.sh demo/robotic_sp.smv demo/robotic_sp.smv demo/robotic_sp.hq 3 -pes -find
# ./hyperqb.sh demo/robotic_sp5.smv demo/robotic_sp5.smv demo/robotic_sp.hq 5 -pes -find
# ./hyperqb.sh demo/robotic_sp10.smv demo/robotic_sp10.smv demo/robotic_sp.hq 10 -pes -find
# ./hyperqb.sh demo/robotic_sp13.smv demo/robotic_sp13.smv demo/robotic_sp.hq 13 -pes -find
./hyperqb.sh demo/robotic_sp15.smv demo/robotic_sp15.smv demo/robotic_sp.hq 15 -pes -find
# ./hyperqb.sh demo/robotic_sp20.smv demo/robotic_sp20.smv demo/robotic_sp.hq 20 -pes -find

# cp build_today/HQ.qcir gen_qcir/test/AB.qcir
