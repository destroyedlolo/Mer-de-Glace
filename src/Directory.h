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
	std::list<Directory *> subdirs;	// objects in this directory
	std::list<File *> subfiles;	// objects in this directory

public :
		// directory constructor
	Directory(const std::filesystem::directory_entry &e) : Item(e, Item::_kind::IF_DIRECTORY){}
	Directory(const char *aname) : Item(aname, Item::_kind::IF_DIRECTORY){}

		// Look for existing object
	Directory *findDir(std::string &, bool recursive=false);
	File *findFile(std::string &);
	bool exist(Item *sub){
		return(this->findDir(sub->getName()) || this->findFile(sub->getName()));
	}

		// add sub objects
	bool addDir(Directory *);
	bool addFile(File *);

		// Rebuild directory's own information
		// Typical use : inital scan of ta directory
	void scan(void);

		// Display content
	virtual void dump(int ident=0);
	virtual void save2DB(std::ofstream &);

		// Check if a path belong to another
	static bool partOf(const std::filesystem::path root, const std::filesystem::path sub);
};

#endif
