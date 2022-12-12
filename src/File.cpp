/* File.h
 *
 * Instantiate a file
 *
 * Copyright 2022 Laurent Faillie
 */

#include "File.h"
#include "Config.h"

#include <openssl/evp.h>
#include <fstream>

#include <cstring>

/* Calculate md5
 * based on https://blog.magnatox.com/posts/c_hashing_files_with_openssl/
 */
std::string File::md5( std::string &res ){
	if(verbose){
		printf("*I* md5(%s)\n", this->c_str());
		fflush(stdout);
	}

	FILE *fp = fopen(this->c_str(), "rb");
	if(!fp){
		fprintf(stderr, "*F* '%s' : %s \n", this->c_str(), strerror(errno));
		exit(EXIT_FAILURE);
	}

	EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
	const EVP_MD *EVP_md5();
	EVP_DigestInit_ex(mdctx, EVP_md5(), NULL);

	size_t n;
	char buff[1024 * 16];
	while((n = fread(buff, 1, sizeof(buff), fp)))
		EVP_DigestUpdate(mdctx, buff, n);

	unsigned char result[EVP_MAX_MD_SIZE];
	unsigned int md_len;

	EVP_DigestFinal_ex(mdctx, result, &md_len);
	EVP_MD_CTX_free(mdctx);
	
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
