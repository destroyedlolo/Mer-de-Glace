/* File.h
 *
 * Instantiate a file
 *
 * Copyright 2022 Laurent Faillie
 */

#include "File.h"
#include "Config.h"

#include <openssl/md5.h>
#include <fstream>

#include <cstring>

/*AF* Add try/catch */
std::string File::md5( std::string &res ){
	if(verbose){
		printf("*I* md5(%s)\n", this->c_str());
		fflush(stdout);
	}

	std::ifstream file(this->c_str(), std::ifstream::binary);

	if(!file){
		fprintf(stderr, "*F* '%s' : %s \n", this->c_str(), strerror(errno));
		exit(EXIT_FAILURE);
	}

	MD5_CTX md5Context;
	MD5_Init(&md5Context);
	char buf[1024 * 16];
	while (file.good()) {
    	file.read(buf, sizeof(buf));
	    MD5_Update(&md5Context, buf, file.gcount());
	}
	unsigned char result[MD5_DIGEST_LENGTH];
	MD5_Final(result, &md5Context);

	std::stringstream md5string;
	md5string << std::hex << std::uppercase << std::setfill('0');
	for (const auto &byte: result)
    	md5string << std::setw(2) << (int)byte;

	res = md5string.str();
	return res;
}

void File::dump( int ident ){
	for(int i=0; i<ident; i++)
		putchar('\t');

	printf("File '%s' : hist:%s act:%s\n", this->getName().c_str(), this->getHistorical().c_str(), this->getActual().c_str());
}

void File::save2DB(FILE *f){
	fprintf(f, "\t%s\t%s\n", this->getName().c_str(),this->getHistorical().c_str());
}
