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
#include "markus.h"
#include <cstring>
#include <cstdio>

#include "Manager.h"

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
	// try
	{
		QApplication app(argc, argv);
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
			while(true)
				manager.Process();
//#endif
			//for(int i = 0 ; i < nbCols * nbLines ; i++)
			//	m_moduleViewer[i]->update();
			return 0;
		}
		else
		{
	
			markus gui(mainConfig, manager, centralized);
			gui.setWindowTitle("OpenCV --> QtImage");
			gui.show();
			return app.exec();
		}
	}
	/*catch(cv::Exception& e)
	{
		cout << "Exception raised (std::exception) : " << e.what() <<endl;
	}
	catch(std::exception& e)
	{
		cout << "Exception raised (std::exception) : " << e.what() <<endl;
	}
	catch(std::string str)
	{
		cout << "Exception raised (string) : " << str <<endl;
	}
	catch(const char* str)
	{
		cout << "Exception raised (const char*) : " << str <<endl;
	}
	catch(...)
	{
		cout << "Unknown exception raised: "<<endl;
	} */
	return -1;
}
