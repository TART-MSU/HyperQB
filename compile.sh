#!/bin/bash
######################################
# compile all necessary executable   #
######################################

### new genqbf with partial multi-gate ###
# cd src/expression/
# make clean
# make 
# cp bin/genqbf ../../exec/genqbf_partialmulti
# cd ../..

### genqbf with binary gate ###
# cd src/genqbf/
# make clean
# make
# cp bin/genqbf ../../exec/genqbf_bingate
# cd ../..

### genqbf with specpfic cases ###
# cd src/genqbfv5/
# make clean
# make
# cp bin/genqbf ../../exec/genqbf_v5
# cd ../..

# cd src/parser
# ./build.sh
# cd ../..
