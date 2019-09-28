/*
 * version.hpp
 *
 *  Created on: May 30, 2018
 *      Author: kisada
 */

#ifndef VERSION_HPP_
#define VERSION_HPP_


const char* mueb_version = 

"v1.0"


#ifdef _COMMIT
"-" _COMMIT
#endif
//TODO else compile error

#ifdef _DIRTYTREE
"-dirty"
#endif

;


#endif /* VERSION_HPP_ */
