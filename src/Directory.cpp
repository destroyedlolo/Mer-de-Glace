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

Directory::~Directory(){
		// Subs need to be delete as well
	for(auto sub : this->subfiles){
		if(debug)
			std::cout << "*D* deleting sub file '" << *sub << "'\n";
		delete(sub);
	}

	for(auto sub : this->subdirs){
		if(debug)
			std::cout << "*D* deleting sub dir '" << *sub << "'\n";
		delete(sub);
	}
}

void Directory::scan(int fd){
		// Reset state of locals'
	for(auto sub : this->subdirs)
		sub->touch(false);
	for(auto sub : this->subfiles)
		sub->touch(false);
	this->touch();	// The current directory was found
	
	socsend(fd, "In '"+ (std::string)*this + "'\n");

	for(const auto & entry : std::filesystem::directory_iterator(swapAlternate(*this))){
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

				n = new File(backToRoot(entry));
				assert(n);
				n->markCreated();	// New file
				n->touch();			// File found
				this->subfiles.push_back(n);
			}
		} else if(entry.is_directory()){
			Directory *n;
			if((n = this->findDir(std::filesystem::path(entry).filename()))){
				if(debug)
					std::cout << "*d* Existing directory : " << std::filesystem::path(entry).filename() << std::endl;

				n->touch();
				n->scan(fd);
			} else {
				if(debug)
					std::cout << "*d* New directory : " << std::filesystem::path(entry).filename() << std::endl;

				n = new Directory(backToRoot(entry));
				assert(n);
				n->markCreated();	// New directory
				n->touch();
				n->scan(fd);
				this->subdirs.push_back(n);
			}
		}
		else	// Ignoring all "special" files
			continue;
	}
}

/* Find an existing object in the in memory state.
 *
 * Fail if the object or one of the intermediate directory doesn't exist
 */
Item *Directory::findItemInRootDir(NoSlashPath name, Directory *&parent, int fd){
	if(Directory::partOf(*rootDir,name) < 0){
		socsend(fd, "*E* Object outside root directory");
		return NULL;
	}

		/* Only to cheat as in case we're looking for the root directory
		 * modification (otherwise, it will find as its parent is outside
		 */
	if(name == *rootDir)
		return rootDir;

		/* Find out the parent of the object */
	parent = rootDir->findDir(name.parent_path(), true, false);
	if(!parent){
		socsend(fd, "*E* The parent directory doesn't exist in memory's state");
		return NULL;
	}

	if(debug)
		std::cout << "*d* findDir(parent) -> " << *parent << std::endl;

		/* look for the object itself */
	Item *res = parent->findDir(name.filename(), false, false);
	if(!res)
		res = parent->findFile(name.filename());

	return res;
}

/* Find (or create) a directory.
 *
 * -> recursive : scan recursively
 * -> name : if recursive is set, name = full path
 * otherwise, we are looking only on the Directory and name = filename / last dir
 * -> create : we can recursively create intermediate directories (we are loading)
 */
Directory *Directory::findDir(std::string name, bool recursive, bool create){
	if(debug)
		std::cout << "*d* findDir(" << name << (recursive ? ", rec" : "") << (create ? ", create":"") << ")\n";

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

			for(auto sub : this->subdirs){	// Look in sub directories list
				if(name == *sub){
					if(debug)
						std::cout << "*d* found as sub directory\n";
					return sub;
				}
			}

			if(create){
				if(debug)
					std::cout << "*d* Create new directory\n";

				Directory *n = new Directory(name.c_str());
				assert(n);
				n->markCreated();
				this->addDir(n);

				return n;
			} else {
				if(debug)
					std::cout << "*d* parent found but not object '" << *this << "' - '" << name << "'\n";
				return NULL;
			}
		} else {	// recurse in subdir
			if(this->partOf(*this, name) > 0){
				if(debug)
					std::cout << "*d* going deeper\n";

				for(auto sub : this->subdirs){
					Directory *res = sub->findDir(name, true, create);
					if(res)
						return res;
				}
			} else {
				if(debug)
					std::cout << "*d* outside '" << *this << "' - '" << name << "'\n";
				return NULL;
			}
		}
	} else {
		for(auto sub : this->subdirs){
			if(sub->getName() == name)
				return sub;
		}
	}

	return NULL;
}

File *Directory::findFile(std::string name){
	for(auto sub : this->subfiles){
		if(sub->getName() == name){
			if(debug)
				std::cout << "*d* found as sub file\n";
			return sub;
		}
	}
	
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
	if(!restrict.empty() && !loaded){
		if(Directory::partOf(restrict,*this) >= 0){
			this->Item::raz(loaded);

			for(auto sub : this->subfiles)
				sub->raz(loaded);
		} else if(debug)
			std::cout << "*d* skip "<< *this << std::endl;
	} else {
		this->Item::raz(loaded);

		for(auto sub : this->subfiles)
			sub->raz(loaded);
	}

	for(auto sub : this->subdirs)
		sub->raz(loaded);
}

void Directory::recalculateCS(void){
	if(!restrict.empty()){
		if(Directory::partOf(restrict,*this) >= 0){
			for(auto sub : this->subfiles)
				sub->recalculateCS();
		} else if(debug)
			std::cout << "*d* skip "<< *this << std::endl;
	} else {
		for(auto sub : this->subfiles)
			sub->recalculateCS();
	}

	for(auto sub : this->subdirs)
		sub->recalculateCS();
}

void Directory::feedDuplicate(int fd, FindDuplicate &dup){
	if(!restrict.empty()){
		if(Directory::partOf(restrict,*this) >= 0){
			for(auto sub : this->subfiles)
				dup.addFile(sub);
		} else if(debug)
			std::cout << "*d* skip "<< *this << std::endl;
	} else {
		for(auto sub : this->subfiles)
			dup.addFile(sub);
	}

	for(auto sub : this->subdirs)
		sub->feedDuplicate(fd,dup);
}

void Directory::dump(int ident, int fd){
	std::stringstream res;
	for(int i=0; i<ident; i++)
		res << '\t';

	res << "Directory '" << this->getName() << "' (" << *this << ") ";
	if(this->isCreated())
		res << "crt ";
	if(this->isDeleted())
		res << "Del";

	res << '\n';

	if(debug)
		std::cout << res.str();
	socsend(fd, res.str());

	for(auto sub : this->subfiles)
		sub->dump(ident + 1, fd);

	for(auto sub : this->subdirs)
		sub->dump(ident + 1, fd);
}

void Directory::Report(int fd){
	bool issue=false;
	std::stringstream res;
	res << "[D]";

	if(this->isCreated()){
		res << (altroot.empty() ? "[Created]" : "[Master only]");
		issue = true;
	}
	if(this->isDeleted()){
		res << (altroot.empty() ? "[Deleted]" : "[Replicat only]");
		issue = true;
	}

	if(issue){
		res << '\t' << Directory::swapAlternate(*this);
		if(!altroot.empty())
			res << " (original : " << *this << ")";
		res << std::endl;
		socsend(fd, res.str());
	}

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

std::string Directory::swapAlternate(const std::filesystem::path p){
	if(altroot.empty())
		return p;
	else {
		if(p == *rootDir)
			return(altroot);
		else if(Directory::partOf(root,p) >= 0){
			if(debug)
				std::cout << "*d* swapAlternate r:" << root << " | a:" << altroot << " | " << p << " | " << p.lexically_relative(*rootDir) << std::endl;
			return std::filesystem::path(altroot) / p.lexically_relative(*rootDir);
		} else {
			if(debug)
				std::cout << "swapAlternate : local -> " << p << std::endl;
			return p;
		}
	}
}

std::string Directory::backToRoot(const std::filesystem::path p){
	if(altroot.empty())
		return p;
	else
		return std::filesystem::path(*rootDir) / p.lexically_relative(altroot);
}
