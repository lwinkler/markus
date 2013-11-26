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

#include "Manager.h"
#include "Module.h"
#include "Input.h"
#include "Stream.h"
#include "MkException.h"

#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <assert.h>

#include <sstream>
#include <string.h>
using namespace std;

#include "AllModules.h"

	
#if defined( WIN32 ) && defined( TUNE )
	#include <crtdbg.h>
	_CrtMemState startMemState;
	_CrtMemState endMemState;
#endif


using namespace std;


Manager::Manager(ConfigReader& x_configReader, bool x_centralized) : 
	Configurable(x_configReader),
	m_param(m_configReader, "Manager"),
	m_centralized(x_centralized),
	m_logger(log4cxx::Logger::getLogger("manager"))
{
	LOG4CXX_INFO(m_logger, "Create object Manager");
	m_frameCount = 0;
	
	
	m_inputs.clear();
	m_modules.clear();
	
	// Read the configuration of each module
	ConfigReader moduleConfig = m_configReader.GetSubConfig("module");
	
	while(! moduleConfig.IsEmpty())
	{
		// Read parameters
		if( moduleConfig.GetSubConfig("parameters").IsEmpty()) 
			throw MkException("Impossible to find <parameters> section for module " +  moduleConfig.GetAttribute("name"), LOC);
		Module * tmp1 = createNewModule( moduleConfig);		
		
		// Add to inputs if an input
		m_modules.push_back(tmp1);
		if(tmp1->IsInput())
			m_inputs.push_back(dynamic_cast<Input* >(tmp1));
		moduleConfig = moduleConfig.NextSubConfig("module");
	}

	
	// Connect input and output streams (re-read the config once since we need all modules to be connected)
	moduleConfig = m_configReader.GetSubConfig("module");
	
	while(! moduleConfig.IsEmpty())
	{
			// Read conections of inputs
			ConfigReader conf = moduleConfig.GetSubConfig("inputs");
			if(!conf.IsEmpty())
			{
				ConfigReader inputConfig = conf.GetSubConfig("input");
				while(! inputConfig.IsEmpty())
				{
					int moduleId		= atoi(moduleConfig.GetAttribute("id").c_str());
					int inputId 		= atoi(inputConfig.GetAttribute("id").c_str());
					int outputModuleId 	= atoi(inputConfig.GetAttribute("moduleid").c_str());
					int outputId 		= atoi(inputConfig.GetAttribute("outputid").c_str());

					Stream * inputStream  = GetModuleById(moduleId)->GetInputStreamById(inputId);
					Stream * outputStream = GetModuleById(outputModuleId)->GetOutputStreamById(outputId);

					inputStream->Connect(outputStream);

					inputConfig = inputConfig.NextSubConfig("input");
				}
			}
		moduleConfig = moduleConfig.NextSubConfig("module");
	}

	// Set the module preceeding the current module
	for(vector<Module*>::iterator it = m_modules.begin() ; it != m_modules.end() ; it++)
	{
		try
		{
			Stream& stream = (*it)->GetInputStreamById(0)->RefConnected();
			if(&stream == NULL)
				throw MkException("Module must have at least one stream", LOC);
			Module& preceeding(stream.RefModule());
			(*it)->SetPreceedingModule(preceeding);
			if((*it)->RefParameter().autoProcess == false)
				preceeding.AddDependingModule(**it); // TODO: find a better way to execute non real time modules
		}
		catch(...){}
	}

}

Manager::~Manager()
{
	PrintTimers();
	for(vector<Module*>::iterator it = m_modules.begin() ; it != m_modules.end() ; it++)
		delete *it;
	Global::Infos();
}


/// Reset the manager: must be called externally after initialization
void Manager::Reset()
{
	// Reset timers
	// m_timerConvertion = 0;
	m_timerProcessing = 0;

	// Reset all modules (to set the module timer)
	for(vector<Module*>::iterator it = m_modules.begin() ; it != m_modules.end() ; it++)
	{
		(*it)->Reset();
	}
}

/// Process all modules

bool Manager::Process()
{
	// If methods are not called in a centralized way, we will rely on timers on each module
	assert(m_centralized);

	if(!m_lock.tryLockForWrite())
	{
		LOG4CXX_WARN(m_logger, "Manager too slow !");
		return true;
	}
	Timer ti;
	
	bool continueFlag = true;
	for(vector<Module*>::iterator it = m_modules.begin() ; it != m_modules.end() ; it++)
	{
		try
		{
			if((*it)->RefParameter().autoProcess)
				(*it)->Process();
		}
		/*catch(cv::Exception& e)
		{
			LOG4CXX_ERROR(m_logger, (*it)->GetName() << ": Exception raised (std::exception) : " << e.what());
		}*/
		catch(EndOfStreamException& e)
		{
			// TODO: Catch end of stream exception
			LOG4CXX_WARN(m_logger, (*it)->GetName() << ": Exception raised (EndOfStream) : " << e.what());

			// test if all inputs are over
			if(EndOfAllStreams())
			{
				// throw("End of all video streams : Manager::Process");
				LOG4CXX_WARN(m_logger, "End of all video streams : Manager::Process");
				continueFlag = false;
			}
		}
		catch(std::exception e)
		{
			LOG4CXX_ERROR(m_logger, (*it)->GetName() << ": Exception raised (std::exception): " << e.what());
		}
		catch(std::string str)
		{
			LOG4CXX_ERROR(m_logger, (*it)->GetName() << ": Exception raised (string): " << str);
		}
		catch(const char* str)
		{
			LOG4CXX_ERROR(m_logger, (*it)->GetName() << ": Exception raised (const char*): " << str);
		}
		catch(...)
		{
			LOG4CXX_ERROR(m_logger, (*it)->GetName() << ": Unknown exception raised");
		}
	}

	m_timerProcessing += ti.GetMSecLong();
	m_frameCount++;
	if(m_frameCount % 100 == 0)
	{
		PrintTimers();
	}
	m_lock.unlock();
	return continueFlag;
}

/// Print the results of timings

void Manager::PrintTimers()
{
	LOG4CXX_INFO(m_logger, "Manager: "<<m_frameCount<<" frames processed in "<<m_timerProcessing<<" ms ("<<  (1000.0 * m_frameCount) / m_timerProcessing<<" frames/s)");
	// LOG_INFO("input convertion "                  <<m_timerConvertion<<" ms ("<<(1000.0 * m_frameCount) / m_timerConvertion<<" frames/s)");
	// LOG_INFO("Total time "<< m_timerProcessing + m_timerConvertion<<" ms ("<<     (1000.0 * m_frameCount) /(m_timerProcessing + m_timerConvertion)<<" frames/s)");

	int cpt = 0;
	for(vector<Module*>::const_iterator it = m_modules.begin() ; it != m_modules.end() ; it++)
	{
		// LOG_INFO(cpt<<": ");
		(*it)->PrintStatistics();
		cpt++;
	}
}

/// Pause all modules
void Manager::Pause(bool x_pause)
{
	for(vector<Module*>::iterator it = m_modules.begin() ; it != m_modules.end() ; it++)	
	{
		(*it)->Pause(x_pause);	
	}
}

/// Pause all inputs
void Manager::PauseInputs(bool x_pause)
{
	for(vector<Input*>::iterator it = m_inputs.begin() ; it != m_inputs.end() ; it++)	
	{
		(*it)->Pause(x_pause);	
	}
}


/// Check if end of all input streams

bool Manager::EndOfAllStreams() const
{
	bool endOfStreams = true;
	for(vector<Input*>::const_iterator it1 = m_inputs.begin() ; it1 != m_inputs.end() ; it1++)
	{
		if(!(*it1)->IsEndOfStream())
			endOfStreams = false;
	}
	return endOfStreams;
}

/// Export current configuration to xml

void Manager::Export()
{
	try
	{
		SYSTEM("mkdir -p modules");
		for(vector<Module*>::const_iterator it = m_modules.begin() ; it != m_modules.end() ; it++)
		{
			string file("modules/" + (*it)->GetName() + ".xml");
			ofstream os(file.c_str());
			os<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"<<endl;
			(*it)->Export(os, 0);
			os.close();
		}
	}
	catch(...)
	{
		LOG4CXX_WARN(m_logger, "Exception in Manager::Export");
	}
}

Module* Manager::GetModuleById(int x_id) const
{
	for(vector<Module*>::const_iterator it = m_modules.begin() ; it != m_modules.end() ; it++)
		if((*it)->GetId() == x_id) 
			return *it;
	assert(false);
	return NULL;
}


Module* Manager::GetModuleByName(const string& x_name) const
{
	for(vector<Module*>::const_iterator it = m_modules.begin() ; it != m_modules.end() ; it++)
		if((*it)->GetName() == x_name) 
			return *it;
	throw MkException("Cannot find module " + x_name, LOC);
	return NULL;
}
