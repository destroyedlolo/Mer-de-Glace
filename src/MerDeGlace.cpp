/*
 * MerDeGlace
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
#include <iomanip>
#include <fstream>

#include <cstring>
#include <cassert>

#include <unistd.h> 	// getopt()
#include <libgen.h>		// basename()
#include <poll.h>

#include "Version.h"
#include "Helpers.h"
#include "Config.h"
#include "Directory.h"
#include "File.h"
#include "SocketInterface.h"

// using namespace std;

	/*****
	 * global configuration
	 *****/

bool verbose = false;
bool debug = false;
bool init = false;
bool autosave = false;

std::string root;
std::string restrict;
std::string dbfile;
std::string rendezvous;	// Command's socket


Directory *rootDir = NULL;	// impersonation of the root directory

void SaveDB(void){
	std::ofstream f;
	f.open(dbfile);
	if(!f.is_open()){
		std::perror(dbfile.c_str());
		exit(EXIT_FAILURE);
	}

	rootDir->save2DB(f);

	f.close();
}

static bool LoadDB(void){
/*
 * Try to load a backup
 * 	no :
 * 		- continue w/o processing
 * 	yes : 
 * 		- check if the root directory matches -> if not, crash
 * 		- create rootDir
 * 		- load all data from the database
 */
	std::ifstream file(dbfile, std::ios::binary);

	if(!file){
		if(verbose)
			std::cerr << "*E* Can't open the database, creating a new one\n";
		return false;
	}

	file.exceptions (std::ios::eofbit | std::ios::failbit);
	try {
		std::string l;

			// Get the root directory
		std::getline( file, l);
		if( l != root ){
			std::cerr << "*F* DB's root directory doesn't match : does the configuration changed\n";
			exit(EXIT_FAILURE);
		}

		rootDir = new Directory(root);	// Create in memory database
		assert(rootDir);
//	rootDir->markCreated();

		if(debug)
			std::cout << "*D* --> root : " << l << std::endl;

		Directory *current = rootDir;
		while(std::getline( file, l)){	// Reading the database from disk
			if(l[0] == '\t'){	// starting with a tab, it's a file
				size_t sep = l.find('\t', 1);
				if(sep == std::string::npos){
					std::cerr << "*F* Malformed database file (missing tab separator)\n";
					exit(EXIT_FAILURE);
				}
				std::string fname = l.substr(1,sep-1);
				std::string md5 = l.substr(sep+1);

				File *n = new File(*current / fname, md5);
				if(!(current->addFile(n))){
					std::cerr << "*F* duplicate File entry '" << fname << "'\n";
					exit(EXIT_FAILURE);
				}
			} else {	// a directory
				Directory *res;

				res = rootDir->findDir(l, true);

				if(res)
					current = res;
				else {
					std::cerr << "*F* no directory found\n";
					exit(EXIT_FAILURE);
				}
			}
		}

	} catch(const std::ifstream::failure &e){
		if(!file.eof()){
			std::cerr << "*F* " << dbfile << " : " << std::strerror(errno) << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	rootDir->raz(true);	// Just to avoid wrong reporting
	if(verbose)
		std::cout << "*I* Database reloaded\n";

	return true;
}

int main(int ac, char **av){
	const char *conf_file = DEFAULT_CONFIGURATION_FILE;
	int c;

	while((c = getopt(ac, av, "hvdf:r:iS")) != EOF) switch(c){
	case 'h':
		std::cerr << basename(av[0]) 
			<< " (" << std::setprecision(5) << VERSION << ")\n"
			<< "Integrity archiving solution\n" << COPYRIGHT << std::endl
			<< "Known options are :\n"
			"\t-h : this online help\n"
			"\t-f<file> : read <file> for configuration\n"
			"\t\t(default is '" << DEFAULT_CONFIGURATION_FILE << "')\n"
			"\t-r<dir> : restrict actions to <dir> directory (allow to process only a subset of a tree)\n"
			"\t-i : launch a scan at startup (warning : can be very long !)\n"
			"\t-S : backup the state after the initial scan (imply -i)\n"
			"\t-v : enable verbose messages\n"
			"\t-d : enable debug messages\n";
		exit(EXIT_FAILURE);
		break;
	case 'd':
		debug = true;
	case 'v':
		std::cout << "Mer de Glace daemon (" << basename(av[0]) << ") v"<< std::setprecision(5) << VERSION << std::endl;
		verbose = true;
		break;
	case 'f':
		conf_file = optarg;
		break;
	case 'r':
		restrict = optarg;
		break;
	case 'S':
		autosave = true;
	case 'i':
		init = true;
		break;
	default:
		std::cerr << "Unknown option\n" << av[0] << " -h\n\tfor some help\n";
		exit(EXIT_FAILURE);
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
			else if( !(arg = striKWcmp( l, "rootDirectory=" )).empty() ){
				if(arg.back() == '/')	// remove trailing '/'
					root = arg.substr(0, arg.size()-1);
				else
					root = arg;
			} else if( !(arg = striKWcmp( l, "DBFile=" )).empty() )
				dbfile = arg;
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
		 * Sanity checks
		 ***/
	if(root.empty()){
		std::cerr << "*F* Root directory not defined\n";
		exit(EXIT_FAILURE);
	}

	if(dbfile.empty()){
		std::cerr << "*F* Database file not defined\n";
		exit(EXIT_FAILURE);
	}

	if(rendezvous.empty()){
		std::cerr << "*F* No rendez-vous file defined\n";
		exit(EXIT_FAILURE);
	}

	if(verbose){
		std::cout << "\troot directory to scan : '" << root << "'\n";
		std::cout << "\tDatabase : '" << dbfile << "'\n";
		std::cout << "\tRendez-vous : '" << rendezvous << "'\n";
	}
	if(!std::filesystem::exists(root)){
		std::cerr << "*F* Root directory doesn't exists\n";
		exit(EXIT_FAILURE);
	}

	if(!restrict.empty()){
		if(Directory::partOf(root,restrict) < 0){
			std::cerr << "*F* Restrict is not part of the root path\n";
			exit(EXIT_FAILURE);
		} else if(!std::filesystem::exists(restrict)){
			std::cerr << "*F* Restrict doesn't exists\n";
			exit(EXIT_FAILURE);
		}
	}

		/***
		 * Feed in memory data
		 ***/
	bool loaded = LoadDB();

	if(!loaded){	// Fresh data
		rootDir = new Directory(root);
		assert(rootDir);
	}

	if(init){
		if(verbose)
			std::cout << "*I* Launching initial scan\n";
		rootDir->raz();
		rootDir->scan();
		if(autosave)
			SaveDB();
	}

#if 1	/* to reduce noise during development */
	if(debug){
		std::cout << "\n*I* Current in memory database\n";
		rootDir->dump();
	}
#endif

		/***
		 * Event handling
		 ***/
	SocketInterface clirdv(rendezvous);
#	define MAXFD	2
	struct pollfd fds[MAXFD];

	for(;;){
		int szfd = 0;	// Number of fd to poll

		clirdv.initPoll(fds, szfd, MAXFD);

		if(debug)
			std::cout << "*d* waiting for event\n";
		int r=poll(fds, szfd, -1);

		if(r < 0){
			if( errno == EINTR)	/* Signal received */
				continue;
			std::perror("poll()");
		}

		clirdv.processSockets(fds, szfd);
	}

	exit(EXIT_SUCCESS);
}

