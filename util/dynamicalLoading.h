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

/// This file contains all functions used for the dynamical loading of modules
// link to article: http://www.linuxjournal.com/article/3687

#ifndef MARKUS_DYNAMICAL_LOADING_H
#define MARKUS_DYNAMICAL_LOADING_H

#include <iostream>
#include <map>
#include <list>
#include <vector>
#include <string>
#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>
#include <log4cxx/xml/domconfigurator.h>

#include "Factories.h"


// size of buffer for reading in directory entries
static unsigned int BUF_SIZE = 1024;
// our global factory for making shapes
std::map<std::string, FactoryModules::CreateObjectFunc *, std::less<std::string> > factory;

void dynamicalLoading()
{
	static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("dynamicalLoading"));

	const char *cmd = "find modules/ modules2/ -name \"*VideoFileReader.so\""; // TODO: All modules
	char in_buf[BUF_SIZE]; // string to get dynamic lib names
	std::list<void *> dl_list; // list to hold handles for dynamic libs
	std::vector<std::string> moduleNames;  // vector of module types used to build menu
	std::list<Module *> modulesList;  // list of module objects we create

	// get the names of all the dynamic libs (.so files) in the current dir
	FILE* dl = popen(cmd, "r");
	LOG_DEBUG(logger, "Searching dynamic libs with: " << cmd);
	if(!dl)
	{
		perror("popen");
		exit(-1);
	}
	while(fgets(in_buf, BUF_SIZE, dl))
	{
		char name[1024];
		// trim off the whitespace
		char *ws = strpbrk(in_buf, " \t\n");
		if(ws)
			*ws = '\0';
		// append ./ to the front of the lib name
		sprintf(name, "./%s", in_buf);
		LOG_DEBUG(logger, "Reading " << name);
		// void* dlib = dlopen(name, RTLD_NOW | RTLD_GLOBAL);
		void* dlib = dlopen(name, RTLD_LAZY);
		if(dlib == NULL)
		{
			LOG_ERROR(logger, "Error while loading module " << name << " dynamically: " << dlerror());
			exit(-1);
		}
		// add the handle to our list
		dl_list.insert(dl_list.end(), dlib);
	}
	// create an array of the module names
	for(auto fitr : factory)
	{
		moduleNames.insert(moduleNames.end(), fitr.first);
	}
}

#endif
