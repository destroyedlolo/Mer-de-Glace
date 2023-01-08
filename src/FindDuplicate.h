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

class FindDuplicate {
	int sz;
	std::vector<File *> *dt;

public:
	FindDuplicate(int treesize);

	void addFile(File *);

		// Populate in memory database
	void report(int);
};

#endif
