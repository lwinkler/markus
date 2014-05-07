/*----------------------------------------------------------------------------------
*
*    MARKUS : a manager for video analysis modules
* 
*    author : Laurent Winkler <lwinkler888@gmail.com>
* 
* 
*    This file is part of Markus.
*
*    Markus is free software: you can redistribute it and/or modify
*    it under the terms of the GNU Lesser General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    Markus is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public License
*    along with Markus.  If not, see <http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------------------*/

#ifndef DEFINE_H
#define DEFINE_H

/// @file this file contains some usefull functions and methods

#define POW2(x) (x) * (x)
#define DIST(x, y) sqrt((x) * (x) + (y) * (y))
#define DIFF_REL(f) ((f).value - (f).mean) * ((f).value - (f).mean) / (f).sqVariance
#define CLEAN_DELETE(x) if((x) != NULL){delete((x));(x) = NULL;}

/// Macro to define the Class of an object (for method GetClass)
#define MKCLASS(cl) inline virtual const std::string& GetClass() const {const static std::string s = cl; return s;}
/// Macro to define the Type of an object (for method GetType)
#define MKTYPE(cl) inline virtual const std::string& GetType() const {const static std::string s = cl; return s;}

/// Disable copy constructor
#define DISABLE_COPY(Class) \
Class(const Class &); \
Class &operator=(const Class &);

// time stamp: use for all time stamps on frames in [ms]
#define TIME_STAMP unsigned long long
#define TIME_STAMP_INITIAL 0
#define TIME_STAMP_MIN -100000 // for initialization as well

// Logging functions
#define LOG_ERROR LOG4CXX_ERROR
#define LOG_WARN  LOG4CXX_WARN
#define LOG_INFO  LOG4CXX_INFO
#define LOG_DEBUG LOG4CXX_DEBUG

// Log function for tests only
#define LOG_TEST(logger, str) {\
	std::cout<<str<<std::endl;\
	LOG_INFO((logger), str);\
}

#define SYSTEM(x) {std::string cmd; cmd = (x);\
	if(system(cmd.c_str())) {\
	std::stringstream ss;\
	ss<<"Execution of command failed: "<<cmd;\
	throw MkException(ss.str(), LOC);}}

#endif
