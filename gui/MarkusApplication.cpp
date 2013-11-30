
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

#include "MarkusApplication.h"
#include <opencv2/opencv.hpp>

using namespace std;

MarkusApplication::MarkusApplication(int &argc, char ** argv) : QApplication(argc, argv){}
// ~MarkusApplication();


//---------------------------------------------------------------------------------------------------- 
// Reimplement the notify function to manage exceptions
//---------------------------------------------------------------------------------------------------- 
bool MarkusApplication::notify(QObject *receiver_, QEvent *event_)
{
	try
	{
		return QApplication::notify(receiver_, event_);
	}
	catch(cv::Exception& e)
	{
		cout << "Exception raised (std::exception) : " << e.what() <<endl;
	}
	catch(std::exception& e)
	{
		cout << "Exception raised (std::exception) : " << e.what() <<endl;
			// test if all inputs are over
			/*if(EndOfAllStreams())
			{
				// throw MkException("End of all video streams : Manager::Process");
				LOG_WARN("End of all video streams : Manager::Process");
				continueFlag = false;
			} TODO implement something similar*/ 
		cout<<"Exception raised. Ending process"<<endl;
		exit(0);
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
	}

	return false;
}
