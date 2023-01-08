/* FindDuplicate.h
 *
 * Looks for duplication based on checksum
 *
 * Copyright 2022 Laurent Faillie
 */

#include "FindDuplicate.h"

FindDuplicate::FindDuplicate(int treesize) : sz(treesize){
		// Let's std::bad_alloc exception handling memory issue
	this->dt = new std::vector<File *>[treesize];
}

void FindDuplicate::addFile(File *file){
	this->dt[file->getCS() % this->sz].push_back(file);
}

void FindDuplicate::report(int fd){
}
