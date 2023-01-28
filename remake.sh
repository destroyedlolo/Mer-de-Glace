#!/bin/bash

#this script is used to create a Makefile

# uncomment this line to enable development code
DEV="-DDEV"

echo
echo "Deamon's source configuration"
echo "-----------------------------"
echo

cd src
LFMakeMaker -v +f=Makefile -I../shared/ +g +gr \
	-cc="c++ -Wall -O2 -I../shared $DEV " \
	--opts="\$(shell pkg-config --libs openssl)" \
	../shared/*.cpp \
	*.cpp -t=../MerDeGlaced > Makefile

echo
echo "Client's source configuration"
echo "-----------------------------"
echo

cd ../client
LFMakeMaker -v +f=Makefile -I../shared/ +g +gr \
	-cc="c++ -Wall -O2 -I../shared $DEV " \
	../shared/*.cpp \
	*.cpp -t=../MdG > Makefile
