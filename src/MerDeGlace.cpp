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

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <unistd.h> 	// getopt()
#include <libgen.h>		// basename()

#include "Version.h"

#define DEFAULT_CONFIGURATION_FILE "/usr/local/etc/MerDeGlace.conf"

using namespace std;

	/*****
	 * global configuration
	 *****/

bool verbose = false;


int main(int ac, char **av){
	const char *conf_file = DEFAULT_CONFIGURATION_FILE;
	int c;

	while((c = getopt(ac, av, "hvf:")) != EOF) switch(c){
	case 'h':
		fprintf(stderr, "%s (%.04f)\n"
			"Integrity archiving solution\n"
			"%s\n"
			"Known options are :\n"
			"\t-h : this online help\n"
			"\t-v : enable verbose messages\n"
			"\t-f<file> : read <file> for configuration\n"
			"\t\t(default is '%s')\n",
			basename(av[0]), VERSION, COPYRIGHT, DEFAULT_CONFIGURATION_FILE
		);
		exit(EXIT_FAILURE);
		break;
	case 'v':
		printf("%s v%.04f\n", basename(av[0]), VERSION);
		verbose = true;
		break;
	default:
		fprintf(stderr, "Unknown option '%c'\n%s -h\n\tfor some help\n", c, av[0]);
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

