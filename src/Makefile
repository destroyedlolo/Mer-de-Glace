# makefile created automaticaly by LFMakeMaker
# LFMakeMaker 1.6 (May  7 2022 20:46:23) (c)LFSoft 1997

gotoall: all


#The compiler (may be customized for compiler's options).
cc=c++ -Wall -O2 -I../shared
opts=$(shell pkg-config --libs openssl)

../shared/Helpers.o : ../shared/Helpers.cpp Makefile 
	$(cc) -c -o ../shared/Helpers.o ../shared/Helpers.cpp $(opts) 

Directory.o : Directory.cpp Directory.h Item.h File.h Config.h \
  Makefile 
	$(cc) -c -o Directory.o Directory.cpp $(opts) 

File.o : File.cpp File.h Item.h Config.h Makefile 
	$(cc) -c -o File.o File.cpp $(opts) 

MerDeGlace.o : MerDeGlace.cpp Version.h Config.h Directory.h Item.h \
  File.h SocketInterface.h Makefile 
	$(cc) -c -o MerDeGlace.o MerDeGlace.cpp $(opts) 

SocketInterface.o : SocketInterface.cpp SocketInterface.h Config.h \
  Makefile 
	$(cc) -c -o SocketInterface.o SocketInterface.cpp $(opts) 

../MerDeGlaced : SocketInterface.o MerDeGlace.o File.o Directory.o \
  ../shared/Helpers.o Makefile 
	 $(cc) -o ../MerDeGlaced SocketInterface.o MerDeGlace.o File.o \
  Directory.o ../shared/Helpers.o $(opts) 

all: ../MerDeGlaced 
