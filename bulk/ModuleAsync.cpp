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

ModuleAsync::ModuleAsync(const ConfigReader& x_configReader) :
	Module(x_configReader)
{
	cout<<endl<<"*** Create object ModuleAsync : "<<m_name<<" ***"<<endl;
	m_timeSinceLastThread 	= 1e99;
	m_resultsCopied 	= false;
	m_timerThread 		= 0;
	m_countFramesThread 	= 0;
};

/// Process one frame

void ModuleAsync::ProcessFrame()
{
	m_timeSinceLastThread += m_processingTime;
	//cout<<GetRefParameter().detectionFps<<" : "<<m_timeSinceLastThread<<" += "<<m_processingTime<<endl;
	
	if(!m_resultsCopied && !GetRefThread().isRunning())
	{
		CopyResults();
		m_resultsCopied = true;
	}
	
	if(m_timeSinceLastThread >= 1 / GetRefParameter().detectionFps)
	{
		if(!GetRefThread().isRunning())
		{
			LaunchThread();
			m_resultsCopied = false;
		}
		else
		{
			// thread is taking too long
			cout<<"Thread too slow, frame dropped after "<<m_timeSinceLastThread<<" [s]"<<endl;
		}
		m_timeSinceLastThread = 0;
	}
	NormalProcess();
}


ModuleAsync::~ModuleAsync()
{
};

void ModuleAsync::PrintStatistics(ostream& os) const
{
    Module::PrintStatistics(os);
    os<<m_countFramesThread<<" frames processed asynchronously in "<<m_timerThread<<"ms ("<<m_countFramesThread / (m_timerThread * 0.001)<<" fps)"<<endl;
}
