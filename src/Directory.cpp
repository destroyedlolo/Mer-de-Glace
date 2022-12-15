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
#include <cassert>

void Directory::scan(void){
	for(const auto & entry : std::filesystem::directory_iterator(*this)){
		if(entry.is_regular_file()){
			File *n = new File(entry);
			assert(n);
			this->subfiles.push_back(n);
		} else if(entry.is_directory()){
			Directory *n = new Directory(entry);
			assert(n);
			n->scan();
			this->subdirs.push_back(n);
		}
		else	// Ignoring all "special" files
			continue;
	}
}

/* -> name, recursive : if recursive is set, name = full path
 * otherwise, we are looking only on the Directory and name = filename / last dir
 * <- parent stores parent directory
 * -> up : upper (parent) directory
 */
Directory *Directory::findDir(std::string &name, bool recursive, Directory **parent, Directory *up){
	if(recursive){
		std::filesystem::path path(name);

		if(debug)
			printf("*d* cur: '%s'\n      -> '%s'\n", this->c_str(), path.c_str());

		if(name == *this){	// We found the target
			if(debug)
				puts("*d* found");

			*parent = up;
			return this;
		} else if(path.parent_path() == *this){	// We found the parent directory but the target doesn't exist
			if(debug)
				puts("*d* Create new directory");

			*parent = this;
			Directory *n = new Directory(name.c_str());
			assert(n);
			this->addDir(n);

			return n;
		} else {	// recurse in subdir
			if(this->string().compare(0, this->string().length(), name)){
				if(debug)
					puts("*d* going deeper");

				for(auto sub : this->subdirs){
					Directory *res = sub->findDir(name, true, parent, this);
					if(res)
						return res;
				}
			} else {
				puts("*F* Data outside any directory");
				exit(EXIT_FAILURE);
			}
		}
	} else
		for(auto sub : this->subdirs)
			if(sub->getName() == name)
				return sub;

	return NULL;
}

File *Directory::findFile(std::string &name){
	for(auto sub : this->subfiles)
		if(sub->getName() == name)
			return sub;
	
	return NULL;
}

bool Directory::addDir(Directory *d){
	if(this->exist(d))
		return(false);

	this->subdirs.push_back(d);
	return true;
}

bool Directory::addFile(File *f){
	if(this->exist(f))
		return(false);

	this->subfiles.push_back(f);
	return true;
}

void Directory::dump(int ident){
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
