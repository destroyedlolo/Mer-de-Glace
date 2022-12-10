#!/bin/bash

#this script is used to create a Makefile

echo
echo "Source configuration"
echo "--------------------"
echo

cd src
LFMakeMaker -v +f=Makefile \
	-cc="c++ -Wall -O2 -DDEBUG" \
	--opts="\$(shell pkg-config --libs openssl)" \
	*.cpp -t=../MdG > Makefile
