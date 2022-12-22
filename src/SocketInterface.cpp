/* SocketInterface.h
 *
 * CLI using unix socket
 *
 * Copyright 2022 Laurent Faillie
 *
 * based on : https://www.ibm.com/docs/en/i/7.1?topic=designs-example-nonblocking-io-select
 */

#include "SocketInterface.h"

#include <iostream>

#include <cerrno>
#include <cstdlib>

#include <unistd.h>

SocketInterface::SocketInterface(const char *name) : peer(-1), socketfile(name) {
	if((this->s = socket(AF_UNIX, SOCK_SEQPACKET | SOCK_CLOEXEC, 0)) == -1){
		perror("socket");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_un local;
	local.sun_family = AF_UNIX;
	strcpy(local.sun_path, name);
	unlink(local.sun_path);	// remove potential existing file

	int len = strlen(local.sun_path) + sizeof(local.sun_family);
	if(bind(this->s, (struct sockaddr *)&local, len) == -1){
		perror("bind()");
		exit(EXIT_FAILURE);
	}

	if(listen(this->s,1) == -1){
		perror("listen()");
		exit(EXIT_FAILURE);
	}
}

SocketInterface::~SocketInterface(void){
	if(this->s != -1)
		close(this->s);

	unlink(this->socketfile);
}
