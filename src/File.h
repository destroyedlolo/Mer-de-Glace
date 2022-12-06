/* File.h
 *
 * Instantiate a file
 *
 * Copyright 2022 Laurent Faillie
 */

#ifndef FILE_H
#define FILE_H

#include <string>

#include <cstdio>

using namespace std;

class File {
	string name;			// Name of the file
	string historical_md5;	// md5 as computed initially
	string actual_md5;		// md5 calculated NOW

public :
		// initialisation from existing data
	File( const char *aname, const char *amd5 ):name(aname), historical_md5(amd5){
	}

		// update actual md5 from an opened file
	File( FILE *, const char *aname );

		// accept any modification
	accept( void );
};

#endif
