/* FindDuplicate.h
 *
 * Looks for duplication based on checksum
 *
 * Copyright 2022 Laurent Faillie
 */

#include "Config.h"
#include "FindDuplicate.h"

#include <iostream>

FindDuplicate::FindDuplicate(int treesize) : sz(treesize){
		// Let's std::bad_alloc exception handling memory issue
	this->dt = new std::map<std::string, std::vector<File *>>[treesize];
}

void FindDuplicate::addFile(File *file){
	if(debug)
		std::cout << "*I* Adding "<< *file << std::endl;
	this->dt[file->getCS() % this->sz][file->getHistorical()].push_back(file);
}

void FindDuplicate::report(int fd){
}

void FindDuplicate::dump(void){
	std::cout << "*I* Duplicate in memory content\n";
	for(int i=0; i<this->sz; i++){
		std::cout << "index : "<< i << " (" << this->dt[i].size() << ")\n";
		for(const auto &[k,v] : this->dt[i]){
			std::cout << "  md5: " << k << " (" << v.size() << ")\n";
			for(auto d : v){
				std::cout << "    file: " << *d << std::endl;
			}
		}
	}
}
