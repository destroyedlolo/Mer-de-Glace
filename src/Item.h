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
	enum _kind kind;	// kind of this item

protected :
	bool created;	// discovered during scan
	bool found;		// found during scan

public :
	Item(const std::filesystem::directory_entry &p, _kind akind) : path(p), kind(akind), created(false), found(false) {
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
		return this->created;
	}

	bool isDeleted(void){
		return(!this->found);
	}

	_kind getKind(void){
		return this->kind;
	}

		// Reset the status before a scan
	void raz(void){
		this->created = false;
		this->found = false;
	}

	void markCreated(void){
		this->created = true;
	}

		// This data has been found during a scan
	void touch(void){
		this->found = true;
	}

	virtual void save2DB(std::ofstream &) = 0;

	virtual void dump(int ident = 0, int fd = -1) = 0;
	virtual void Report(int fd) = 0;
};

#endif
