/* File.h
 *
 * Instantiate a file
 *
 * Copyright 2022 Laurent Faillie
 */

#include "File.h"
#include "Directory.h"
#include "Config.h"
#include "SocketHelpers.h"

#include <openssl/evp.h>

#include <fstream>
#include <iostream>

#include <cstring>

	// As this constructor is used mostly when creating an object during a scan
	// with an altroot, swapAlternate() is needed
File::File(const std::string aname, int fd) : Item(Directory::swapAlternate(aname), Item::_kind::IT_FILE){
	this->md5(this->historical_md5, fd);
	this->cs = File::calCS(this->historical_md5);
}

/* Calculate md5
 * inspired by https://blog.magnatox.com/posts/c_hashing_files_with_openssl/
 */
std::string File::md5(std::string &res, int fd){
	if(debug)
		std::cout << "*D* md5(" << this->string() << ")\n" << std::flush;

	FILE *fp = fopen64(this->c_str(), "rb");
	if(!fp){
		std::stringstream msg;
		msg << "*E* '"<< *this << "' : " << std::strerror(errno);

		if(debug)
			std::cerr << msg.str() << std::endl;
		socsend(fd, msg.str());

		res = "ERROR";
		this->markBad();
		return res;
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

	for(unsigned int i=0; i<md_len; i++)
    	md5string << std::setw(2) << (int)result[i];

	res = md5string.str();
	this->markBad(false);
	return res;
}

uint16_t File::calCS(std::string md5){
	uint16_t res = 0;

	for(auto &c : md5)
		res += c;
	
	return res;
}

bool File::potentialEq(File *other){
	return(this->getCS() == other->getCS() && this->getHistorical() == other->getHistorical());
}

void File::raz(bool loaded){
	if(!restrict.empty() && !loaded){
		if(Directory::partOf(restrict,*this) >= 0){
			if(loaded)
				this->acceptChange();
			this->Item::raz(loaded);
		} else if(debug)
			std::cout << "*d* skip "<< *this << std::endl;
	} else {
		if(loaded)
			this->acceptChange();
		this->Item::raz(loaded);
	}
}

bool File::setActual(int fd){
	this->md5(this->actual_md5,fd);

	if(this->actual_md5 == this->historical_md5){	// not changed
		this->actual_md5.clear();
		return(false);
	} else
		return(true);
}

void File::acceptChange(void){
	if(this->isChanged()){
		this->historical_md5 = this->actual_md5;
		this->actual_md5.clear();
		this->cs = File::calCS(this->getHistorical());
	}
}

void File::Report(int fd){
	bool issue=false;
	std::stringstream res;
	res << "[F]";

	if(!this->hasValideSignature()){
		res << "[Bad CS]";
		issue = true;
		corrupted = true;
	}
	if(this->isCreated()){
		res << (altroot.empty() ? "[Created]" : "[Replica only]");
		issue = true;
	}
	if(this->isBad()){
		res << "[ERROR]";
		issue = true;
	}
	if(this->isDeleted()){
		res << (altroot.empty() ? "[Deleted]" : "[Master only]");
		issue = true;
	}
	if(this->isChanged()){
		res << (altroot.empty() ? "[Changed]" : "[Discrepancy]");
		issue = true;
	}

	if(issue){
		res << '\t' << Directory::swapAlternate(*this);
		if(!altroot.empty())
			res << " (original : " << *this << ")";
		res << std::endl;
		socsend(fd, res.str());
	}
}

void File::dump(int ident, int fd){
	std::stringstream res;

	for(int i=0; i<ident; i++)
		res << '\t';
	
	res << "File '" << this->getName()
		<< "' : hist:" << this->getHistorical() << " -" << std::hex << this->getCS() << "/" << File::calCS(this->getHistorical()) << "-"
		<< " act:" << this->getActual();

	if(!this->hasValideSignature())
		res << " CS";
	if(this->isBad())
		res << " BAD";
	if(this->isCreated())
		res << " crt";
	if(this->isDeleted())
		res << " Del";
	res << '\n';

	if(debug)
		std::cout << res.str();
	socsend(fd, res.str());
}

void File::save2DB(std::ofstream &f){
	f << '\t' << this->getName() << '\t' << this->getHistorical() << '\t' << this->getCS() << std::endl;
	this->markCreated(false);
}
