/* FindDuplicate.h
 *
 * Looks for duplication based on checksum
 *
 * Copyright 2022 Laurent Faillie
 *
 * Well, everything may have been done in a single function. But using
 * here an object and separate steps makes the maintenance and
 * future evolution easier.
 */

#include "FindDuplicate.h"

FindDuplicate::FindDuplicate(int treesize) : sz(treesize){
}

	/* Populate in memory database.
	 * -> fd : to display messages
	 * -> root : root directory for document
	 * <- false in case of error
	 */
bool FindDuplicate::init(int fd, Directory *root){
	return true;
}

void FindDuplicate::report(int fd){
}
