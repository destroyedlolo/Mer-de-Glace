/* NoSlashPath.h
 *	Path without trailing '/'
 *
 * Copyright 2022 Laurent Faillie
 */

#include "NoSlashPath.h"

NoSlashPath &NoSlashPath::operator= (std::string arg){
	if(arg.back() == '/')	// remove trailing '/'
		arg = arg.substr(0, arg.size()-1);

	this->assign(arg);
	return(*this);
}
