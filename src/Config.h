/*
 * Config.h
 * 	Shared configuration
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
#ifndef CONFIG_H
#define CONFIG_H

#include "NoSlashPath.h"

#include <string>

	// verbosity
extern bool verbose;
extern bool debug;
extern bool corrupted;

	// configuration
extern NoSlashPath root;		// Root data directory
extern NoSlashPath restrict;
extern NoSlashPath altroot;		// alternate root directory

extern std::string dbfile;		// state's backup
extern std::string rendezvous;	// CLI communication socket

	// shared function
void SaveDB(void);
#endif
