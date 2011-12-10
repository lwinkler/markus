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

#include "ModuleAsync.h"
#include "ConfigReader.h"

#include <list>

using namespace std;

ModuleAsync::ModuleAsync(const std::string& x_name, ConfigReader& x_configReader) :
	Module(x_name, x_configReader)
	//m_name(x_name)
{
	cout<<endl<<"*** Create object ModuleAsync : "<<x_name<<" ***"<<endl;
	m_threadStatus = ThreadStatusWaiting;
};

void ModuleAsync::ProcessFrame(const IplImage* m_input, const double x_timeSinceLastProcessing)
{
/*	if(m_threadStatus == ThreadStatusRunning)
	{
		
	}
	else if(m_threadStatus == ThreadStatusWaiting)
	{
		
	}
*/
	
	PreProcess(m_input, x_timeSinceLastProcessing);
	ThreadProcess(m_input, x_timeSinceLastProcessing);
	PostProcess(m_input, x_timeSinceLastProcessing);

}


ModuleAsync::~ModuleAsync()
{
	//TODO : delete m_outputStreams
};

/*void ModuleAsync::AddStream(const std::string& x_name, StreamType x_type, IplImage* x_image)
{
	m_outputStreams.push_back(new OutputStream(x_name, x_type, x_image));
}*/
