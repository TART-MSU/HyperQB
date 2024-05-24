#!/bin/bash
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
  
  echo "HyperQB setup starts for $OSTYPE"
  systemctl stop unattended-upgrades
  apt update

  ### Docker setup
  echo "[ setup docker and a helper image... ]"
  snap refresh
  snap install docker
  echo "check docker installation: "
  docker --version

  ### Temporary container setup (only give permission to R/W into this folder)
  echo "[ setup container of temp-generated files ]"
  mkdir build_today/
  chown -R artifact build_today/

  ### Extract Docker Image
  echo "[ setup docker images ]"
  apt-get install docker-ce
  systemctl start docker
  # docker pull tzuhanmsu/hyperqube:latest --> this require internet access
  groupadd docker
  usermod -aG docker $USER
  newgrp docker
  docker load < hyperqb_docker.tar.gz

  echo "(!) Notice please make sure the PATH of main HyperQB/ folder is placed in: /home/artifact/HyperQB"
  echo "(This is in order to correctly run comparison with other tools, which require specific absolute path into their sub-tool)"
  echo "HyperQB setup finished."
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