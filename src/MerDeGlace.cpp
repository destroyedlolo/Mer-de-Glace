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
#include <fstream>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>

#include <unistd.h> 	// getopt()
#include <libgen.h>		// basename()

#include "Version.h"
#include "MayBeEmptyString.h"
#include "Helpers.h"
#include "Config.h"
#include "Directory.h"
#include "File.h"

#define DEFAULT_CONFIGURATION_FILE "/usr/local/etc/MerDeGlace.conf"

// using namespace std;

	/*****
	 * global configuration
	 *****/

bool verbose = false;
bool debug = false;
const char *root = NULL;
const char *restrict = NULL;
const char *dbfile = NULL;
const char *report = NULL;
enum _Mode mode = _Mode::VERIFY;

Directory *rootDir = NULL;

static void SaveDB(void){
	std::ofstream f;
	f.open(dbfile);
	if(!f.is_open()){
		perror(dbfile);
		exit(EXIT_FAILURE);
	}

	rootDir->save2DB(f);

	f.close();
}

static bool LoadDB(void){
/*
 * check if the database can be loaded
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
			puts("*E* Can't open the database, creating a new one");
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

		if(debug)
			printf("*D* --> root : %s\n", l.c_str());

		Directory *current = rootDir;
		while(std::getline( file, l)){	// Reading the database from disk
			if(l[0] == '\t'){	// starting with a tab, it's a file
				size_t sep = l.find('\t', 1);
				if(sep == std::string::npos){
					fputs("*F* Malformed database file (missing tab separator)\n", stderr);
					exit(EXIT_FAILURE);
				}
				std::string fname = l.substr(1,sep-1);
				std::string md5 = l.substr(sep+1);

				File *n = new File(fname, md5);
				if(!(current->addFile(n))){
					fprintf(stderr,"*F* duplicate File entry '%s'\n", fname.c_str());
					exit(EXIT_FAILURE);
				}
			} else {	// a directory
				Directory *res;

				res = rootDir->findDir(l, true);

				if(res)
					current = res;
				else {
					puts("*F* no directory found");
					exit(EXIT_FAILURE);
				}
			}
		}

	} catch(const std::ifstream::failure &e){
		if(!file.eof()){
			fprintf(stderr, "*F* %s : %s", dbfile, strerror(errno) );
			exit(EXIT_FAILURE);
		}
	}

	if(verbose)
		puts("*I* Database reloaded");

	return true;
}

int main(int ac, char **av){
	const char *conf_file = DEFAULT_CONFIGURATION_FILE;
	int c;

	while((c = getopt(ac, av, "hvdf:m:r:")) != EOF) switch(c){
	case 'h':
		std::cerr << basename(av[0]) 
			<< " (" << std::setprecision(5) << VERSION << ")\n"
			<< "Integrity archiving solution\n" << COPYRIGHT << std::endl
			<< "Known options are :\n"
			"\t-h : this online help\n"
			"\t-f<file> : read <file> for configuration\n"
			"\t\t(default is '" << DEFAULT_CONFIGURATION_FILE << "')\n"
			"\t-m<MODE> : set mode among\n"
			"\t\tVERIFY : check for files changes\n"
			"\t\tREBUILD : rebuild the database (set with fresh values)\n"
			"\t-r<dir> : restrict action to <dir> directory (allow to process only a subset of a tree)\n"
			"\t-v : enable verbose messages\n"
			"\t-d : enable debug messages\n";
		exit(EXIT_FAILURE);
		break;
	case 'd':
		debug = true;
	case 'v':
		std::cout << "Mer de Glace (" << basename(av[0]) << ") v"<< std::setprecision(5) << VERSION << std::endl;
		verbose = true;
		break;
	case 'f':
		conf_file = optarg;
		break;
	case 'r':
		restrict = optarg;
		break;
	case 'm':
		if(!strcasecmp(optarg, "VERIFY"))
			mode = _Mode::VERIFY;
		else if(!strcasecmp(optarg, "REBUILD"))
			mode = _Mode::REBUILD;
		else {
			std::cerr << "*F* Unknown mode : '" << optarg << ")\n";
			exit(EXIT_FAILURE);
		}
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
			MayBeEmptyString arg;

			if( l[0]=='#' )
				continue;
			else if( !!(arg = striKWcmp( l, "rootDirectory=" ))){
				if(arg.back() == '/')	// remove trailing '/'
					arg = arg.substr(0, arg.size()-1);
				assert(( root = strdup( arg.c_str() ) ));
			} else if( !!(arg = striKWcmp( l, "DBFile=" )))
				assert(( dbfile = strdup( arg.c_str() ) ));
			else if( !!(arg = striKWcmp( l, "Report=" )))
				assert(( report = strdup( arg.c_str() ) ));
		}
	} catch(const std::ifstream::failure &e){
		if(!file.eof()){
			std::cerr << "*F* "<< conf_file << " : " << strerror(errno) << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	file.close();

		/***
		 * Sanity checks
		 ***/
	if(!root){
		std::cerr << "*F* Root directory not defined\n";
		exit(EXIT_FAILURE);
	}

	if(!dbfile){
		std::cerr << "*F* Database file not defined\n";
		exit(EXIT_FAILURE);
	}

	if(!report){
		std::cerr << "*F* No report file defined\n";
		exit(EXIT_FAILURE);
	}

	if(verbose){
		std::cout << "\troot directory to scan : '" << root << "'\n";
		std::cout << "\tDatabase : '" << dbfile << "'\n";
		std::cout << "\tResulting report : '" << report << "'\n";
		switch(mode){
		case _Mode::VERIFY :
			std::cout << "\tVERIFY mode : checking for files differences\n";
			break;
		case _Mode::REBUILD :
			std::cout << "\tREBUILD mode : Rebuild the database with the actual status\n";
			break;
		}
	}
	if(!std::filesystem::exists(root)){
		std::cerr << "*F* Root directory doesn't exists\n";
		exit(EXIT_FAILURE);
	}

	if(restrict){
		if(!Directory::partOf(root,restrict)){
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

	if(mode != _Mode::REBUILD){
		LoadDB();

#if 0	/* debug only to check reloaded state */
dbfile = "/tmp/reloaded.mdg";
SaveDB();
#endif
	}

	if(!rootDir){	// The database need to be rebuilt
		mode = _Mode::REBUILD;

		rootDir = new Directory(root);
		assert(rootDir);

		rootDir->scan();
		SaveDB();			// New content need to be saved
	}


#if 0	/* to reduce noise during development */
	if(debug){
		std::cout << "\n*I* Current in memory database\n";
		rootDir->dump();
	}
#endif

	exit(EXIT_SUCCESS);
}

