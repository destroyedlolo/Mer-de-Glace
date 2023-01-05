/* File.h
 *
 * Instantiate a file
 *
 * Copyright 2022 Laurent Faillie
 */

#ifndef FILE_H
#define FILE_H

#include "Item.h"

// #include <cstdio>

class File : public Item {
	std::string name;			// Name of the file
	std::string historical_md5;	// md5 as computed initially
	uint16_t cs;					// md5 checksum
	std::string actual_md5;		// md5 calculated NOW

public :
		// Create a new file and initialise md5
	File(const std::filesystem::directory_entry &e) : Item(e, Item::_kind::IT_FILE){
		this->md5(this->historical_md5);
		this->cs = File::calCS(this->historical_md5);
	}

		// initialisation from existing data (loading from backup)
	File( std::string aname, std::string amd5, uint16_t acs ) : Item(aname, Item::_kind::IT_FILE), historical_md5(amd5), cs(acs){
	}

		// Compute it's md5
	std::string md5( std::string & );

		// calcule the CS of an md5 string
	static uint16_t calCS( std::string );
	bool hasValideSignature(void){
		return(!(this->historical_md5.empty() || calCS(this->historical_md5) != this->cs));
	}

		// Set actual md5 (if changed)
		// -> true : md5 changed
	bool setActual(void);

		// True if the file has changed
	bool isChanged(void){
		return(!this->actual_md5.empty());
	}

	void acceptChange(void);

	std::string getHistorical(void){ return this->historical_md5; }
	uint16_t getCS(void){ return this->cs; }
	std::string getActual(void){ return this->actual_md5; }

	void raz(bool loaded=false);
	void recalculateCS(void){
		this->cs = File::calCS(this->historical_md5);
	}

		// Display content
	virtual void Report(int);
	virtual void dump(int ident=0, int fd=-1);
	virtual void save2DB(std::ofstream &);
};

#endif
