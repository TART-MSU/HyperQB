#!/bin/bash
if [[ "$OSTYPE" == "linux-gnu"* ]]; then

  ### .NET tool for running AutoHyper and AutoHyperQ
  echo "[ setpup dotnet for comparison with AH and AHQ... ]"
  apt-get update && apt-get install -y dotnet-sdk-7.0
  echo "check dotnet installataion: "
  dotnet --version

  apt-get install build-essential libtool
  apt-get install python3-dev
  apt-get install wget
  apt-get install curl
  apt-get install emacs
  apt-get install r-base r-base-dev r-base-core

  cd exec/linux/compare/spot-2.11.5
  autoreconf -f -i
  ./configure
  make
  make install
  cd .. 
  cd ..
  cd ..
  cd ..

  
else
  echo "sorry, this setup script is designed for Ubuntu 22.04 VM for now"
  exit 1 
fi


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