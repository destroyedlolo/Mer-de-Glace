#!/bin/bash

#this script is used to create a Makefile

echo
echo "Source configuration"
echo "--------------------"
echo

cd src
LFMakeMaker -v +f=Makefile -cc="g++ -Wall -O2 -DDEBUG" *.cpp -t=../MdG > Makefile
