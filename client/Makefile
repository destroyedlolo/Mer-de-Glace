# makefile created automaticaly by LFMakeMaker
# LFMakeMaker 1.6 (May  9 2022 15:11:43) (c)LFSoft 1997

gotoall: all


#The compiler (may be customized for compiler's options).
cc=c++ -Wall -O2 -I../shared
opts=

../shared/Helpers.o : ../shared/Helpers.cpp Makefile 
	$(cc) -c -o ../shared/Helpers.o ../shared/Helpers.cpp $(opts) 

../shared/SocketHelpers.o : ../shared/SocketHelpers.cpp Makefile 
	$(cc) -c -o ../shared/SocketHelpers.o \
  ../shared/SocketHelpers.cpp $(opts) 

MdG.o : MdG.cpp Makefile 
	$(cc) -c -o MdG.o MdG.cpp $(opts) 

../MdG : MdG.o ../shared/SocketHelpers.o ../shared/Helpers.o Makefile 
	 $(cc) -o ../MdG MdG.o ../shared/SocketHelpers.o \
  ../shared/Helpers.o $(opts) 

all: ../MdG 
