/*
 * Helpers.h
 * 	helpers functions spanned across sources files.
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
#ifndef HELPERS_H
#define HELPERS_H

#include <string>

/* Removes the last character if it's an "\n" */
extern char *removeLF(char *);

/* compares string s against kw keywords
 * Return :
 * 	- remaining string if the keyword matches
 * 	- NULL if the keyword is not found
 */
extern char *striKWcmp( char *s, const char *kw );
extern std::string striKWcmp( std::string s, const char *kw );
extern std::string striKWcmp( std::string s, std::string &kw );

#endif
