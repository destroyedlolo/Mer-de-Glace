/* Item.h
 *
 * Instanciate directory's sub object
 *
 * Copyright 2022 Laurent Faillie
 */

#ifndef ITEM_H
#define ITEM_H

#include <cstdio>

#include <string>
#include <filesystem>

class Item : public std::filesystem::path {
public :
	enum _kind {
		IT_FILE = 0,
		IF_DIRECTORY
	};

private :
	std::string name;		// Name of the directory
	enum _kind kind;	// kind ot this item

public :
	Item(const std::filesystem::directory_entry &p, _kind akind) : path(p), kind(akind) {
		this->name = this->filename();
	}

	Item(const char *aname, _kind akind) : path(aname), kind(akind) {
		this->name = this->filename();
	}

	std::string &getName(void){
		return this->name;
	}

	_kind getKind(void){
		return this->kind;
	}

	virtual void dump( int ident = 0 ) = 0;
	virtual void save2DB(FILE *) = 0;
};

#endif
