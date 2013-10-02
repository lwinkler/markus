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

	std::string configFile;
	configFile = "config.xml";
	int cpt = 1;
	while(cpt < argc)
	{
		if(!strcmp(argv[cpt], "-d"))
		{
			describe = true;
		}	
		else if(!strcmp(argv[cpt], "--no-gui"))
		{
			nogui = true;
			centralized = true;
		}
		/* else if(!strcmp(argv[cpt], "--centralized"))
		{
			centralized = true;
		}*/ 
		else
		{
			configFile = argv[cpt];
		}
		cpt++;
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

		if(describe) 
		{
			manager.Export();
			return 0;
		}

		if(nogui)
		{
			// No gui. launch the process directly
			// so far we cannot launch the process in a decentralized manner (with a timer on each module)
//#ifdef CENTRALIZE_PROCESS
			while(manager.Process()) // TODO: Find a better way to call Process. Through timers for example
			{
				// nothing 
			}
//#endif
			//for(int i = 0 ; i < nbCols * nbLines ; i++)
			//	m_moduleViewer[i]->update();
			return 0;
		}
		else
		{
#ifndef MARKUS_NO_GUI
			markus gui(mainConfig, manager);
			gui.setWindowTitle("OpenCV --> QtImage");
			gui.show();
			return app.exec();
#else
			LOG_ERROR("Markus was compiled without GUI. It can only be launched with option --no-gui");
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

