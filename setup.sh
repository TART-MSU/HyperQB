#!/bin/bash

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
  echo "HyperQB setup starts for $OSTYPE"
  apt update
  echo "[ setup docker and a helper image... ]"
  snap refresh
  snap install docker
  docker pull tzuhanmsu/hyperqube:latest

  echo "[ setpup dotnet for comparison with AH and AHQ... ]"
  apt-get update && \
    apt-get install -y dotnet-sdk-7.0
  apt-get update && \
    apt-get install -y aspnetcore-runtime-7.0
  apt-get install -y dotnet-runtime-7.0

  echo "[ checking installations ]"
  echo "check docker installation: "
  docker --version

  echo "check dotnet installataion: "
  dotnet --version
elif [[ "$OSTYPE" == "darwin"* ]]; then
  echo "HyperQB setup starts for $OSTYPE"
  # BINLOCATION="exec/mac"
else
  echo "sorry, current OS not supported yet :("
  exit 1 
fi

echo "HyperQB setup success!"


######################################
# compile all necessary executable   #
######################################
# if (echo $* | grep -e "--compile" -q) then 
# ## new genqbf with partial multi-gate ###
# cd src/expression/
# make clean
# make 
# cp bin/genqbf ../../exec/genqbf_partialmulti
# make clean
# cd ../..

# ### genqbf with binary gate ###
# cd src/genqbf/
# make clean
# make
# cp bin/genqbf ../../exec/genqbf_bingate
# make clean
# cd ../..

# ### genqbf with specpfic cases ###
# cd src/genqbfv5/
# make clean
# make
# cp bin/genqbf ../../exec/genqbf_v5
# make clean
# cd ../..

# cd src/parser
# g++ -std=c++17 -Wall -o parser parser.cpp
# # g++ parser.cpp -o parser 
# cp parser ../../exec/parser 
# cd ../..