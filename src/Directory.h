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
#include "FindDuplicate.h"

#include <string>
#include <list>

class Directory : public Item {
	std::list<Directory *> subdirs;	// objects in this directory
	std::list<File *> subfiles;	// objects in this directory

public :
		// directory constructor
	Directory(const std::filesystem::directory_entry &e) : Item(e, Item::_kind::IF_DIRECTORY){}
	Directory(const std::string aname) : Item(aname, Item::_kind::IF_DIRECTORY){}

	virtual ~Directory();

		// Look for an existing object
	static Item *findItemInRootDir(NoSlashPath, Directory *&, int fd=-1);
	Directory *findDir(std::string, bool recursive=false, bool create=true);
	File *findFile(std::string);
	bool exist(Item *sub){
		return(this->findDir(sub->getName()) || this->findFile(sub->getName()));
	}

		// add sub object
	bool addDir(Directory *);
	bool addFile(File *);

		// remove sub object
	void removeDir(Directory *o){
		this->subdirs.remove(o);
	}
	void removeFile(File *o){
		this->subfiles.remove(o);
	}
	

		// Refresh directory's own information
		// fd = file descriptor to write to
	void scan(int fd = -1);
	void feedDuplicate(int, FindDuplicate &);

		// test and recovery functions
	void raz(bool loaded=false);
	void recalculateCS(void);

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

		// Swap rootDir with the alternate if it set
	static std::string swapAlternate(const std::filesystem::path);
		// reverse to rootDir if the alternate is set
	static std::string backToRoot(const std::filesystem::path);
};

extern Directory *rootDir;
#endif
