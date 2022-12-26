/* NoSlashPath.h
 * 	Path without trailing '/'
 *
 * Copyright 2022 Laurent Faillie
 *
 * 		MerDeGlace is covered by
 *		Creative Commons Attribution-NonCommercial 3.0 License
 *      (http://creativecommons.org/licenses/by-nc/3.0/) 
 *      Consequently, you're free to use if for personal or non-profit usage,
 *      professional or commercial usage REQUIRES a commercial licence.
 *  
 *      MerDeGlace is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#ifndef NOSLASHPATH_H
#define NOSLASHPATH_H

#include <filesystem>
#include <string>


class NoSlashPath : public std::filesystem::path {
public:
	NoSlashPath() {};
	
	NoSlashPath &operator= (std::string);
};

#endif
