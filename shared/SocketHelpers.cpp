/* SocketHelpers.h
 * 	Socket related helpers
 */

#include "SocketHelpers.h"

#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

void socsend(int fd, std::string msg){
	if(fd < 0)
		return;

	int rc = send(fd, msg.c_str(), msg.length(), 0);
	if(rc < 0)
		std::perror("send()");
}
