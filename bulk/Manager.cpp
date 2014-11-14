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
#include "Event.h"
#include "MkException.h"
#include "ControllerManager.h"
#include "Controller.h"

#include "util.h"

#include <fstream>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>


using namespace std;

log4cxx::LoggerPtr Manager::m_logger(log4cxx::Logger::getLogger("Manager"));

const FactoryModules Manager::m_factory;


Manager::Manager(const ConfigReader& x_configReader) : 
	Processable(x_configReader),
	m_param(m_configReader),
	m_lastException(MK_EXCEPTION_NORMAL, "No exception were thrown", "", "")
{
	LOG_INFO(m_logger, "Create object Manager");
	m_frameCount = 0;
	m_isConnected = false;
	m_continueFlag = true;
	m_hasRecovered = true;
	m_timerProcessing = 0;
	
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
			m_inputs.push_back(tmp1);
		moduleConfig = moduleConfig.NextSubConfig("module");
	}
}

Manager::~Manager()
{
	PrintStatistics();


	for(vector<Module*>::iterator it = m_modules.begin() ; it != m_modules.end() ; ++it)
		delete *it;

	/// Do the final operations on the static class
	const string& outputDir = m_context.GetOutputDir();
	if(!outputDir.empty())
	{
		LOG_INFO(m_logger, "Results written to directory "<<outputDir);
		if(getenv("LOG_DIR") == NULL)
		{
			try
			{
				SYSTEM("cp markus.log " + outputDir + "/markus.copy.log");
			}
			catch(...)
			{
				LOG_WARN(m_logger, "Error at the copy of markus.log");	
			}
		}

		// Copy the directory for archiving if needed
		try
		{
			if(m_param.autoClean)
			{
				if(m_param.archiveDir != "")
				{
					LOG_INFO(m_logger, "Working directory moved to " + m_param.archiveDir);
					SYSTEM("mkdir -p " + m_param.archiveDir);
					SYSTEM("mv " + outputDir + " " + m_param.archiveDir + "/");
				}
				else 
				{
					LOG_INFO(m_logger, "Working directory deleted");
					SYSTEM("rm -rf " + outputDir);
				}
			}
			else
			{
				if(m_param.archiveDir != "")
				{
					LOG_INFO(m_logger, "Working directory copied to " + m_param.archiveDir);
					SYSTEM("mkdir -p " + m_param.archiveDir);
					SYSTEM("cp -r " + outputDir + " " + m_param.archiveDir);
				}
			}
		}
		catch(MkException &e)
		{
			LOG_ERROR(m_logger, "Exception thrown while archiving: " << e.what());
		}
	}
}

/**
* @brief Connect the different modules
*/
void Manager::Connect()
{
	bool centralized = m_param.autoProcess;

	Processable::Reset();
	if(m_isConnected)
		throw MkException("Manager can only connect modules once", LOC);
	
	// Connect input and output streams (re-read the config once since we need all modules to be connected)
	ConfigReader moduleConfig = m_configReader.GetSubConfig("module");
	
	while(! moduleConfig.IsEmpty())
	{
		int moduleId = atoi(moduleConfig.GetAttribute("id").c_str());
		Module& module = RefModuleById(moduleId);

		// If the module is automatically processed: set as ready
		if(module.IsAutoProcessed())
			module.SetAsReady();

		// For each module
		// Read conections of inputs
		ConfigReader conf = moduleConfig.GetSubConfig("inputs");
		if(!conf.IsEmpty())
		{
			ConfigReader inputConfig = conf.GetSubConfig("input");
			while(! inputConfig.IsEmpty())
			{
				// Check if connected to our previous module
				try
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
				}
				catch(MkException& e)
				{
					LOG_ERROR(m_logger, "Cannot connect input "<<inputConfig.GetAttribute("id")<<" of module "<<module.GetName());
					throw;
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
	vector<Module*> newOrder = m_inputs;

	while(changed)
	{
		changed = false;
		ready = true;
		for(vector<Module*>::iterator it = m_modules.begin() ; it != m_modules.end() ; ++it)
		{
			if(!(*it)->IsReady())
			{
				ready = false;
				if((*it)->AllInputsAreReady())
				{
					(*it)->SetAsReady();
					if(centralized)
						newOrder.push_back(*it);
					else
					{
						Module& master = RefModuleByName((*it)->GetMasterModule().GetName());
						master.AddDependingModule(**it);
					}
					// cout<<"Set module "<<depending->GetName()<<" as ready"<<endl;
					changed = true;
				}
			}
		}
	}
	// If centralized reorder the module list
	if(centralized)
		m_modules = newOrder;
	
	if(! ready)
		throw MkException("Not all modules can be assigned to a master. There is probably a problem with the connections between modules.", LOC);
	
	m_isConnected = true;	
}

/**
* @brief Reset the manager: must be called externally after initialization
*
* @param x_resetInputs Also reset input modules (true by default)
*/
void Manager::Reset(bool x_resetInputs)
{
	// Reset timers
	// m_timerConvertion = 0;
	m_timerProcessing = 0;

	// Reset all modules (to set the module timer)
	for(vector<Module*>::iterator it = m_modules.begin() ; it != m_modules.end() ; ++it)
	{
		if(x_resetInputs || !(*it)->IsInput())
		{
			// If manager is in autoprocess, modules must not be
			(*it)->AllowAutoProcess(!m_param.autoProcess);
			(*it)->SetRealTime(!m_param.fast);
			(*it)->Reset();
		}
	}
	if(FindController("manager") == NULL)
	{
		AddController(new ControllerManager(*this));
	}
	m_hasRecovered = true;
	m_continueFlag = true;
	m_frameCount = 0;
}

/**
* @brief All modules process one frame
*
* @return False if the processing must be stopped
*/
bool Manager::Process()
{
	if(!m_isConnected)
		throw MkException("Modules must be connected before processing", LOC);

	if(!m_lock.tryLockForWrite())
	{
		// LOG_WARN(m_logger, "Manager too slow !"); // Note: this happens every time
		return true;
	}
	if(m_pause)
	{
		m_lock.unlock();
		return true;
	}
	Timer ti;
	bool recover = true;
	
	for(vector<Module*>::iterator it = m_modules.begin() ; it != m_modules.end() ; ++it)
	{
		try
		{
			// if((*it)->IsAutoProcessed())
			// Note: Since we are in centralized mode, all modules are called directly from the master
			(*it)->Process();
		}
		catch(EndOfStreamException& e)
		{
			if(m_hasRecovered)
			{
				// This exception happens after a recovery, keep it!
				m_lastException = e;
				NotifyException(m_lastException);
			}
			recover = m_hasRecovered = false;
				
			LOG_INFO(m_logger, (*it)->GetName() << ": Exception raised (EndOfStream) : " << e.what());

			// test if all inputs are over
			if(EndOfAllStreams())
			{
				// TODO: test what happens if the stream of a camera is cut
				LOG_INFO(m_logger, "End of all video streams : Manager::Process");
				m_continueFlag = false;
			}
		}
		catch(MkException& e)
		{
			if(m_hasRecovered)
			{
				// This exception happens after a recovery, keep it!
				m_lastException = e;
				NotifyException(m_lastException);
			}
			recover = m_hasRecovered = false;
			LOG_ERROR(m_logger, "(Markus exception " << e.GetCode() << "): " << e.what());
		}
		catch(exception& e)
		{
			if(m_hasRecovered)
			{
				// This exception happens after a recovery, keep it!
				m_lastException = MkException(MK_EXCEPTION_UNKNOWN, (*it)->GetName() + " :" + string(e.what()), LOC);
				NotifyException(m_lastException);
			}
			LOG_ERROR(m_logger, (*it)->GetName() << ": Exception raised (std::exception): " << e.what());
		}
		catch(string& str)
		{
			if(m_hasRecovered)
			{
				// This exception happens after a recovery, keep it!
				m_lastException = MkException(MK_EXCEPTION_UNKNOWN, (*it)->GetName() + " :" + string(str), LOC);
				NotifyException(m_lastException);
			}
			recover = m_hasRecovered = false;
			LOG_ERROR(m_logger, (*it)->GetName() << ": Exception raised (string): " << str);
		}
		catch(const char* str)
		{
			if(m_hasRecovered)
			{
				// This exception happens after a recovery, keep it!
				m_lastException = MkException(MK_EXCEPTION_UNKNOWN, (*it)->GetName() + " :" + string(str), LOC);
				NotifyException(m_lastException);
			}
			recover = m_hasRecovered = false;
			LOG_ERROR(m_logger, (*it)->GetName() << ": Exception raised (const char*): " << str);
		}
		catch(...)
		{
			if(m_hasRecovered)
			{
				// This exception happens after a recovery, keep it!
				m_lastException = MkException(MK_EXCEPTION_UNKNOWN, (*it)->GetName() + ": Unknown", LOC);
				NotifyException(m_lastException);
			}
			recover = m_hasRecovered = false;
			LOG_ERROR(m_logger, (*it)->GetName() << ": Unknown exception raised");
		}
	}

	// If a full processing cycle has been made without exception, 
	// we consider that the manager has recovered from exceptions
	if(recover)
		m_hasRecovered = true;
	m_timerProcessing += ti.GetMSecLong();
	m_frameCount++;
	if(m_frameCount % 100 == 0 && m_logger->isDebugEnabled())
	{
		PrintStatistics();
	}
	m_lock.unlock();
	//if(m_frameCount % 20 == 0)
		usleep(20); // This keeps the manager unlocked to allow the sending of commands // TODO find a cleaner way

	return m_continueFlag && (m_param.nbFrames == 0 || m_param.nbFrames != m_frameCount);
}

/**
* @brief Send a command
*
* @param x_command Command in format "module.controller.Command"
* @param x_value   Value used as input/output
*/
void Manager::SendCommand(const string& x_command, string x_value)
{
	vector<string> elems;
	split(x_command, '.', elems);
	if(elems.size() != 3)
		throw MkException("Command must be in format \"module.controller.Command\"", LOC);

	if(elems.at(0) == "manager")
	{
		try
		{
			m_lock.lockForWrite();
			FindController(elems.at(1))->CallAction(elems.at(2), &x_value);
			m_lock.unlock();
		}
		catch(...)
		{
			m_lock.unlock();
			throw;
		}
	}
	else
	{
		Module& module(RefModuleByName(elems.at(0)));
		try
		{
			module.LockForWrite();
			module.FindController(elems.at(1))->CallAction(elems.at(2), &x_value);
			module.Unlock();
		}
		catch(...)
		{
			module.Unlock();
			throw;
		}
	}
	LOG_INFO(m_logger, "Command "<<x_command<<" returned value "<<x_value);
}

/**
* @brief Print the results of timings
*/
void Manager::PrintStatistics()
{
	LOG_INFO(m_logger, "Manager: "<<m_frameCount<<" frames processed in "<<m_timerProcessing<<" ms ("<<  (1000.0 * m_frameCount) / m_timerProcessing<<" frames/s)");
	// LOG_INFO("input convertion "                  <<m_timerConvertion<<" ms ("<<(1000.0 * m_frameCount) / m_timerConvertion<<" frames/s)");
	// LOG_INFO("Total time "<< m_timerProcessing + m_timerConvertion<<" ms ("<<     (1000.0 * m_frameCount) /(m_timerProcessing + m_timerConvertion)<<" frames/s)");

	int cpt = 0;
	for(vector<Module*>::const_iterator it = m_modules.begin() ; it != m_modules.end() ; ++it)
	{
		// LOG_INFO(cpt<<": ");
		(*it)->PrintStatistics();
		cpt++;
	}
}

/**
* @brief Pause all modules
*
* @param x_pause Set or unset pause
*/
void Manager::Pause(bool x_pause)
{
	Processable::Pause(x_pause);
	for(vector<Module*>::iterator it = m_modules.begin() ; it != m_modules.end() ; ++it)	
		(*it)->Pause(x_pause);	
}

/**
* @brief Pause all inputs
*
* @param x_pause Set or unset pause
*/
void Manager::PauseInputs(bool x_pause)
{
	for(vector<Module*>::iterator it = m_inputs.begin() ; it != m_inputs.end() ; ++it)	
	{
		(*it)->Pause(x_pause);	
	}
}


/**
* @brief Check if end of all input streams
*
* @return True if all streams have ended
*/
bool Manager::EndOfAllStreams() const
{
	bool endOfStreams = true;
	for(vector<Module*>::const_iterator it1 = m_inputs.begin() ; it1 != m_inputs.end() ; ++it1)
	{
		if(!dynamic_cast<Input*>(*it1)->IsEndOfStream())
			endOfStreams = false;
	}
	return endOfStreams;
}

/**
* @brief Export current configuration to xml: this is used to create the XML files to describe each module
*/
void Manager::Export()
{
	try
	{
		SYSTEM("mkdir -p modules");
		vector<string> moduleTypes;
		m_factory.ListModules(moduleTypes);
		for(vector<string>::const_iterator it = moduleTypes.begin() ; it != moduleTypes.end() ; ++it)
		{
			string file("modules/" + *it + ".xml");
			createEmptyConfigFile("/tmp/config_empty.xml");
			ConfigReader config("/tmp/config_empty.xml");
			ConfigReader moduleConfig = config.RefSubConfig("application", "", true).RefSubConfig("module", *it, true);
			moduleConfig.RefSubConfig("parameters", "", true).RefSubConfig("param", "class", true).SetValue(*it);

			Module* module = m_factory.CreateModule(*it, moduleConfig);

			ofstream os(file.c_str());
			os<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"<<endl;
			module->Export(os, 0);
			delete module;
			os.close();
		}
	}
	catch(MkException& e)
	{
		LOG_WARN(m_logger, "Exception in Manager::Export: "<<e.what());
	}
}

Module& Manager::RefModuleById(int x_id) const
{
	Module* module = NULL;
	int found = 0;
	for(vector<Module*>::const_iterator it = m_modules.begin() ; it != m_modules.end() ; ++it)
		if((*it)->GetId() == x_id) 
		{
			module = *it;
			found++;
		}
	if(found == 1)
		return *module;
	if(found == 0)
		throw MkException("Module not found", LOC);
	else
		throw MkException("more than one module with id ", LOC);
	//return NULL;
}


Module& Manager::RefModuleByName(const string& x_name) const
{
	for(vector<Module*>::const_iterator it = m_modules.begin() ; it != m_modules.end() ; ++it)
		if((*it)->GetName() == x_name) 
			return **it;
	throw MkException("Cannot find module " + x_name, LOC);
	// return NULL;
}


/**
* @brief Log the status of the application (last exception)
*/
void Manager::Status() const
{
	stringstream ss;
	m_lastException.Serialize(ss, "");
	Json::Value root;
	ss >> root;
	root["recovered"] = m_hasRecovered;
	Event evt;
	evt.Raise("status");
	ss.clear();
	ss << root;
	evt.AddExternalInfo("exception", ss);
	evt.Notify(m_context, true);
}

/**
* @brief Return a directory that will contain all outputs files and logs. The dir is created at the first call of this method.
*
* @param x_outputDir  Name and path to the dir, if empty, the name is generated automatically
* @param x_configFile Optional: name of the config file to copy into dir
*
* @return Name of the output dir
*/
string Manager::CreateOutputDir(const string& x_outputDir, const string& x_configFile)
{
	string outputDir;
	try
	{
		if(x_outputDir == "")
		{
			outputDir = "out_" + timeStamp();
			int16_t trial = 0; // Must NOT be a char to avoid concatenation problems!
			string tmp = outputDir;

			// Try to create the output dir, if it fails, try changing the name
			while(trial < 250)
			{
				try
				{
					SYSTEM("mkdir \"" + outputDir + "\"");
					trial = 250;
				}
				catch(...)
				{
					stringstream ss;
					trial++;
					ss<<tmp<<"_"<<trial;
					outputDir = ss.str();
					if(trial == 250)
						throw MkException("Cannot create output directory", LOC);
				}
			}
		}
		else
		{
			// If the name is specified do not check if the direcory exists
			outputDir = x_outputDir;
			SYSTEM("mkdir -p \"" + outputDir + "\"");
		}

		// Copy config to output dir
		if(x_configFile == "")
		{
			// note: do not log as logger may not be initialized
			// LOG_INFO(m_logger, "Creating directory "<<outputDir<<" and symbolic link out_latest");
			// note: use ln with args sfn to override existing link
			SYSTEM("ln -sfn \"" + outputDir + "\" out_latest");
		}
		else
		{
			// note: do not log as logger may not be initialized
			// Copy config file to output directory
			// LOG_INFO(m_logger, "Creating directory "<<outputDir);
			SYSTEM("cp " + x_configFile + " " + outputDir);
		}
	}
	catch(exception& e)
	{
		LOG_WARN(m_logger, "Exception in Manager::OutputDir: " << e.what());
	}
	return outputDir;
}

/**
* @brief Save the configuration of manager and modules to file
*/
void Manager::UpdateConfig()
{
	// Set all config ready to be saved
	for(vector<Module*>::iterator it = m_modules.begin() ; it != m_modules.end() ; ++it)
		(*it)->UpdateConfig();
	Configurable::UpdateConfig();
}

/**
* @brief Write the states of all modules to disk
*
* @param x_directory Output directory
*/
void Manager::WriteStateToDirectory(const string& x_directory) const
{
	string directory = m_context.GetOutputDir() + "/" + x_directory;
	SYSTEM("mkdir -p " + directory);
	for(vector<Module*>::const_iterator it = m_modules.begin() ; it != m_modules.end() ; ++it)
	{
		string fileName = directory + "/" + (*it)->GetName() + ".json";
		ofstream of;
		of.open(fileName.c_str());
		(*it)->Serialize(of, directory);
		of.close();
	}
	LOG_INFO(m_logger, "Written state of the manager and all modules to " << directory);
}


/**
* @brief Notify the parent process of an exception
*
* @param x_exception Exception to be notified
*/
void Manager::NotifyException(const MkException& x_exception)
{
	stringstream ss;
	x_exception.Serialize(ss, "");
	Event ev;
	ev.AddExternalInfo("exception", ss);
	ev.Raise("exception");
	ev.Notify(m_context, true);
}
