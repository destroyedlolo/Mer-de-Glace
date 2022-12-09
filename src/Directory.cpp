/* Directory.cpp
 * 	Helper functions
 */

#include "Directory.h"

#include <string>
#include <iostream>
#include <filesystem>

void Directory::rescan(void){
	for(const auto & entry : std::filesystem::directory_iterator(this->getName())){
		if(entry.is_regular_file())
			putchar('F');
		else if(entry.is_directory())
			putchar('D');
		else	// Ignoring all "special" files
			continue;
		printf("-> %s\n", entry.path().c_str());
	}
}
