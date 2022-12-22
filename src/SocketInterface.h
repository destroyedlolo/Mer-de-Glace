/* SocketInterface.h
 *
 * CLI using unix socket
 *
 * Copyright 2022 Laurent Faillie
 */

#ifndef SOCKETINTERFACE_H
#define SOCKETINTERFACE_H

#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

class SocketInterface {
	int s, peer;
	std::string socketfile;

public:
	SocketInterface(const std::string);
	~SocketInterface(void);

	int getSocket(void){
		return this->s;
	}

	int getPeer(void){
		return this->peer;
	}

		/* Init poolfd structure as per current sockets' state
		 *
		 * -> fds : table of fds
		 * -> act_sz : its actual size (will be increased with newly added sockets)
		 * -> max_sz : max size of fd table
		 */
	void initPoll(struct pollfd *fds, int &act_sz, int max_sz);

		/* Data are waiting on sockets
		 *
		 * -> fds : table of fds
		 * -> sz : size of fds' table
		 */
	void processSockets(struct pollfd *fds, int &sz);
};

#endif
