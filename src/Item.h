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
#include <fstream>

class Item : public std::filesystem::path {
public :
	enum _kind {
		IT_FILE = 0,
		IF_DIRECTORY
	};

private :
	std::string name;		// Name of the directory
	enum _kind kind;	// kind ot this item

protected :
	bool loaded;	// from state backup (otherwise, it has been created during this scan
	bool notfound;	// not found during last scan

public :
	Item(const std::filesystem::directory_entry &p, _kind akind) : path(p), kind(akind), loaded(false), notfound(false) {
		this->name = this->filename();
	}

	Item(const std::string &aname, _kind akind) : path(aname), kind(akind) {
		this->name = this->filename();
	}

	Item(const char *aname, _kind akind) : path(aname), kind(akind) {
		this->name = this->filename();
	}

	std::string &getName(void){
		return this->name;
	}

	bool isCreated(void){
		return !this->loaded;
	}

	bool isDeleted(void){
		return this->notfound;
	}

	_kind getKind(void){
		return this->kind;
	}

		// This data is create by loading a backed state
	void loading(void){
		this->loaded = true;
		this->notfound = true;
	}

		// This data has been found during a scan
	void touch(void){
		this->notfound = false;
	}

	virtual void save2DB(std::ofstream &) = 0;

	void markCreated(void){
		this->loaded = true;
	}

	virtual void dump(int ident = 0, int fd = -1) = 0;
	virtual void Report(int fd) = 0;
};

#endif
