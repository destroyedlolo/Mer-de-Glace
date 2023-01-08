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
#include "FindDuplicate.h"

#include <iostream>
#include <map>

#include <cerrno>
#include <cstdlib>
#include <cassert>	// Sanities for situation that should never happen

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
			socsend(fd, "Restricted to '" + (std::string)restrict + "'");
		}
	} else {	// new restriction
		if(arg == "*"){
			restrict.clear();
			socsend(fd, "No restriction");
		} else if(Directory::partOf(root,arg) < 0)
			socsend(fd, "*E* Restrict is not part of the root path");
		else if(!std::filesystem::exists(arg))
			socsend(fd, "*E* Restricted directory doesn't exists");
		else {
			restrict = arg;
			socsend(fd, "*I* Restriction changed");
		}
	}
}

static void cmd_scan(int fd, std::string){
	::rootDir->scan(fd);
}

static void cmd_dump(int fd, std::string){
	::rootDir->dump(0,fd);
}

static void cmd_save(int fd, std::string){
	SaveDB();

	socsend(fd, "*I* State saved as '" + dbfile + "'");
}

static void cmd_report(int fd, std::string){
	corrupted = false;

	rootDir->Report(fd);

	if(corrupted){
		if(verbose)
			std::cout << "*E* In memory state is corrupted\n";
		socsend(fd, "*E* In memory state is corrupted\n");
	}
}

static void cmd_raz(int fd, std::string){
	rootDir->raz();

	socsend(fd, "*I* State reseted");
}

static void cmd_recs(int fd, std::string){
	if(debug){
		rootDir->recalculateCS();
		socsend(fd, "*W* Checksum recalculated");
	} else
		socsend(fd, "*E* Checksum recalculation is allowed only when the daemon is in debug mode");
}

/* Restrict doesn't impact accept command */
static void cmd_accept(int fd, std::string arg){
	if(arg.empty()){
		socsend(fd, "*E* Object to be accepted missing");
		return;
	}

	Directory *parent;
	Item *obj = Directory::findItemInRootDir(arg, parent, fd);
	if(!obj){
		socsend(fd, "*E* Not found");
		return;
	}

	if(obj->isCreated()){
		if(debug)
			std::cout << "*I* object created\n";
		obj->markCreated(false);
	}

	if(obj->getKind() == Item::_kind::IT_FILE){
		if(((File *)obj)->isChanged()){
			if(debug)
				std::cout << "*I* object changed\n";
			((File *)obj)->acceptChange();
		}
	}

	if(obj->isDeleted()){
		if(obj->getKind() == Item::_kind::IT_FILE){
			parent->removeFile((File *)obj);
			delete((File *)obj);

			if(debug)
				std::cout << "*I* File deleted\n";
		} else if(obj->getKind() == Item::_kind::IF_DIRECTORY){
			parent->removeDir((Directory *)obj);
			delete((Directory *)obj);

			if(debug)
				std::cout << "*I* Directory deleted\n";
		}
	}

	socsend(fd, "*I* Change accepted");
}

static void cmd_duplicate(int fd, std::string arg){
	int nbre = 1024;
	
	if(!arg.empty()){
		nbre = stoi(arg);
		if(nbre <= 0){
			socsend(fd, "*E* Tree size can't be <= 0\n");
			return;
		}
	}

	try {
		FindDuplicate rep(nbre);
		rootDir->feedDuplicate(fd, rep);
		rep.report(fd);
	} catch(std::bad_alloc& ex){
		socsend(fd, "*E* Ran out of memory, sorry\n");
	}
}

std::map<std::string, Command> commands {
	{ "help", { "list known commands", cmd_help }},
	{ "restrict", { "Restrict actions to a subdir, '*' to remove restriction", cmd_restrict }},
	{ "RESET", { "reset items status (DANGEROUS)", cmd_raz }},
	{ "RAZ", { "reset items status (DANGEROUS)", cmd_raz }},
	{ "RECS", { "recalculate checksums (VERY DANGEROUS, debug mode only)", cmd_recs }},
	{ "scan", { "launch a scan", cmd_scan }},
	{ "save", { "Save on disk the memory database", cmd_save }},
	{ "report", { "Report discrepancies", cmd_report }},
	{ "status", { "Report discrepancies (report alias)", cmd_report }},
	{ "duplicate", { "Report potential duplication", cmd_duplicate }},
	{ "accept", { "Validate a discrepancy", cmd_accept }},
	{ "commit", { "Validate a discrepancy (accept alias", cmd_accept }},
	{ "dump", { "Dump current in memory database", cmd_dump }}
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

