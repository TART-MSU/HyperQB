#!/bin/bash

g++ -std=c++17 -Wall -o parser parser.cpp;
cp parser ../../exec/
# ./parser "file/tiny.smv" "output/I_1.bool" "output/R_1.bool"

# FLAG="-find"
# OUTFOLDER="output"
# python3 translate.py ${OUTFOLDER} "file/tiny.smv" "file/tiny.smv" "file/tiny.smv" "file/tiny.hq" ${FLAG}
