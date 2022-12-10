/* File.h
 *
 * Instantiate a file
 *
 * Copyright 2022 Laurent Faillie
 */

#ifndef FILE_H
#define FILE_H

#include "Item.h"

#include <cstdio>

class File : public Item {
	std::string name;			// Name of the file
	std::string historical_md5;	// md5 as computed initially
	std::string actual_md5;		// md5 calculated NOW

public :
	File(const std::filesystem::directory_entry &e) : Item(e, Item::_kind::IT_FILE){}
#if 0
		// initialisation from existing data
	File( const char *aname, const char *amd5 ):name(aname), historical_md5(amd5){
	}

		// update actual md5 from an opened file
	File( FILE *, const char *aname );

		// accept any modification
	accept( void );
#endif

		// Display content
	virtual void dump( int ident=0 );
};

#endif
