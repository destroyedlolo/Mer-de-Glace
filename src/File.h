/* File.h
 *
 * Instantiate a file
 *
 * Copyright 2022 Laurent Faillie
 */

#ifndef FILE_H
#define FILE_H

#include "Item.h"

// #include <cstdio>

class File : public Item {
	std::string name;			// Name of the file
	std::string historical_md5;	// md5 as computed initially
	std::string actual_md5;		// md5 calculated NOW

public :
		// Create a new file and initialise md5
	File(const std::filesystem::directory_entry &e) : Item(e, Item::_kind::IT_FILE){
		this->md5(this->historical_md5);
	}

		// initialisation from existing data
	File( std::string &aname, std::string &amd5 ) : Item(aname, Item::_kind::IT_FILE), historical_md5(amd5){
	}

#if 0
		// update actual md5 from an opened file
	File( FILE *, const char *aname );

		// accept any modification
	accept( void );
#endif

		// Compute it's md5
	std::string md5( std::string & );
	std::string getHistorical( void ){ return this->historical_md5; }
	std::string getActual( void ){ return this->actual_md5; }

		// Display content
	virtual void dump(int ident=0);
	virtual void save2DB(std::ofstream &);
};

#endif
