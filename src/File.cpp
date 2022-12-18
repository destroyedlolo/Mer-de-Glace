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
#include <iostream>

#include <cstring>

/* Calculate md5
 * inspired by https://blog.magnatox.com/posts/c_hashing_files_with_openssl/
 */
std::string File::md5( std::string &res ){
	if(debug)
		std::cout << "*D* md5(" << *this << ")\n" << std::flush;

	FILE *fp = fopen(this->c_str(), "rb");
	if(!fp){
		std::cerr << "*F* '"<< *this << "' : " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}

	EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
	const EVP_MD *EVP_md5();
	EVP_DigestInit_ex(mdctx, EVP_md5(), NULL);

	size_t n;
	char buff[1024 * 16];
	while((n = fread(buff, 1, sizeof(buff), fp)))
		EVP_DigestUpdate(mdctx, buff, n);

	fclose(fp);

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

	std::cout << "File '" << this->getName()
		<< "' : hist:" << this->getHistorical()
		<< " act:" << this->getActual() << std::endl;
}

void File::save2DB(FILE *f){
	fprintf(f, "\t%s\t%s\n", this->getName().c_str(),this->getHistorical().c_str());
}
