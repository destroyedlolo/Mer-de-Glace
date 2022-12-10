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
const char *root = NULL;
const char *dbfile = NULL;
const char *report = NULL;
enum _Mode mode = _Mode::VERIFY;

Directory *rootDir = NULL;

static void SaveDB(void){
	FILE *f = fopen(dbfile, "wb");
	if(!f){
		perror(dbfile);
		exit(EXIT_FAILURE);
	}

	rootDir->save2DB(f);

	fclose(f);
}

int main(int ac, char **av){
	const char *conf_file = DEFAULT_CONFIGURATION_FILE;
	int c;

	while((c = getopt(ac, av, "hvf:m:")) != EOF) switch(c){
	case 'h':
		fprintf(stderr, "%s (%.04f)\n"
			"Integrity archiving solution\n"
			"%s\n"
			"Known options are :\n"
			"\t-h : this online help\n"
			"\t-f<file> : read <file> for configuration\n"
			"\t\t(default is '%s')\n"
			"\t-m<MODE> : set mode among\n"
			"\t\tVERIFY : check for files changes\n"
			"\t\tREBUILD : rebuild the database (set with fresh values)\n"
			"\t-v : enable verbose messages\n",
			basename(av[0]), VERSION, COPYRIGHT, DEFAULT_CONFIGURATION_FILE
		);
		exit(EXIT_FAILURE);
		break;
	case 'v':
		printf("Mer de Glace (%s) v%.04f\n", basename(av[0]), VERSION);
		verbose = true;
		break;
	case 'f':
		conf_file = optarg;
		break;
	case 'm':
		if(!strcasecmp(optarg, "VERIFY"))
			mode = _Mode::VERIFY;
		else if(!strcasecmp(optarg, "REBUILD"))
			mode = _Mode::REBUILD;
		else {
			fprintf(stderr, "*F* Unknown mode : '%s'\n", optarg);
			exit(EXIT_FAILURE);
		}
		break;
	default:
		fprintf(stderr, "Unknown option '%c'\n%s -h\n\tfor some help\n", c, av[0]);
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
			fprintf(stderr, "*F* %s : %s\n", conf_file, strerror(errno) );
			exit(EXIT_FAILURE);
		}
	}

	file.close();

		/***
		 * Sanity checks
		 ***/
	if(!root){
		fprintf(stderr, "*F* Root directory not defined\n");
		exit(EXIT_FAILURE);
	}

	if(!dbfile){
		fprintf(stderr, "*F* Database file not defined\n");
		exit(EXIT_FAILURE);
	}

	if(!report){
		fprintf(stderr, "*F* No report file defined\n");
		exit(EXIT_FAILURE);
	}

	if(verbose){
		printf("\troot directory to scan : '%s'\n", root);
		printf("\tDatabase : '%s'\n", dbfile);
		printf("\tResulting report : '%s'\n", report);
		switch(mode){
		case _Mode::VERIFY :
			puts("\tVERIFY mode : checking for files differences");
			break;
		case _Mode::REBUILD :
			puts("\tREBUILD mode : Rebuild the database with the actual status");
			break;
		}
	}

		/***
		 * Feed in memory data
		 ***/

	if(mode != _Mode::REBUILD){
/*
 * check if the database can be loaded
 * 	no :
 * 		- continue w/o processing
 * 	yes : 
 * 		- check if the root directory matches -> if not, crash
 * 		- create rootDir
 * 		- load all data from the database
 */
	}

	if(!rootDir){	// The database need to be rebuilt
		mode = _Mode::REBUILD;

		rootDir = new Directory(root);
		assert(rootDir);

		rootDir->scan();
		SaveDB();			// New content need to be saved
	}

	if(verbose){
		puts("\n*I* Current in memory database");
		rootDir->dump();
	}

	exit(EXIT_SUCCESS);
}

