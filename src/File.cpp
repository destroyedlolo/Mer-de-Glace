/* File.h
 *
 * Instantiate a file
 *
 * Copyright 2022 Laurent Faillie
 */

#include "File.h"
#include "Config.h"

void File::dump( int ident ){
	for(int i=0; i<ident; i++)
		putchar('\t');

	printf("File '%s' (%s)\n", this->getName().c_str(), this->c_str());
}
