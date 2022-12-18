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
Directory *Directory::findDir(std::string &name, bool recursive){
	if(recursive){
		std::filesystem::path path(name);

		if(debug)
			std::cout << "*d* cur: '" << *this << "'\n"
				"      -> '" << path << "'\n";

		if(name == *this){	// We found the target
			if(debug)
				std::cout << "*d* found\n";

			return this;
		} else if(path.parent_path() == *this){	// We found the parent directory but the target doesn't exist
			if(debug)
				std::cout << "*d* Create new directory\n";

			Directory *n = new Directory(name.c_str());
			assert(n);
			this->addDir(n);

			return n;
		} else {	// recurse in subdir
			if(this->partOf(*this, name)){
				if(debug)
					std::cout << "*d* going deeper\n";

				for(auto sub : this->subdirs){
					Directory *res = sub->findDir(name, true);
					if(res)
						return res;
				}
			} else {
				std::cerr << "*F* Data outside any directory\n";
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
		std::cout << '\t';

	std::cout << "Directory '" << this->getName() << "' ("<< *this <<")\n";

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

bool Directory::partOf(const std::filesystem::path root, const std::filesystem::path sub){
	auto is = sub.begin();
	for(auto ir = root.begin(); ir != root.end(); ir++){
		if(is == sub.end())	// sub shorter than root
			return false;

		if(*is != *ir)	// Different
			return false;

		is++;
	}

	return true;
}
