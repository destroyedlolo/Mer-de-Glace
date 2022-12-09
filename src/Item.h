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

using namespace std;

class Item {
public :
	enum _kind {
		IT_FILE = 0,
		IF_DIRECTORY
	};

private :
	string name;		// Name of the directory
	enum _kind kind;	// kind ot this item

public :

	Item(const char *aname, _kind akind) : kind(akind) {
		this->name = std::filesystem::canonical(aname);
	}

	string &getName(void){
		return this->name;
	}

	_kind getKind(void){
		return this->kind;
	}
};

#endif
