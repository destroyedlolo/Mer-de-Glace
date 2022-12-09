# makefile created automaticaly by LFMakeMaker
# LFMakeMaker 1.6 (May  7 2022 20:46:23) (c)LFSoft 1997

gotoall: all


#The compiler (may be customized for compiler's options).
cc=c++ -Wall -O2 -DDEBUG
opts=

Directory.o : Directory.cpp Directory.h Item.h Makefile 
	$(cc) -c -o Directory.o Directory.cpp $(opts) 

Helpers.o : Helpers.cpp Helpers.h MayBeEmptyString.h Makefile 
	$(cc) -c -o Helpers.o Helpers.cpp $(opts) 

MerDeGlace.o : MerDeGlace.cpp Version.h MayBeEmptyString.h Helpers.h \
  Config.h Directory.h Item.h Makefile 
	$(cc) -c -o MerDeGlace.o MerDeGlace.cpp $(opts) 

../MdG : MerDeGlace.o Helpers.o Directory.o Makefile 
	 $(cc) -o ../MdG MerDeGlace.o Helpers.o Directory.o $(opts) 

all: ../MdG 
