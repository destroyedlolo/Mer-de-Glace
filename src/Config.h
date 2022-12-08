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

extern bool verbose;
extern const char *root;
extern const char *dbfile;
extern const char *report;

enum _Mode {
	VERIFY = 0,	// look for file changes (default)
	REBUILD		// Rebuild the database
};

extern enum _Mode mode;
#endif
