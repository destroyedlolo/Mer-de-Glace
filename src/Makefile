# makefile created automaticaly by LFMakeMaker
# LFMakeMaker 1.6 (May  7 2022 20:46:23) (c)LFSoft 1997

gotoall: all


#The compiler (may be customized for compiler's options).
cc=g++ -Wall -O2 -DDEBUG
opts=

MerDeGlace.o : MerDeGlace.cpp Version.h Makefile 
	$(cc) -c -o MerDeGlace.o MerDeGlace.cpp $(opts) 

../MdG : MerDeGlace.o Makefile 
	 $(cc) -o ../MdG MerDeGlace.o $(opts) 

all: ../MdG 
