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

void Directory::scan( void ){
	for(const auto & entry : std::filesystem::directory_iterator(*this)){
		if(entry.is_regular_file()){
			File *n = new File(entry);
			this->subfiles.push_back(n);
		} else if(entry.is_directory()){
			Directory *n = new Directory(entry);
			n->scan();
			this->subdirs.push_back(n);
		}
		else	// Ignoring all "special" files
			continue;
	}
}

void Directory::dump( int ident ){
	for(int i=0; i<ident; i++)
		putchar('\t');

	printf("Directory '%s' (%s)\n", this->getName().c_str(), this->c_str());

	for(auto sub : this->subfiles)
		sub->dump(ident + 1);

	for(auto sub : this->subdirs)
		sub->dump(ident + 1);
}

void Directory::save2DB(FILE *f){
	fprintf(f, "%s\n", this->c_str());

	for(auto sub : this->subfiles)
		sub->save2DB(f);

	for(auto sub : this->subdirs)
		sub->save2DB(f);
}
