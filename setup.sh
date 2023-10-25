#!/bin/bash

echo "HyperQB setup starts"

echo "[ setup docker and a helper image... ]"
sudo apt install docker.io
sudo docker pull tzuhanmsu/hyperqube:latest

echo "[ setpup dotnet for comparison with AH and AHQ... ]"
sudo apt-get update && \
  sudo apt-get install -y dotnet-sdk-7.0
sudo apt-get update && \
  sudo apt-get install -y aspnetcore-runtime-7.0

sudo apt-get install -y dotnet-runtime-7.0


echo "HyperQB setup success!"