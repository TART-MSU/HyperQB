#!/bin/bash

echo "HyperQB setup starts"

# setup docker and a helper image
sudo apt install docker.io
sudo docker pull tzuhanmsu/hyperqube:latest

echo "HyperQB setup success!"