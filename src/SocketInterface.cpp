/* SocketInterface.h
 *
 * CLI using unix socket
 *
 * Copyright 2022 Laurent Faillie
 *
 * based on : https://www.ibm.com/docs/en/i/7.1?topic=designs-example-nonblocking-io-select
 */

#include "SocketInterface.h"
#include "Config.h"
#include "Directory.h"
#include "SocketHelpers.h"

#include <iostream>
#include <map>

#include <cerrno>
#include <cstdlib>
#include <cassert>

#include <unistd.h>
#include <poll.h>

SocketInterface::SocketInterface(const std::string name) : peer(-1), socketfile(name) {
	if((this->s = socket(AF_UNIX, SOCK_SEQPACKET | SOCK_CLOEXEC | SOCK_NONBLOCK, 0)) == -1){
		std::perror("socket");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_un local;
	local.sun_family = AF_UNIX;
	strcpy(local.sun_path, name.c_str());
	unlink(local.sun_path);	// remove potential existing file

	int len = strlen(local.sun_path) + sizeof(local.sun_family);
	if(bind(this->s, (struct sockaddr *)&local, len) == -1){
		std::perror("bind()");
		exit(EXIT_FAILURE);
	}

	if(listen(this->s,1) == -1){
		std::perror("listen()");
		exit(EXIT_FAILURE);
	}
}

SocketInterface::~SocketInterface(void){
	if(this->s != -1)
		close(this->s);

	unlink(this->socketfile.c_str());
}

void SocketInterface::initPoll(struct pollfd *fds, int &act_sz, int max_sz){
	if(this->peer != -1){
		assert(act_sz < max_sz);
		fds[act_sz].fd = this->peer;
		fds[act_sz].events = POLLIN;
		act_sz++;
	}

	if(this->s != -1){
		assert(act_sz < max_sz);
		fds[act_sz].fd = this->s;
		fds[act_sz].events = POLLIN;
		act_sz++;
	}
}

	/*************************************
	 * Commands implementation
	 *************************************/

struct Command {
	const char *desc;	// description of the command
	void (&func)(int, std::string);	// function to implement the command
};

static void cmd_help(int, std::string);

static void cmd_restrict(int fd, std::string arg){
	if(arg.empty()){
		if(restrict.empty())
			socsend(fd, "No restriction in place");
		else {
			socsend(fd, "Restricted to '" + restrict + "'");
		}
	} else {	// new restriction
		if(Directory::partOf(root,arg) < 0)
			socsend(fd, "*E* Restrict is not part of the root path");
		else if(!std::filesystem::exists(arg))
			socsend(fd, "*E* Restricted directory doesn't exists");
		else {
			restrict = arg;
			socsend(fd, "*I* Restriction changed");
		}
	}
}

std::map<std::string, Command> commands {
	{ "help", { "list known commands", cmd_help }},
	{ "restrict", { "restrict actions to a subdir", cmd_restrict }},
/*	{ "scan", { "launch a scan", cmd_restrict }} */
};

static void cmd_help(int fd, std::string){
	std::string res;

	for(const auto &[k,v] : commands){
		res = k + " : " + v.desc + '\n';
		socsend(fd, res);
	}
}

void SocketInterface::processSockets(struct pollfd *fds, int &sz){
	for(int i=0; i<sz; i++){
		if(fds[i].fd == this->s){	// New connection attempt
			if(fds[i].revents != POLLIN)
				continue;

				// Only one new connexion is possible
			if(this->peer != -1){
				std::cerr << "*F* peering slot is already in use\n";
				exit(EXIT_FAILURE);
			}

			if(debug)
				std::cout << "*d* connection\n";

			if((this->peer = accept(this->s, NULL, NULL))<0){
				std::perror("accept()");
				return;
			}
		}

		if(fds[i].fd == this->peer){	// incoming command
			char buffer[2048];

#if 0	/* Code for an interactive shell */
			bool close_conn = false;	// Do we have to close the connection
			for(;;){
				int rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
				if(rc<0){
					if(errno != EWOULDBLOCK){
						std::perror("recv()");
						close_conn = true;
					}
					break;
				} else if(rc==0){
					if(debug)
						std::cout << "*d* disconnected\n";
					close_conn = true;
					break;
				} else {	// Data received
std::cout << "*d* received " << rc << " bytes\n";
					if(buffer[--rc] == '\n')
						buffer[rc] = 0;
std::cout << ">>" << buffer << "<<\n";
					char msg[]="response\n";
					rc = send(fds[i].fd, msg, sizeof(msg), 0);
          			if(rc < 0){
						std::perror("send()");
						close_conn = true;
						break;
					}
				}
			}

			if(close_conn){
				close(this->peer);
				this->peer = -1;
				fds[i].fd = -1;	// unneeded, but to be sure :)
			}
#else	/* Simple command/response code */
			int rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
			if(rc<0){
				std::perror("recv()");
			} else if(rc==0){
				if(debug)
					std::cout << "*d* disconnected\n";
			} else {	// Data received
				buffer[rc] = 0;	// Ensure it's a null terminated string
				char *p = strchr(buffer, '\t');
				if(p)
					*(p++) = 0;	// buffer is the command, p the argument

				if(debug){
					std::cout << "command : '" << buffer << "'";
					if(p)
						std::cout << " arg: '"<< p << "'";
					std::cout << std::endl;
				}
				
				if(auto cmd = commands.find(buffer); cmd != commands.end())
					cmd->second.func(fds[i].fd, p ? p : std::string() );
				else
					socsend(fds[i].fd, "Command not found");
			}

			close(this->peer);
			this->peer = -1;
			fds[i].fd = -1;	// unneeded, but to be sure :)
#endif
		}
	}
}

