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

#include <QtGui/QApplication>
#include <cstring>
#include <cstdio>
#include <getopt.h>    /* for getopt_long; standard getopt is in unistd.h */

#include "Manager.h"

#ifndef MARKUS_NO_GUI
#include "markus.h"
#include "MarkusApplication.h"
#endif

#include <iostream>

using namespace std;

void usage()
{
	printf("Usage : markus --no-gui --centralized <configuration> [-d]\n");
	printf("     --no-gui          Run process without gui\n");
	// printf("     --centralized     Call all modules processing methods in a centralized way\n");
}

int main(int argc, char** argv)
{
	bool describe    = false;
	bool nogui       = false;
	bool centralized = false;

	string configFile = "config.xml";
	vector<string> parameters;

	MkLog::log.ShowDebug(true);// TODO Add a param for this

	// Read arguments

	//int digit_optind = 0;
	static struct option long_options[] = {
		{"help",        0, 0, 'h'},
		{"version",     0, 0, 'v'},
		{"describe",    0, 0, 'd'},
		{"centralized", 0, 0, 'c'},
		{"no-gui",      0, 0, 'n'},
		{"parameter",   1, 0, 'p'},
		{NULL, 0, NULL, 0}
	};
	int c;
	int option_index = 0;
	while ((c = getopt_long(argc, argv, "hvcnp:",
					long_options, &option_index)) != -1) {
		// int this_option_optind = optind ? optind : 1;
		switch (c) {
			case 'h':
				usage();
				break;
			case 'v':
				LOG_ERROR("version TODO b\n");
				return 0;
			case 'd':
				describe = true;
				printf("desccribe");
				break;
			case 'c':
				centralized = true;
				printf("centralized");
				break;
			case 'n':
				nogui = true;
				break;
			case 'p':
				parameters.push_back(optarg);
				break;
			case '?':
				break;
			default:
				LOG_ERROR("Unknown parameter "<<c);
				return -1;
		}
	}



	if (optind == argc - 1) {
		configFile = argv[argc - 1];
	}
	else if(optind == argc)
	{
		LOG_INFO("Using default configuration file "<<configFile);
	}
	else 
	{
		LOG_ERROR("Invalid number of arguments "<<(argc - optind));
		return -1;
	}

	try
	{
#ifndef MARKUS_NO_GUI
		MarkusApplication app(argc, argv);
#endif
		ConfigReader mainConfig(configFile);
		ConfigReader appConfig = mainConfig.SubConfig("application");
		assert(!appConfig.IsEmpty());
		Manager manager(appConfig, centralized);

		for(vector<string>::const_iterator it = parameters.begin() ; it != parameters.end() ; it++)
		{
			vector<string> elems;
			split(*it, '=', elems);
			if(elems.size() != 2)
				throw("Badly formed parameter in main");
			vector<string> path;
			split(elems[0], '.', path);
			if(path.size() != 2)
				throw("Badly formed parameter in main");
			manager.GetModuleByName(path[0])->RefParameter().RefParameterByName(path[1]).SetValue(elems[1], PARAMCONF_CMD);
		}

		if(describe) 
		{
			manager.Export();
			return 0;
		}

		if(centralized)
		{
			// No gui. launch the process directly
			// so far we cannot launch the process in a decentralized manner (with a timer on each module)
			while(manager.Process()) // TODO: Find a better way to call Process. Through timers for example
			{
				// nothing 
			}
			//for(int i = 0 ; i < nbCols * nbLines ; i++)
			//	m_moduleViewer[i]->update();
			return 0;
		}
		else
		{
#ifndef MARKUS_NO_GUI
			markus gui(mainConfig, manager);
			gui.setWindowTitle("OpenCV --> QtImage");
			if(!nogui)
				gui.show();
			return app.exec(); // TODO: Manage case where centralized with GUI
#else
			LOG_ERROR("Markus was compiled without GUI. It can only be launched with option --centralized");
			return -1;
#endif
		}
	}
	catch(cv::Exception& e)
	{
		LOG_ERROR("Exception raised (std::exception) : " << e.what() );
	}
	catch(std::exception& e)
	{
		LOG_ERROR("Exception raised (std::exception) : " << e.what() );
	}
	catch(std::string str)
	{
		LOG_ERROR("Exception raised (string) : " << str );
	}
	catch(const char* str)
	{
		LOG_ERROR("Exception raised (const char*) : " << str );
	}
	catch(...)
	{
		LOG_ERROR("Unknown exception raised");
	}
	return -1;
}

