/* Directory.h
 *
 * Instantiate a directory
 *
 * Copyright 2022 Laurent Faillie
 */

#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "Item.h"
#include "File.h"

#include <list>

class Directory : public Item {
	std::list<Item *> subdirs;	// objects in this directory
	std::list<Item *> subfiles;	// objects in this directory

public :
		// directory constructor
	Directory(const std::filesystem::directory_entry &e) : Item(e, Item::_kind::IF_DIRECTORY){}
	Directory(const char *aname) : Item(aname, Item::_kind::IF_DIRECTORY){}

		// add sub objects
	void addDir( Directory *d ){
		this->subdirs.push_back(d);
	}
	void addFile( File *f ){
		this->subfiles.push_back(f);
	}

		// Rebuild directory's own information
		// Typical use : inital scan of ta directory
	void scan(void);

		// Display content
	virtual void dump( int ident=0 );
	virtual void save2DB(FILE *);
};

#endif
