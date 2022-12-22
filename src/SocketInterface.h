/* SocketInterface.h
 *
 * CLI using unix socket
 *
 * Copyright 2022 Laurent Faillie
 */

#ifndef SOCKETINTERFACE_H
#define SOCKETINTERFACE_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

class SocketInterface {
	int s, peer;
	const char *socketfile;

public:
	SocketInterface(const char *);
	~SocketInterface(void);

	int getSocket(void){
		return this->s;
	}

	int getPeer(void){
		return this->peer;
	}
};

#endif
