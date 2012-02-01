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

#include "Module.h"
#include "ConfigReader.h"
#include "util.h"

#include "Stream.h"
#include "Control.h"
#include "Timer.h"

//#include <list>

using namespace std;

Module::Module(const ConfigReader& x_configReader) :
	Configurable(x_configReader)
{
	m_name 	= x_configReader.GetAttribute("name");
	m_id	= atoi(x_configReader.GetAttribute("id").c_str());
	cout<<endl<<"*** Create object Module : "<<m_name<<" id:"<<m_id<<" ***"<<endl;
	m_processingTime = 0;
	
	m_timerConvertion = 0;
	m_timerProcessing = 0;
	m_timerWaiting    = 0;
	m_countProcessedFrames = 0;
	
	// Add controls for parameters' change
	m_controls.push_back(new Control("Parameters", "Change the values of parameters at runtime."));
};


Module::~Module()
{
	// Delete all streams
	for(std::vector<Stream* >::iterator it = m_inputStreams.begin() ; it != m_inputStreams.end() ; it++)
		delete(*it);
	for(std::vector<Stream* >::iterator it = m_outputStreams.begin() ; it != m_outputStreams.end() ; it++)
		delete(*it);
	for(std::vector<Stream* >::iterator it = m_debugStreams.begin() ; it != m_debugStreams.end() ; it++)
		delete(*it);
};

void Module::Process(double x_timeCount)
{
	if(GetFps() == 0 || (m_processingTime += x_timeCount) > 1.0 / GetFps()) 
	{
		Timer ti;
	
		// Read and convert inputs
		if(IsInputUsed(x_timeCount))
		{
			for(vector<Stream*>::iterator it = m_inputStreams.begin() ; it != m_inputStreams.end() ; it++)
			{
				Timer ti2;
				(*it)->LockForRead();
				m_timerWaiting += ti2.GetMSecLong();
				(*it)->ConvertInput();
				(*it)->UnLock();
			}
		}
		m_timerConvertion 	+= ti.GetMSecLong();
		ti.Restart();
		
		ProcessFrame();
		
		m_timerProcessing 	 += ti.GetMSecLong();
		
		m_processingTime = 0;
		m_countProcessedFrames++;
	}
}


/// Describe the module with name, parameters, inputs, outputs to an output stream (in xml)

void Module::Export(ostream& rx_os, int x_indentation)
{
	string tabs(x_indentation, '\t');
	tabs = string(x_indentation, '\t');
	rx_os<<tabs<<"<module name=\""<<m_name<<"\" description=\""<<GetDescription()<<"\">"<<endl;
	tabs = string(x_indentation + 1, '\t');
	rx_os<<tabs<<"<parameters>"<<endl;
	for(vector<Parameter*>::const_iterator it = RefParameter().GetList().begin() ; it != RefParameter().GetList().end() ; it++)
		(*it)->Export(rx_os, x_indentation + 2);
	rx_os<<tabs<<"</parameters>"<<endl;

	rx_os<<tabs<<"<inputs>"<<endl;
	for(vector<Stream*>::const_iterator it = m_inputStreams.begin() ; it != m_inputStreams.end() ; it++)
		(*it)->Export(rx_os, x_indentation + 2, true);
	rx_os<<tabs<<"</inputs>"<<endl;

	rx_os<<tabs<<"<outputs>"<<endl;
	for(vector<Stream*>::const_iterator it = m_outputStreams.begin() ; it != m_outputStreams.end() ; it++)
		(*it)->Export(rx_os, x_indentation + 2, false);
	rx_os<<tabs<<"</outputs>"<<endl;
	tabs = string(x_indentation, '\t');
	rx_os<<tabs<<"</module>"<<endl;
}

/// Get a stream by its id
Stream* Module::GetInputStreamById(int x_id) const
{
	for(vector<Stream *>::const_iterator it = m_inputStreams.begin() ; it != m_inputStreams.end() ; it++)
		if((*it)->GetId() == x_id) return *it;
	assert(false);
	return NULL;
}

/// Get a stream by its id
Stream* Module::GetOutputStreamById(int x_id) const
{
	for(vector<Stream *>::const_iterator it = m_outputStreams.begin() ; it != m_outputStreams.end() ; it++)
		if((*it)->GetId() == x_id) return *it;
	assert(false);
	return NULL;
}

void Module::PrintStatistics(ostream& os) const
{
	os<<"Module "<<GetName()<<" : "<<m_countProcessedFrames<<" frames processed (tproc="<<m_timerProcessing<<"ms, tconv="<<m_timerConvertion<<"ms, twait="<<
		m_timerWaiting<<"ms)"<<endl;
}

