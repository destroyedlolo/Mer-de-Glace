/* Directory.h
 *
 * Instantiate a directory
 *
 * Copyright 2022 Laurent Faillie
 */

#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "Item.h"

class Directory : public Item {

public :
		// Empty directory
	Directory(const char *aname) : Item(aname, Item::_kind::IF_DIRECTORY){
	}

		// rescan the directory
	void rescan(void);
};

#endif
