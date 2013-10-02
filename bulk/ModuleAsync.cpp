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
#include "Stream.h"

#include <list>

using namespace std;

ModuleAsync::ModuleAsync(const ConfigReader& x_configReader) :
	Module(x_configReader)
{
	LOG_INFO("*** Create object ModuleAsync : "<<m_name<<" ***");
	m_timeStampLastThread 	= TIME_STAMP_MIN;
	m_resultsCopied 	= false;
	m_timerThread 		= 0;
	m_countFramesThread 	= 0;
};

/// Process one frame

void ModuleAsync::ProcessFrame()
{
	if(m_inputStreams.size() < 1)
		throw("Error: Module must have at least one input or inherit from class Input in ModuleAsync::Process");

	// cout << "m_timeStampLastThread" << m_timeStampLastThread;
	// cout << " m_currentTimeStamp " <<  m_currentTimeStamp << endl;
	//cout<<RefParameter().detectionFps<<" : "<<m_timeStampLastThread<<" += "<<m_processingTime<<endl;
	
	if(!m_resultsCopied && !GetRefThread().isRunning())
	{
		CopyResults();
		m_resultsCopied = true;
	}
	
	if((m_currentTimeStamp - m_timeStampLastThread) * RefParameter().detectionFps >= 1000)
	{
		if(!GetRefThread().isRunning())
		{
			LaunchThread();
			m_resultsCopied = false;
		}
		else
		{
			// thread is taking too long
			LOG_WARNING("Thread too slow, frame dropped after "<<m_currentTimeStamp - m_timeStampLastThread<<" [ms]");
		}
		m_timeStampLastThread = m_currentTimeStamp;
	}
	NormalProcess();
}


ModuleAsync::~ModuleAsync()
{
};

void ModuleAsync::PrintStatistics(ostream& os) const
{
	Module::PrintStatistics(os);
	os<<"\t"<<m_countFramesThread<<" frames processed asynchronously in "<<m_timerThread<<"ms ("<<m_countFramesThread / (m_timerThread * 0.001)<<" fps)"<<endl;
}
