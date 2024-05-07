#!/bin/bash

echo "HyperQB setup starts"

sudo apt update

echo "[ setup docker and a helper image... ]"
sudo snap install docker
sudo docker pull tzuhanmsu/hyperqube:latest

echo "[ setpup dotnet for comparison with AH and AHQ... ]"
sudo apt-get update && \
  sudo apt-get install -y dotnet-sdk-7.0
sudo apt-get update && \
  sudo apt-get install -y aspnetcore-runtime-7.0
sudo apt-get install -y dotnet-runtime-7.0

echo "check docker installation: "
docker --version

echo "check dotnet installataion: "
dotnet --version

echo "HyperQB setup success!"


######################################
# compile all necessary executable   #
######################################

### new genqbf with partial multi-gate ###
# cd src/expression/
# make clean
# make 
# cp bin/genqbf ../../exec/genqbf_partialmulti
# make clean
# cd ../..

### genqbf with binary gate ###
# cd src/genqbf/
# make clean
# make
# cp bin/genqbf ../../exec/genqbf_bingate
# make clean
# cd ../..

### genqbf with specpfic cases ###
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