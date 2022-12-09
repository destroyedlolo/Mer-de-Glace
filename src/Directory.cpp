/* Directory.cpp
 * 	Helper functions
 */

#include "Directory.h"
#include "Config.h"

#include <string>
#include <iostream>
#include <filesystem>
#include <list>

void Directory::rescan(void){
	if(verbose)
		printf("*I* scanning '%s' directory\n", this->getName().c_str());

	for(const auto & entry : std::filesystem::directory_iterator(*this)){
		if(entry.is_regular_file())
			putchar('F');
		else if(entry.is_directory()){
			Directory *n = new Directory(entry.path());
			putchar('D');
		}
		else	// Ignoring all "special" files
			continue;
		printf("-> %s\n", entry.path().c_str());
	}
}
