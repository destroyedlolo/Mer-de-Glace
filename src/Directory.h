/* Directory.h
 *
 * Instantiate a directory
 *
 * Copyright 2022 Laurent Faillie
 */

#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "Item.h"

#include <list>

class Directory : public Item {
	std::list<Item *> subs;	// objects in this directory

public :
		// directory constructor
	Directory(const std::filesystem::directory_entry &e) : Item(e, Item::_kind::IF_DIRECTORY){}
	Directory(const char *aname) : Item(aname, Item::_kind::IF_DIRECTORY){}

		// Rebuild directory's own information
		// Typical use : inital scan of ta directory
	void scan(void);

		// Display content
	virtual void dump( int ident=0 );
};

#endif
