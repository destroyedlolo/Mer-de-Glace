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
	Directory(const std::filesystem::path &p) : Item(p, Item::_kind::IF_DIRECTORY){}

	Directory(const char *aname) : Item(aname, Item::_kind::IF_DIRECTORY){}

		// rescan the directory
	void rescan(void);
};

#endif
