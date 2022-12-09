/* Item.h
 *
 * Instanciate directory's sub object
 *
 * Copyright 2022 Laurent Faillie
 */

#ifndef ITEM_H
#define ITEM_H

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

	Item(const char *aname, _kind akind) : path(aname), kind(akind) {
		this->name = this->filename();
	}

	std::string &getName(void){
		return this->name;
	}

	_kind getKind(void){
		return this->kind;
	}
};

#endif
