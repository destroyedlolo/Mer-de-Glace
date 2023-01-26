/*
 * MerDeGlace - CLI control
 * 	Integrity archiving solution
 *
 * Copyright 2022 Laurent Faillie
 *
 * 		MerDeGlace is covered by
 *		Creative Commons Attribution-NonCommercial 3.0 License
 *      (http://creativecommons.org/licenses/by-nc/3.0/) 
 *      Consequently, you're free to use if for personal or non-profit usage,
 *      professional or commercial usage REQUIRES a commercial licence.
 *  
 *      MerDeGlace is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#include <iostream>
#include <iomanip>      // std::setprecision
#include <fstream>

#include <cstring>

#include <unistd.h> 	// getopt()
#include <libgen.h>		// basename()
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "Version.h"
#include "Helpers.h"

bool debug = false;

std::string rendezvous;	// Command's socket

int main(int ac, char **av){
	const char *conf_file = DEFAULT_CONFIGURATION_FILE;
	int c;

	while((c = getopt(ac, av, "hdf:")) != EOF) switch(c){
	case 'h':
		std::cerr
			<< "Integrity archiving solution - CLI control ("
			<< std::setprecision(5) << VERSION << ")\n"
			<< COPYRIGHT << "\n\n"
			<< basename(av[0]) << " [-opt] command ...\n" 
			<< "Known options are :\n"
			"\t-h : this online help\n"
			"\t-f<file> : read <file> for configuration\n"
			"\t\t(default is '" << DEFAULT_CONFIGURATION_FILE << "')\n"
			"\t-d : enable debug messages\n"
			"\nHELP displays list of known server commands\n";
		exit(EXIT_FAILURE);
		break;
	case 'd':
		debug = true;
		std::cout << "Mer de Glace CLI control (" << basename(av[0]) << ") v"<< std::setprecision(5) << VERSION << std::endl;
		break;
	case 'f':
		conf_file = optarg;
		break;
	default:
		std::cerr << "Unknown option\n" << av[0] << " -h\n\tfor some help\n";
		exit(EXIT_FAILURE);
	}

	std::string cmd;
	for(;optind < ac; optind++){
		if(!cmd.empty())
			cmd += '\t';
		cmd += av[optind];
	}
		
		/***
		 * Read configuration 
		 ***/
	std::ifstream file;
	file.exceptions ( std::ios::eofbit | std::ios::failbit ); // No need to check failbit

	try {
		std::string l;

		file.open(conf_file);
		while( std::getline( file, l) ){
			std::string arg;

			if( l[0]=='#' )
				continue;
			else if( !(arg = striKWcmp( l, "Socket=" )).empty() )
				rendezvous = arg;
		}
	} catch(const std::ifstream::failure &e){
		if(!file.eof()){
			std::cerr << "*F* "<< conf_file << " : " << std::strerror(errno) << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	file.close();


		/***
		 * Socket's
		 ***/
	int s;
	if((s = socket(AF_UNIX, SOCK_SEQPACKET | SOCK_CLOEXEC /*| SOCK_NONBLOCK*/, 0)) == -1){
		std::perror("socket");
		exit(EXIT_FAILURE);
    }

	if(debug)
		std::cout << "*D* Connecting ...\n";

	struct sockaddr_un remote;
	remote.sun_family = AF_UNIX;
	strcpy(remote.sun_path, rendezvous.c_str());
	int len = strlen(remote.sun_path) + sizeof(remote.sun_family);
	if(connect(s, (struct sockaddr *)&remote, len) == -1){
		std::perror("connect()");
		exit(EXIT_FAILURE);
	}

	if(debug)
		std::cout << "*D* Connected\n";


		/***
		 * Sending command
		 ***/
	
	if(send(s, cmd.c_str(), cmd.length(), 0) < 0){
		std::perror("send()");
		exit(EXIT_FAILURE);
	}

	char buffer[2048];
	int ret=EXIT_SUCCESS;
	for(;;){
		int rc = recv(s, buffer, sizeof(buffer), 0);
		if(rc < 0){
			if(errno != EAGAIN){
				std::perror("recv()");
				exit(EXIT_FAILURE);
			}
		} else if(!rc)	// Socket closed
			break;
		else {
			buffer[rc] = 0;
			std::cout << buffer;
			ret = 100;
		}
	}
	close(s);

	exit(ret);
}
