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
#include "ControllerModule.h"
#include "ControllerParameters.h"
#endif

using namespace std;

Module::Module(const ConfigReader& x_configReader) :
	Configurable(x_configReader),
	m_name(x_configReader.GetAttribute("name")),
	m_logger(log4cxx::Logger::getLogger(m_name))
{
	m_id	= atoi(x_configReader.GetAttribute("id").c_str());
	LOG_INFO(m_logger, "Create object " << m_name);
	
	m_timerConvertion      = 0;
	m_timerProcessing      = 0;
	m_timerWaiting         = 0;
	m_countProcessedFrames = 0;
	m_lastTimeStamp        = TIME_STAMP_MIN;
	m_currentTimeStamp     = TIME_STAMP_INITIAL;
	m_pause                = false;
	m_isReady              = false;
	m_unsyncWarning        = true;

	m_moduleTimer = NULL;
}

void Module::Reset()
{
	// Lock the parameters that cannot be changed
	const ModuleParameterStructure& param(RefParameter());
	param.PrintParameters(m_logger);
	param.CheckRange();

	// Add the module timer (only works with QT)
	if(param.autoProcess)
	{
		if(m_moduleTimer)
			delete(m_moduleTimer);
		m_moduleTimer = new QModuleTimer(*this, 0);
		m_moduleTimer->Reset(param.fps);
	}
	else m_moduleTimer = NULL;
	// param.PrintParameters(); // Do not print 2x at startup

	// Add controls for parameters' change
#ifndef MARKUS_NO_GUI
	const std::vector<Parameter*>& list = param.GetList();
	// for(const vector<Parameter*>&::iterator it = list.begin() ; it != list.end() ; it++)
		// m_controls.push_back(new ParameterControl("Parameters", "Change the values of parameters at runtime."));

	// Delete all controllers
	/*for(vector<Controller*>::iterator it = m_controls.begin() ; it != m_controls.end() ; it++)
	{
		delete(*it);
	}
	m_controls.clear();
	*/
	// This must be done only once to avoid troubles in the GUI
	if(m_controls.size() == 0)
	{
		// Add module controller
		Controller* ctr = new ControllerModule(*this);
		m_controls.insert(std::make_pair(ctr->GetName(), ctr));

		for(vector<Parameter*>::const_iterator it = list.begin(); it != list.end(); it++)
		{
			if((*it)->IsLocked())
				continue;
			ctr = NULL;
			switch((*it)->GetType())
			{
				case PARAM_BOOL:
					ctr = new ControllerBool(*dynamic_cast<ParameterBool*>(*it));
					break;
				case PARAM_DOUBLE:
					ctr = new ControllerDouble(*dynamic_cast<ParameterDouble*>(*it));
					break;
				case PARAM_FLOAT:
					ctr = new ControllerFloat(*dynamic_cast<ParameterFloat*>(*it));
					break;
				case PARAM_IMAGE_TYPE:
					ctr = new ControllerEnum(*dynamic_cast<ParameterEnum*>(*it)); 
					break;
				case PARAM_INT:
					ctr = new ControllerInt(*dynamic_cast<ParameterInt*>(*it));
					break;
				case PARAM_STR:
					ctr = new ControllerString(*dynamic_cast<ParameterString*>(*it));
					break;
			}
			if(ctr == NULL)
				throw MkException("Controller creation failed", LOC);
			// else m_controls.push_back(ctr);
			else m_controls.insert(make_pair(ctr->GetName(), ctr));
		}
	}
#endif
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
		if(fps == 0)
			throw MkException("FPS cannot be equal to zero", LOC);
		return fps;
	}
	else
	{
		// If the module is not autoprocessed then the FPS is given by the previous module
		// Note: we assume that the fps is given by the first stream in the module
		if(m_inputStreams.size() == 0)
			throw MkException("This module must have at least one input stream", LOC);
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
	for(std::map<string, Controller* >::iterator it = m_controls.begin() ; it != m_controls.end() ; it++)
		delete(it->second);
#endif

	if(m_moduleTimer)
		delete(m_moduleTimer);
}

void Module::Process()
{
	if(m_pause)
		return;
	if(!m_isReady)
		throw MkException("Module must be ready before processing", LOC);

	const ModuleParameterStructure& param = RefParameter();
	
	// Timestamp of the module is given by the input stream
	m_currentTimeStamp = 0;
	if(m_inputStreams.size() >= 1)
	{
		m_currentTimeStamp = m_inputStreams[0]->GetTimeStampConnected(); // TODO: should we lock module here ?
	}
	else if(! param.autoProcess)
		throw MkException("Module must have at least one input or have parameter auto_process=true", LOC);

	if(param.autoProcess || (param.fps == 0 && m_currentTimeStamp != m_lastTimeStamp) || (m_currentTimeStamp - m_lastTimeStamp) * param.fps > 1000)
	{
		// Process this frame
		m_lock.lockForRead();
		
		// Timer for benchmark
		Timer ti;

		// cout<<GetName()<<" "<<m_currentTimeStamp<<" "<<m_lastTimeStamp<<endl;

		// Check that all inputs are in sync
		if(! param.allowUnsyncInput && m_unsyncWarning)
			for(unsigned int i = 1 ; i < m_inputStreams.size() ; i++)
			{
				Stream& stream(*m_inputStreams.at(i));
				if(stream.IsConnected() && stream.GetTimeStampConnected() != m_currentTimeStamp)
				{
					LOG_WARN(m_logger, "Input stream id="<<i<<" is not in sync with input stream id=0 for module "<<GetName()<<". If this is acceptable set parameter allow_unsync_input=1 for this module. To fix this problem cleanly you should probably set parameters auto_process=0 and fps=0 for the modules located between the input and module "<<GetName()<<".");
					m_unsyncWarning = false;
				}
			}

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
Stream& Module::RefInputStreamById(int x_id) const
{
	for(vector<Stream *>::const_iterator it = m_inputStreams.begin() ; it != m_inputStreams.end() ; it++)
		if((*it)->GetId() == x_id) return **it;
	stringstream ss;
	ss<<"GetInputStreamById : no stream with id="<<x_id<<" for module "<<GetName();
	throw MkException(ss.str(), LOC);
	// return NULL;
}

/// Get a stream by its id
Stream& Module::RefOutputStreamById(int x_id) const
{
	for(vector<Stream *>::const_iterator it = m_outputStreams.begin() ; it != m_outputStreams.end() ; it++)
		if((*it)->GetId() == x_id) return **it;

	stringstream ss;
	ss<<"Input stream not found module="<<GetName()<<" id="<<x_id;
	throw MkException(ss.str(), LOC);
	// return NULL;
}

void Module::PrintStatistics() const
{
	LOG_INFO(m_logger, "Module "<<GetName()<<" : "<<m_countProcessedFrames<<" frames processed (tproc="<<
		m_timerProcessing<<"ms, tconv="<<m_timerConvertion<<"ms, twait="<<
		m_timerWaiting<<"ms), "<< (m_countProcessedFrames * 1000.0 / (m_timerProcessing + m_timerConvertion + m_timerWaiting))<<" fps");
}

/// Check that all inputs are ready (they are connected to a working module)
bool Module::AllInputsAreReady() const
{
	for(vector<Stream*>::const_iterator it = m_inputStreams.begin() ; it != m_inputStreams.end() ; it++)
	{
		if((*it)->IsConnected() && !(*it)->RefConnected().IsReady())
			return false;
	}
	return true;
}

/// Return a reference to the master module
const Module& Module::GetMasterModule()
{
	for(vector<Stream*>::const_iterator it = m_inputStreams.begin() ; it != m_inputStreams.end() ; it++)
	{
		if((*it)->IsConnected())
		{
			Module& preceding = (*it)->RefConnected().RefModule();
			if(preceding.IsAutoProcessed())
				return preceding;
			else
				return preceding.GetMasterModule();
		}
	}
	throw MkException("Module must have at least one input connected or have auto_process=1", LOC);
}

/// Set as ready (and all inputs too)
void Module::SetAsReady()
{
	m_isReady = true;
	for(vector<Stream*>::iterator it = m_outputStreams.begin() ; it != m_outputStreams.end() ; it++)
	{
		(*it)->SetAsReady();
	}
}

/// find a controller in map by name
Controller* Module::FindController(const std::string& x_name) const
{
	map<string, Controller*>::const_iterator it = m_controls.find(x_name);
	if(it == m_controls.end())
		throw MkException("Cannot find controller in module", LOC);

	// Call the function pointer associated with the action
	return it->second;
}


