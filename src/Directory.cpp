/* Directory.h
 *
 * Instantiate a directory
 *
 * Copyright 2022 Laurent Faillie
 */

#include "Directory.h"
#include "File.h"
#include "Config.h"

#include <string>
#include <iostream>
#include <filesystem>
#include <list>

void Directory::rescan(void){
	for(const auto & entry : std::filesystem::directory_iterator(*this)){
		if(entry.is_regular_file()){
			File *n = new File(entry);
			this->subs.push_back(n);
		} else if(entry.is_directory()){
			Directory *n = new Directory(entry);
			n->rescan();
			this->subs.push_back(n);
		}
		else	// Ignoring all "special" files
			continue;
	}
}

void Directory::dump( int ident ){
	for(int i=0; i<ident; i++)
		putchar('\t');

	printf("Directory '%s' (%s)\n", this->getName().c_str(), this->c_str());

	for(auto sub : this->subs)
		sub->dump(ident + 1);
}
