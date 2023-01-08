/* FindDuplicate.h
 *
 * Looks for duplication based on checksum
 *
 * Copyright 2022 Laurent Faillie
 */
#ifndef FINDDUPLICATE_H
#define FINDDUPLICATE_H

#include "Directory.h"

class FindDuplicate {
	int sz;

public:
	FindDuplicate(int treesize);


	bool init(int, Directory *);
	void lookForDuplicate(int);
};

#endif
