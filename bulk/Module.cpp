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


#include "ControllerModule.h"
#include "ControllerParameters.h"

using namespace std;

log4cxx::LoggerPtr Module::m_logger(log4cxx::Logger::getLogger("Module"));

Module::Module(const ConfigReader& x_configReader) :
	Configurable(x_configReader),
	m_name(x_configReader.GetAttribute("name"))
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
	m_processByTimer       = false;
	m_unsyncWarning        = true;

	m_moduleTimer = NULL;
}

void Module::Reset()
{
	// Lock the parameters that cannot be changed
	LOG_INFO(m_logger, "Reseting module "<<GetName());
	const ModuleParameterStructure& param(GetParameters());
	param.PrintParameters();
	param.CheckRange(true);

	// Add the module timer (only works with QT)
	if(param.autoProcess && m_processByTimer)
	{
		CLEAN_DELETE(m_moduleTimer);
		m_moduleTimer = new QModuleTimer(*this, 0);
		
		// An input will try to acquire frames as fast as possible
		// another module is called at the rate specified by the fps parameter
		if(IsInput())
			m_moduleTimer->Reset(100);
		else
			m_moduleTimer->Reset(param.fps);
	}
	else m_moduleTimer = NULL;
	// param.PrintParameters(); // Do not print 2x at startup

	// Add controls for parameters' change
	const std::vector<Parameter*>& list = param.GetList();

	// This must be done only once to avoid troubles in the GUI
	// Add module controller
	if(FindController("module") == NULL)
		AddController(new ControllerModule(*this));

	for(vector<Parameter*>::const_iterator it = list.begin(); it != list.end(); it++)
	{
		if((*it)->IsLocked() || FindController((*it)->GetName()) != NULL)
			continue;
		Controller* ctr = NULL;
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
		else AddController(ctr);
	}
}

void Module::Pause(bool x_pause)
{
	m_pause = x_pause;
}


/// Return the fps that can be used to recording. This value is special as it depends from preceeding modules. 

double Module::GetRecordingFps()
{
	double fps = GetParameters().fps;
	bool autop = GetParameters().autoProcess;

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
				throw MkException("First stream is null", LOC);
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
	for(std::map<int, Stream* >::iterator it = m_inputStreams.begin() ; it != m_inputStreams.end() ; it++)
		delete(it->second);
	for(std::map<int, Stream* >::iterator it = m_outputStreams.begin() ; it != m_outputStreams.end() ; it++)
		delete(it->second);
#ifdef MARKUS_DEBUG_STREAMS
	for(std::map<int, Stream* >::iterator it = m_debugStreams.begin() ; it != m_debugStreams.end() ; it++)
		delete(it->second);
#endif
	if(m_moduleTimer)
		delete(m_moduleTimer);
}

void Module::Process()
{
	m_lock.lockForWrite();
	if(m_pause)
	{
		m_lock.unlock();
		return;
	}
	try
	{
		if(!m_isReady)
			throw MkException("Module must be ready before processing", LOC);

		const ModuleParameterStructure& param = GetParameters();

		// Timestamp of the module is given by the input stream
		m_currentTimeStamp = 0;
		if(m_inputStreams.size() >= 1)
		{
			// m_inputStreams[0]->LockModuleForRead();
			m_currentTimeStamp = m_inputStreams[0]->GetTimeStampConnected(); // TODO: should we lock module here ?
			// m_inputStreams[0]->UnLockModule();
		}
		else if(! param.autoProcess)
			throw MkException("Module must have at least one input or have parameter auto_process=true", LOC);

		if(param.autoProcess || (param.fps == 0 && m_currentTimeStamp != m_lastTimeStamp) || (m_currentTimeStamp - m_lastTimeStamp) * param.fps > 1000)
		{
			// Process this frame

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
				for(map<int, Stream*>::iterator it = m_inputStreams.begin() ; it != m_inputStreams.end() ; it++)
				{
					Timer ti2;
					//(*it)->LockModuleForRead();
					m_timerWaiting += ti2.GetMSecLong();
					it->second->ConvertInput();
					//(*it)->UnLockModule();
				}
				//assert(m_currentTimeStamp == m_inputStreams[0]->GetTimeStamp());
			}
			m_timerConvertion 	+= ti.GetMSecLong();
			ti.Restart();

			ProcessFrame();

			m_timerProcessing 	 += ti.GetMSecLong();

			// Propagate time stamps to outputs
			for(map<int, Stream*>::iterator it = m_outputStreams.begin() ; it != m_outputStreams.end() ; it++)
				it->second->SetTimeStamp(m_currentTimeStamp);

			// Call depending modules (modules with fps = 0)
			for(vector<Module*>::iterator it = m_modulesDepending.begin() ; it != m_modulesDepending.end() ; it++)
				(*it)->Process();

			m_countProcessedFrames++;
			m_lastTimeStamp = m_currentTimeStamp;
		}
	}
	catch(...)
	{
		m_lock.unlock();
		throw;
	}
	m_lock.unlock();
}


/// Describe the module with name, parameters, inputs, outputs to an output stream (in xml)

void Module::Export(ostream& rx_os, int x_indentation)
{
	string tabs(x_indentation, '\t');
	tabs = string(x_indentation, '\t');
	rx_os<<tabs<<"<module name=\""<<m_name<<"\" description=\""<<GetDescription()<<"\">"<<endl;
	tabs = string(x_indentation + 1, '\t');
	rx_os<<tabs<<"<parameters>"<<endl;
	for(vector<Parameter*>::const_iterator it = GetParameters().GetList().begin() ; it != GetParameters().GetList().end() ; it++)
		(*it)->Export(rx_os, x_indentation + 2);
	rx_os<<tabs<<"</parameters>"<<endl;

	rx_os<<tabs<<"<inputs>"<<endl;
	for(map<int, Stream*>::const_iterator it = m_inputStreams.begin() ; it != m_inputStreams.end() ; it++)
		it->second->Export(rx_os, x_indentation + 2, true);
	rx_os<<tabs<<"</inputs>"<<endl;

	rx_os<<tabs<<"<outputs>"<<endl;
	for(map<int, Stream*>::const_iterator it = m_outputStreams.begin() ; it != m_outputStreams.end() ; it++)
		it->second->Export(rx_os, x_indentation + 2, false);
	rx_os<<tabs<<"</outputs>"<<endl;
	tabs = string(x_indentation, '\t');
	rx_os<<tabs<<"</module>"<<endl;
}

/// Get a stream by its id
Stream& Module::RefInputStreamById(int x_id)
{
	//for(vector<Stream *>::const_iterator it = m_inputStreams.begin() ; it != m_inputStreams.end() ; it++)
		//if((*it)->GetId() == x_id) return **it;
	map<int, Stream*>::iterator it = m_inputStreams.find(x_id);

	if(it == m_inputStreams.end())
	{
		stringstream ss;
		ss<<"GetInputStreamById : no stream with id="<<x_id<<" for module "<<GetName();
		throw MkException(ss.str(), LOC);
	}
	return *(it->second);
}

/// Get a stream by its id
Stream& Module::RefOutputStreamById(int x_id)
{
	map<int, Stream*>::iterator it = m_outputStreams.find(x_id);

	if(it == m_outputStreams.end())
	{
		stringstream ss;
		ss<<"GetInputStreamById : no stream with id="<<x_id<<" for module "<<GetName();
		throw MkException(ss.str(), LOC);
	}
	return *(it->second);
}

/// Print all statistics related to the module
void Module::PrintStatistics() const
{
	LOG_INFO(m_logger, "Module "<<GetName()<<" : "<<m_countProcessedFrames<<" frames processed (tproc="<<
		m_timerProcessing<<"ms, tconv="<<m_timerConvertion<<"ms, twait="<<
		m_timerWaiting<<"ms), "<< (m_countProcessedFrames * 1000.0 / (m_timerProcessing + m_timerConvertion + m_timerWaiting))<<" fps");
}

/// Dump the last inputs and outputs to disk
void Module::WriteStateToDirectory(const string& x_directory) const
{
	// string dir = OutputDir() + "/dump_" + timeStamp();
	
	// Dump inputs
	string fileName = x_directory + "/" + GetName() + ".module.json";
	ofstream of;
	of.open(fileName.c_str());
	for(map<int, Stream*>::const_iterator it = m_inputStreams.begin() ; it != m_inputStreams.end() ; it++)
	{
		if(it->second->IsConnected())
			it->second->Serialize(of, x_directory);
	}
	// Dump outputs
	for(map<int, Stream*>::const_iterator it = m_outputStreams.begin() ; it != m_outputStreams.end() ; it++)
	{
		if(it->second->IsConnected())
			it->second->Serialize(of, x_directory);
	}
	// Dump debugs
	for(map<int, Stream*>::const_iterator it = m_debugStreams.begin() ; it != m_debugStreams.end() ; it++)
	{
		if(it->second->IsConnected())
			it->second->Serialize(of, x_directory);
	}
	of.close();
}

/// Check that all inputs are ready (they are connected to a working module)
bool Module::AllInputsAreReady() const
{
	for(map<int, Stream*>::const_iterator it = m_inputStreams.begin() ; it != m_inputStreams.end() ; it++)
	{
		if(it->second->IsConnected() && !it->second->RefConnected().IsReady())
			return false;
	}
	return true;
}

/// Return a reference to the master module
const Module& Module::GetMasterModule()
{
	for(map<int, Stream*>::const_iterator it = m_inputStreams.begin() ; it != m_inputStreams.end() ; it++)
	{
		if(it->second->IsConnected())
		{
			Module& preceding = it->second->RefConnected().RefModule();
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
	for(map<int, Stream*>::iterator it = m_outputStreams.begin() ; it != m_outputStreams.end() ; it++)
	{
		it->second->SetAsReady();
	}
}


/// Add an input stream
void Module::AddInputStream(int x_id, Stream* xp_stream)
{
	// m_inputStreams.push_back(xp_stream);
	xp_stream->SetId(x_id);
	if(m_inputStreams.find(x_id) != m_inputStreams.end())
		throw MkException("Two streams with same id", LOC);
	m_inputStreams.insert(make_pair(x_id, xp_stream));
}

/// Add an input stream
void Module::AddOutputStream(int x_id, Stream* xp_stream)
{
	xp_stream->SetId(x_id);
	if(m_outputStreams.find(x_id) != m_outputStreams.end())
		throw MkException("Two streams with same id", LOC);
	m_outputStreams.insert(make_pair(x_id, xp_stream));
}

/// Add an input stream
void Module::AddDebugStream(int x_id, Stream* xp_stream)
{
	xp_stream->SetId(x_id);
	if(m_debugStreams.find(x_id) != m_debugStreams.end())
		throw MkException("Two streams with same id", LOC);
	m_debugStreams.insert(make_pair(x_id, xp_stream));
}

