/* FindDuplicate.h
 *
 * Looks for duplication based on checksum
 *
 * Copyright 2022 Laurent Faillie
 */
#ifndef FINDDUPLICATE_H
#define FINDDUPLICATE_H

#include "File.h"

#include <vector>
#include <map>

class FindDuplicate {
	int sz;
	std::map<std::string, std::vector<File *>> *dt;

public:
	FindDuplicate(int treesize);
	~FindDuplicate(){
		delete[] this->dt;
	}

		// Add a file in the in memory database
	void addFile(File *);

		// Populate in memory database
	void report(int);

		// dump in memory database
		// (to be called only in debut mode as output to daemon's std::cout)
	void dump(void);
};

#endif
