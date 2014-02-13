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

using namespace std;

//#include "AllModules.h"
#include "Controller.h"

	
#if defined( WIN32 ) && defined( TUNE )
	#include <crtdbg.h>
	_CrtMemState startMemState;
	_CrtMemState endMemState;
#endif


using namespace std;

log4cxx::LoggerPtr Manager::m_logger(log4cxx::Logger::getLogger("manager"));
string Manager::m_configFile;
string Manager::m_outputDir;
FactoryModules Manager::m_factory;


Manager::Manager(const ConfigReader& x_configReader, bool x_centralized) : 
	Configurable(x_configReader),
	m_param(m_configReader),
	m_centralized(x_centralized)
{
	LOG_INFO(m_logger, "Create object Manager");
	m_frameCount = 0;
	m_isConnected = false;
	
	m_inputs.clear();
	m_modules.clear();
	
	// Read the configuration of each module
	ConfigReader moduleConfig = m_configReader.GetSubConfig("module");
	
	while(! moduleConfig.IsEmpty())
	{
		// Read parameters
		if( moduleConfig.GetSubConfig("parameters").IsEmpty()) 
			throw MkException("Impossible to find <parameters> section for module " +  moduleConfig.GetAttribute("name"), LOC);
		string moduleType = moduleConfig.GetSubConfig("parameters").GetSubConfig("param", "class").GetValue();
		Module * tmp1 = m_factory.CreateModule(moduleType, moduleConfig);		
		
		// Add to inputs if an input
		m_modules.push_back(tmp1);
		if(tmp1->IsInput())
			m_inputs.push_back(dynamic_cast<Input* >(tmp1));
		moduleConfig = moduleConfig.NextSubConfig("module");
	}
}

Manager::~Manager()
{
	PrintTimers();
	for(vector<Module*>::iterator it = m_modules.begin() ; it != m_modules.end() ; it++)
		delete *it;

	// Write final infos
	if(m_outputDir.size() != 0)
		LOG_INFO(m_logger, "Results written to directory "<<m_outputDir);

	/// Do the final operations on the static class
	if(m_outputDir.size() != 0 && getenv("LOG_DIR") == NULL)
	{
		SYSTEM("cp markus.log " + m_outputDir + "/markus.copy.log");
		// SYSTEM("cp markus " + m_outputDir);
	}
}

/// Connect the different modules
void Manager::Connect()
{
	if(m_isConnected)
		throw MkException("Manager can only connect modules once", LOC);
	
	// Connect input and output streams (re-read the config once since we need all modules to be connected)
	ConfigReader moduleConfig = m_configReader.GetSubConfig("module");
	
	while(! moduleConfig.IsEmpty())
	{
		int moduleId = atoi(moduleConfig.GetAttribute("id").c_str());
		Module& module = RefModuleById(moduleId);
		if(module.IsAutoProcessed())
			module.SetAsReady();

		// Read conections of inputs
		ConfigReader conf = moduleConfig.GetSubConfig("inputs");
		if(!conf.IsEmpty())
		{
			ConfigReader inputConfig = conf.GetSubConfig("input");
			while(! inputConfig.IsEmpty())
			{
				int inputId        = atoi(inputConfig.GetAttribute("id").c_str());
				const string& tmp1 = inputConfig.GetAttribute("moduleid");
				const string& tmp2 = inputConfig.GetAttribute("outputid");
				if(tmp1 != "" && tmp2 != "")
				{
					int outputModuleId    = atoi(tmp1.c_str());
					int outputId          = atoi(tmp2.c_str());
					Stream& inputStream  = module.RefInputStreamById(inputId);
					Stream& outputStream = RefModuleById(outputModuleId).RefOutputStreamById(outputId);

					// Connect input and output streams
					inputStream.Connect(&outputStream);
				}
				inputConfig = inputConfig.NextSubConfig("input");
			}
		}
		moduleConfig = moduleConfig.NextSubConfig("module");
	}

	// Set the master module for each module
	//    the master module is the module responsible to call the Process method
	bool changed = true;
	bool ready = true;
	while(changed)
	{
		changed = false;
		ready = true;
		for(vector<Module*>::iterator it = m_modules.begin() ; it != m_modules.end() ; it++)
		{
			if(!(*it)->IsReady())
			{
				ready = false;
				if((*it)->AllInputsAreReady())
				{
					Module& master = RefModuleByName((*it)->GetMasterModule().GetName());
					(*it)->SetAsReady();
					master.AddDependingModule(**it);
					// cout<<"Set module "<<depending->GetName()<<" as ready"<<endl;
					changed = true;
				}
			}
		}
	}
	if(! ready)
		throw MkException("Not all modules can be assigned to a master. There is probably a problem with the connections between modules.");
	
	m_isConnected = true;	
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
	if(!m_isConnected)
		throw MkException("Modules must be connected before processing", LOC);

	// If methods are not called in a centralized way, we will rely on timers on each module
	assert(m_centralized);

	if(!m_lock.tryLockForWrite())
	{
		LOG_WARN(m_logger, "Manager too slow !");
		return true;
	}
	Timer ti;
	
	bool continueFlag = true;
	for(vector<Module*>::iterator it = m_modules.begin() ; it != m_modules.end() ; it++)
	{
		try
		{
			if((*it)->IsAutoProcessed())
				(*it)->Process();
		}
		/*catch(cv::Exception& e)
		{
			LOG_ERROR(m_logger, (*it)->GetName() << ": Exception raised (std::exception) : " << e.what());
		}*/
		catch(EndOfStreamException& e)
		{
			LOG_INFO(m_logger, (*it)->GetName() << ": Exception raised (EndOfStream) : " << e.what());

			// test if all inputs are over
			if(EndOfAllStreams())
			{
				LOG_INFO(m_logger, "End of all video streams : Manager::Process");
				continueFlag = false;
			}
		}
		catch(std::exception& e)
		{
			LOG_ERROR(m_logger, (*it)->GetName() << ": Exception raised (std::exception): " << e.what());
		}
		catch(std::string str)
		{
			LOG_ERROR(m_logger, (*it)->GetName() << ": Exception raised (string): " << str);
		}
		catch(const char* str)
		{
			LOG_ERROR(m_logger, (*it)->GetName() << ": Exception raised (const char*): " << str);
		}
		catch(...)
		{
			LOG_ERROR(m_logger, (*it)->GetName() << ": Unknown exception raised");
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

/// Send a command
void Manager::SendCommand(const std::string& x_command, std::string x_value)
{
	vector<string> elems;
	split(x_command, '.', elems);
	if(elems.size() != 3)
		throw MkException("Command must be in format \"module.controller.command\"", LOC);
	
	if(elems.at(0) == "manager")
		;	// manager.GetControlList();
	else
	{
		Module& module(RefModuleByName(elems.at(0)));
		module.LockForWrite();
		module.FindController(elems.at(1))->CallAction(elems.at(2), &x_value);
		module.Unlock();
	}
	LOG_INFO(m_logger, "Command "<<x_command<<" returned value "<<x_value);
	
}


/// Print the results of timings

void Manager::PrintTimers()
{
	LOG_INFO(m_logger, "Manager: "<<m_frameCount<<" frames processed in "<<m_timerProcessing<<" ms ("<<  (1000.0 * m_frameCount) / m_timerProcessing<<" frames/s)");
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

/// Export current configuration to xml: this is used to create the XML files to describe each module

void Manager::Export()
{
	try
	{
		SYSTEM("mkdir -p modules");
		vector<string> moduleTypes;
		m_factory.ListModules(moduleTypes);
		for(vector<string>::const_iterator it = moduleTypes.begin() ; it != moduleTypes.end() ; it++)
		{
			string file("modules/" + *it + ".xml");
			ofstream os(file.c_str());
			os<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"<<endl;
			ConfigReader config("config_empty.xml");
			ConfigReader moduleConfig = config.RefSubConfig("application", "").RefSubConfig("module", *it, true);
			moduleConfig.RefSubConfig("parameters", "", true).RefSubConfig("param", "class", true).SetValue(*it);

			Module* module = m_factory.CreateModule(*it, moduleConfig);
			module->Export(os, 0);
			delete module;
			os.close();
		}
	}
	catch(...)
	{
		LOG_WARN(m_logger, "Exception in Manager::Export");
	}
}

Module& Manager::RefModuleById(int x_id) const
{
	Module* module = NULL;
	int found = 0;
	for(vector<Module*>::const_iterator it = m_modules.begin() ; it != m_modules.end() ; it++)
		if((*it)->GetId() == x_id) 
		{
			module = *it;
			found++;
		}
	if(found == 1)
		return *module;
	if(found == 0)
		throw MkException("Module not found " + x_id, LOC);
	else
		throw MkException("more than one module with id " + x_id, LOC);
	//return NULL;
}


Module& Manager::RefModuleByName(const string& x_name) const
{
	for(vector<Module*>::const_iterator it = m_modules.begin() ; it != m_modules.end() ; it++)
		if((*it)->GetName() == x_name) 
			return **it;
	throw MkException("Cannot find module " + x_name, LOC);
	// return NULL;
}

/// Returns a directory that will contain all outputs
const string& Manager::OutputDir(const string& x_outputDir)
{
	if(m_outputDir.size() == 0)
	{
		try
		{
			if(x_outputDir == "")
				m_outputDir = "out_" + timeStamp();
			else
				m_outputDir = x_outputDir;

			if(m_configFile == "")
				throw MkException("Config file not set in Manager. You need to specify it with Manager::SetConfigFile(...).");
			short trial = 0;
			string tmp = m_outputDir;

			// Try to create the output dir, if it fails, try changing the name
			while(trial < 250)
			{
				try
				{
					SYSTEM("mkdir \"" + m_outputDir + "\"");
					trial = 250;
				}
				catch(...)
				{
					stringstream ss;
					trial++;
					ss<<tmp<<"_"<<trial;
					m_outputDir = ss.str();
					if(trial == 250)
						throw MkException("Cannot create output directory", LOC);
				}
			}
			LOG_INFO(m_logger, "Creating directory "<<m_outputDir<<" and symbolic link out_latest");
			// note: use ln with atgs sfn to override existing link
			SYSTEM("ln -sfn \"" + m_outputDir + "\" out_latest");
			SYSTEM("tools/version.sh > " + m_outputDir + "/version.txt");
			SYSTEM("cp " + m_configFile + " " + m_outputDir);
		}
		catch(exception& e)
		{
			LOG_WARN(m_logger, "Exception in Manager::OutputDir" << e.what());
		}
	}
	return m_outputDir;
}


