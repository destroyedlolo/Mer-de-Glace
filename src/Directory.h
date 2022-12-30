/* Directory.h
 *
 * Instantiate a directory
 *
 * Copyright 2022 Laurent Faillie
 */

#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "NoSlashPath.h"
#include "Item.h"
#include "File.h"

#include <string>
#include <list>

class Directory : public Item {
	std::list<Directory *> subdirs;	// objects in this directory
	std::list<File *> subfiles;	// objects in this directory

public :
		// directory constructor
	Directory(const std::filesystem::directory_entry &e) : Item(e, Item::_kind::IF_DIRECTORY){}
	Directory(const std::string aname) : Item(aname, Item::_kind::IF_DIRECTORY){}

		// Look for an existing object
	static Item *findItemInRootDir(NoSlashPath, int fd=-1);
	Directory *findDir(std::string, bool recursive=false, bool create=true);
	File *findFile(std::string);
	bool exist(Item *sub){
		return(this->findDir(sub->getName()) || this->findFile(sub->getName()));
	}

		// add sub objects
	bool addDir(Directory *);
	bool addFile(File *);

		// Refresh directory's own information
		// fd = file descriptor to write to
	void scan(int fd = -1);
	void raz(bool loaded=false);

		// Display content
	virtual void dump(int ident=0, int fd=-1);
	virtual void Report(int fd);
	virtual void save2DB(std::ofstream &);

		// Check if a path belong to another
		//
		// root = a/b/c
		// -2 : sub is outside root ( "a/b/d" )
		// -1 : sub is upper root ( "a/b" )
		// 0 : sub is == root ( "a/b/c" )
		// +1 : sub is inside root ( "a/b/c/d" )
	static int partOf(const std::filesystem::path root, const std::filesystem::path sub);
};

extern Directory *rootDir;
#endif
