# makefile created automaticaly by LFMakeMaker
# LFMakeMaker 1.6 (May  9 2022 15:11:43) (c)LFSoft 1997

gotoall: all

# Warning : 'string' can't be located for this node.
# Warning : 'cstring' can't be located for this node.
# Warning : 'cstdarg' can't be located for this node.

#The compiler (may be customized for compiler's options).
cc=c++ -Wall -O2 -I../shared -DDEV 
opts=

../shared/Helpers.o : ../shared/Helpers.cpp ../shared/Helpers.h \
  Makefile 
	$(cc) -c -o ../shared/Helpers.o ../shared/Helpers.cpp $(opts) 

# Warning : 'filesystem' can't be located for this node.
# Warning : 'string' can't be located for this node.
../shared/NoSlashPath.o : ../shared/NoSlashPath.cpp \
  ../shared/NoSlashPath.h Makefile 
	$(cc) -c -o ../shared/NoSlashPath.o ../shared/NoSlashPath.cpp \
  $(opts) 

# Warning : 'string' can't be located for this node.
# Warning : 'iostream' can't be located for this node.
# Warning : 'sys/types.h' can't be located for this node.
# Warning : 'sys/socket.h' can't be located for this node.
# Warning : 'sys/un.h' can't be located for this node.
../shared/SocketHelpers.o : ../shared/SocketHelpers.cpp \
  ../shared/SocketHelpers.h Makefile 
	$(cc) -c -o ../shared/SocketHelpers.o \
  ../shared/SocketHelpers.cpp $(opts) 

# Warning : 'iostream' can't be located for this node.
# Warning : 'iomanip' can't be located for this node.
# Warning : 'fstream' can't be located for this node.
# Warning : 'cstring' can't be located for this node.
# Warning : 'unistd.h' can't be located for this node.
# Warning : 'libgen.h' can't be located for this node.
# Warning : 'sys/types.h' can't be located for this node.
# Warning : 'sys/socket.h' can't be located for this node.
# Warning : 'sys/un.h' can't be located for this node.
# Warning : 'string' can't be located for this node.
MdG.o : MdG.cpp ../shared/Helpers.h ../shared/Version.h Makefile 
	$(cc) -c -o MdG.o MdG.cpp $(opts) 

../MdG : MdG.o ../shared/SocketHelpers.o ../shared/NoSlashPath.o \
  ../shared/Helpers.o Makefile 
	 $(cc) -o ../MdG MdG.o ../shared/SocketHelpers.o \
  ../shared/NoSlashPath.o ../shared/Helpers.o $(opts) 

all: ../MdG 
