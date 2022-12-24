/* Directory.h
 *
 * Instantiate a directory
 *
 * Copyright 2022 Laurent Faillie
 */

#include "Directory.h"
#include "File.h"
#include "Config.h"
#include "SocketHelpers.h"

#include <string>
#include <iostream>
#include <filesystem>
#include <list>
#include <cassert>

void Directory::scan(int fd){
	this->touch();

	socsend(fd, "In '"+ (std::string)*this + "'\n");

	for(const auto & entry : std::filesystem::directory_iterator(*this)){
		int res = 0;	// by default, let scan !

		if(!restrict.empty()){
			if((res = Directory::partOf(restrict,entry)) == -2){
				if(debug)
					std::cout << "*d* skip "<< entry << std::endl;
				continue;
			}
		}

		if(entry.is_regular_file()){
			if(res < 0)	// Not inside restrict
				continue;

			socsend(fd, "File '"+ (std::string)std::filesystem::path(entry).filename() + "'\n");

			File *n;
			if((n = this->findFile(std::filesystem::path(entry).filename()))){
				if(debug)
					std::cout << "*d* Existing file : " << std::filesystem::path(entry).filename() << std::endl;

				n->touch();
				if(n->setActual()){
					if(debug)
						std::cout << "*d* File changed !\n";
				}
			} else {
				if(debug)
					std::cout << "*d* New file : " << std::filesystem::path(entry).filename() << std::endl;

				n = new File(entry);
				assert(n);
				this->subfiles.push_back(n);
			}
		} else if(entry.is_directory()){
			Directory *n;
			if((n = this->findDir(std::filesystem::path(entry).filename()))){
				if(debug)
					std::cout << "*d* Existing directory : " << std::filesystem::path(entry).filename() << std::endl;

				n->scan(fd);
			} else {
				if(debug)
					std::cout << "*d* New directory : " << std::filesystem::path(entry).filename() << std::endl;

				n = new Directory(entry);
				assert(n);
				n->scan(fd);
				this->subdirs.push_back(n);
			}
		}
		else	// Ignoring all "special" files
			continue;
	}
}

/* -> recursive : we can recursively create intermediate directories (we are loading)
 * -> name : if recursive is set, name = full path
 * otherwise, we are looking only on the Directory and name = filename / last dir
 * <- parent stores parent directory
 * -> up : upper (parent) directory
 */
Directory *Directory::findDir(std::string name, bool recursive){
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
			n->markCreated();
			this->addDir(n);

			return n;
		} else {	// recurse in subdir
			if(this->partOf(*this, name) > 0){
				if(debug)
					std::cout << "*d* going deeper\n";

				for(auto sub : this->subdirs){
					Directory *res = sub->findDir(name, true);
					if(res)
						return res;
				}
			} else
				return NULL;
		}
	} else
		for(auto sub : this->subdirs)
			if(sub->getName() == name)
				return sub;

	return NULL;
}

File *Directory::findFile(std::string name){
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

void Directory::raz(bool loaded){
	this->Item::raz(loaded);

	for(auto sub : this->subfiles)
		sub->raz(loaded);

	for(auto sub : this->subdirs)
		sub->raz(loaded);
}

void Directory::dump(int ident, int fd){
	std::string res;
	for(int i=0; i<ident; i++)
		res += '\t';

	res += "Directory '" + (std::string)this->getName() + "' ("+ (std::string)*this +") ";
	if(this->isCreated())
		res += "crt ";
	if(this->isDeleted())
		res += "Del";
	res += '\n';

	if(debug)
		std::cout << res;
	socsend(fd, res);

	for(auto sub : this->subfiles)
		sub->dump(ident + 1, fd);

	for(auto sub : this->subdirs)
		sub->dump(ident + 1, fd);
}

void Directory::Report(int fd){
	if(this->isCreated())
		socsend(fd, "[Created]\t" + (std::string)*this + '\n');
	if(this->isDeleted())
		socsend(fd, "[Deleted]\t" + (std::string)*this + '\n');

	for(auto sub : this->subfiles)
		sub->Report(fd);

	for(auto sub : this->subdirs)
		sub->Report(fd);
}

void Directory::save2DB(std::ofstream &f){
	f << this->string() << std::endl;
	this->markCreated(false);

	for(auto sub : this->subfiles)
		sub->save2DB(f);

	for(auto sub : this->subdirs)
		sub->save2DB(f);
}

int Directory::partOf(const std::filesystem::path root, const std::filesystem::path sub){
	auto is = sub.begin();
	for(auto ir = root.begin(); ir != root.end(); ir++){
		if(is == sub.end())	// sub shorter than root
			return -1;

		if(*is != *ir)	// Different
			return -2;

		is++;
	}

	return (is == sub.end()) ? 0 : 1;
}
