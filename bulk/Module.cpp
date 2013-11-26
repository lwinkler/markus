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
#include "Timer.h"
#include "ModuleTimer.h"


#ifndef MARKUS_NO_GUI
#include "ParameterControl.h"
#endif

using namespace std;

Module::Module(const ConfigReader& x_configReader) :
	Configurable(x_configReader),
	m_name(x_configReader.GetAttribute("name")),
	m_logger(log4cxx::Logger::getLogger(m_name))
{
	m_id	= atoi(x_configReader.GetAttribute("id").c_str()); // TODO: remove id ?
	LOG4CXX_INFO(m_logger, "Create object " << m_name);
	
	m_timerConvertion      = 0;
	m_timerProcessing      = 0;
	m_timerWaiting         = 0;
	m_countProcessedFrames = 0;
	m_modulePreceeding     = NULL;
	m_lastTimeStamp        = TIME_STAMP_MIN;
	m_currentTimeStamp     = TIME_STAMP_INITIAL;
	m_pause                = false;

	// Add controls for parameters' change
#ifndef MARKUS_NO_GUI
	m_controls.push_back(new ParameterControl("Parameters", "Change the values of parameters at runtime."));
#endif
	m_moduleTimer = NULL;
}

void Module::Reset()
{
	// Add the module timer (only works with QT)
	if(RefParameter().autoProcess)
	{
		if(m_moduleTimer)
			delete(m_moduleTimer);
		m_moduleTimer = new QModuleTimer(*this, 0);
		m_moduleTimer->Reset(RefParameter().fps);
	}
	else m_moduleTimer = NULL;
	stringstream ss;
	RefParameter().PrintParameters();
	LOG4CXX_INFO(m_logger, ss);
}

void Module::Pause(bool x_pause)
{
	m_pause = x_pause;
}


/// Return the fps that can be used to recording. This value is special as it depends from preceeding modules. 

double Module::GetRecordingFps()
{
	double fps = RefParameter().fps;
	bool autop = RefParameter().autoProcess;

	if(autop)
	{
		// If the module is autoprocessed then the FPS is determining
		assert(fps != 0);
		return fps;
	}
	else
	{
		// If the module is not autoprocessed then the FPS is given by the previous module
		// Note: we assume that the fps is given by the first stream in the module
		assert(m_inputStreams.size() > 0);
		if(fps == 0)
		{
			Stream * stream = m_inputStreams.at(0);
			if(stream == NULL)
				throw MkException("First stream is null in Module::GetRecordingFps", LOC);
			stream = &(stream->RefConnected());
			if(stream == NULL)
				throw MkException("Connected stream is null in Module::GetRecordingFps", LOC);
			return stream->RefModule().GetRecordingFps();
		}
		else
		{
			// estimate the fps to the min of input and current
			return MIN(fps, m_inputStreams.at(0)->RefConnected().RefModule().GetRecordingFps());
		}
	}

}

Module::~Module()
{
	// Delete all streams
	for(std::vector<Stream* >::iterator it = m_inputStreams.begin() ; it != m_inputStreams.end() ; it++)
		delete(*it);
	for(std::vector<Stream* >::iterator it = m_outputStreams.begin() ; it != m_outputStreams.end() ; it++)
		delete(*it);
#ifdef MARKUS_DEBUG_STREAMS
	for(std::vector<Stream* >::iterator it = m_debugStreams.begin() ; it != m_debugStreams.end() ; it++)
		delete(*it);
#endif
#ifndef MARKUS_NO_GUI
	for(std::vector<ControlBoard* >::iterator it = m_controls.begin() ; it != m_controls.end() ; it++)
		delete(*it);
#endif

	if(m_moduleTimer)
		delete(m_moduleTimer);
}

void Module::Process()
{
	if(m_pause)
		return; // TODO: Manage pause more elegantly
	//try
	{
		const ModuleParameterStructure& param = RefParameter();
		
		// Timestamp of the module is given by the input stream // TODO: How to manage if there are several streams
		m_currentTimeStamp = 0;
		if(m_inputStreams.size() >= 1)
			m_currentTimeStamp = m_inputStreams[0]->GetTimeStampConnected();
		else if(! param.autoProcess)
			throw MkException("Error: Module must have at least one input or have parameter auto_process=true in Module::Process", LOC);

		if(param.autoProcess || param.fps == 0 || (m_currentTimeStamp - m_lastTimeStamp) * param.fps > 1000)
		{
			// Process this frame
			m_lock.lockForRead();
			
			// Timer for benchmark
			Timer ti;

			// Read and convert inputs
			if(IsInputProcessed())
			{
				for(vector<Stream*>::iterator it = m_inputStreams.begin() ; it != m_inputStreams.end() ; it++)
				{
					Timer ti2;
					(*it)->LockModuleForRead();
					m_timerWaiting += ti2.GetMSecLong();
					(*it)->ConvertInput();
					(*it)->UnLockModule();
				}
			}
			m_timerConvertion 	+= ti.GetMSecLong();
			ti.Restart();

			ProcessFrame();

			m_timerProcessing 	 += ti.GetMSecLong();

			// Set time stamps to outputs
			if(!IsInput())
				for(vector<Stream*>::iterator it = m_outputStreams.begin() ; it != m_outputStreams.end() ; it++)
				(*it)->SetTimeStamp(m_currentTimeStamp);
			
			// Call depending modules (modules with fps = 0)
			for(vector<Module*>::iterator it = m_modulesDepending.begin() ; it != m_modulesDepending.end() ; it++)
				(*it)->Process();

			m_countProcessedFrames++;
			m_lastTimeStamp = m_currentTimeStamp;
			m_lock.unlock();
		}
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
	throw MkException(string("GetInputStreamById : no stream with this id for module ") + string(GetName()), LOC);
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

void Module::PrintStatistics() const
{
	LOG4CXX_INFO(m_logger, "Module "<<GetName()<<" : "<<m_countProcessedFrames<<" frames processed (tproc="<<
		m_timerProcessing<<"ms, tconv="<<m_timerConvertion<<"ms, twait="<<
		m_timerWaiting<<"ms), "<< (m_countProcessedFrames * 1000.0 / (m_timerProcessing + m_timerConvertion + m_timerWaiting))<<" fps");
}

