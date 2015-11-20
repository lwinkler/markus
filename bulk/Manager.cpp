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
#include "Factories.h"
#include "Controller.h"

#include "util.h"

#include <fstream>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>


using namespace std;

log4cxx::LoggerPtr Manager::m_logger(log4cxx::Logger::getLogger("Manager"));

Manager::Manager(ParameterStructure& xr_params) :
	Processable(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params)),
	mr_parametersFactory(Factories::parametersFactory()),
	mr_moduleFactory(Factories::modulesFactory()),
	mr_parameterFactory(Factories::parametersFactory()),
	m_lastException(MK_EXCEPTION_NORMAL, "normal", "No exception were thrown", "", ""),
	m_interruptionManager(InterruptionManager::GetInst())
{
	LOG_INFO(m_logger, "Create manager");
	m_frameCount = 0;
	m_isConnected = false;
	m_continueFlag = true;
	m_hasRecovered = true;

	m_inputs.clear();
	m_modules.clear();
	m_parameters.clear();

	for(const auto& moduleConfig : m_param.GetConfig().FindAll("module", true))
	{
		// Read parameters
		if(moduleConfig.Find("parameters", true).IsEmpty())
			throw MkException("Impossible to find <parameters> section for module " +  moduleConfig.GetAttribute("name", "(unknown)"), LOC);
		string moduleType = moduleConfig.Find("parameters>param[name=\"class\"]").GetValue();
		ParameterStructure * tmp2 = mr_parametersFactory.Create(moduleType, moduleConfig);
		Module * tmp1 = mr_moduleFactory.Create(moduleType, *tmp2);

		// Add to inputs if an input
		m_modules.push_back(tmp1);
		m_parameters.push_back(tmp2);
		if(tmp1->IsInput())
			m_inputs.push_back(tmp1);
	}
}

void Manager::Start()
{
	for(auto & elem : m_modules)
		elem->Start();
	Processable::Start();
}


void Manager::Stop()
{
	cout << "stop" << m_modules.size() << endl;
	for(auto & elem : m_modules)
		elem->Stop();
	Processable::Stop();
}

Manager::~Manager()
{
	cout << "destroy manager" << endl;

	PrintStatistics();


	for(auto & elem : m_modules)
		delete elem;

	for(auto & elem : m_parameters)
		delete elem;

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
	for(const auto& moduleConfig : m_param.GetConfig().FindAll("module"))
	{
		int moduleId = atoi(moduleConfig.GetAttribute("id").c_str());
		Module& module = RefModuleById(moduleId);

		// If the module is automatically processed: set as ready
		if(module.IsAutoProcessed())
			module.SetAsReady();

		// For each module
		// Read conections of inputs
		for(const auto& inputConfig : moduleConfig.Find("inputs").FindAll("input"))
		{
			// Check if connected to our previous module
			try
			{
				int inputId        = atoi(inputConfig.GetAttribute("id").c_str());
				const string& tmp1 = inputConfig.GetAttribute("moduleid", "");
				const string& tmp2 = inputConfig.GetAttribute("outputid", "");
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
				LOG_ERROR(m_logger, "Cannot connect input "<<inputConfig.GetAttribute("id", "(unknown)")<<" of module "<<module.GetName());
				throw;
			}
		}
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
		for(auto & elem : m_modules)
		{
			if(!(elem)->IsReady())
			{
				ready = false;
				if((elem)->AllInputsAreReady())
				{
					(elem)->SetAsReady();
					if(centralized)
						newOrder.push_back(elem);
					else
					{
						Module& master = RefModuleByName((elem)->GetMasterModule().GetName());
						master.AddDependingModule(*elem);
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
	m_interruptionManager.Configure(m_param.GetConfig());

	// Reset timers
	// m_timerConvertion = 0;
	m_timerProcessing.Reset();

	// Reset all modules (to set the module timer)
	for(auto & elem : m_modules)
	{
		if(x_resetInputs || !(elem)->IsInput())
		{
			// If manager is in autoprocess, modules must not be
			(elem)->AllowAutoProcess(!m_param.autoProcess);
			(elem)->SetRealTime(!m_param.fast);
			(elem)->Reset();
		}
	}
	if(!HasController("manager"))
	{
		AddController(new ControllerManager(*this));
	}

	for(const auto& elem : m_param.GetList())
	{
		// Do not add param if locked or already present
		if(elem->IsLocked() || HasController(elem->GetName()))
			continue;
		Controller* ctr = Factories::parameterControllerFactory().Create(elem->GetType(), *elem);
		if(ctr == nullptr)
			throw MkException("Controller creation failed", LOC);
		else AddController(ctr);
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

	{
		WriteLock lock(m_lock, boost::try_to_lock);

		if(!lock)
		{
			LOG_WARN(m_logger, "Manager too slow !"); // Note: this happens every time
			return true;
		}
		m_timerProcessing.Start();
		bool recover = true;

		for(auto & elem : m_modules)
		{
			try
			{
				// if((*it)->IsAutoProcessed())
				// Note: Since we are in centralized mode, all modules are called directly from the master
				elem->Process();
			}
			catch(FatalException& e)
			{
				LOG_ERROR(m_logger, (elem)->GetName() << ": Exception raised (FatalException), aborting : " << e.what());
				m_continueFlag = recover = m_hasRecovered = false;
			}
			catch(EndOfStreamException& e)
			{
				elem->Stop();
				if(m_hasRecovered)
				{
					// This exception happens after a recovery, keep it!
					m_lastException = e;
					NotifyException(m_lastException);
				}
				recover = m_hasRecovered = false;

				LOG_INFO(m_logger, (elem)->GetName() << ": Exception raised (EndOfStream) : " << e.what());

				// test if all inputs are over
				if(EndOfAllStreams())
				{
					InterruptionManager::GetInst().AddEvent("exception." + e.GetName());
					// TODO: test what happens if the stream of a camera is cut
					LOG_INFO(m_logger, "End of all video streams : Manager::Process");
					m_continueFlag = false;
				}
			}
			catch(MkException& e)
			{
				// InterruptionManager::GetInst().AddEvent("exception." + e.GetName());
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
					m_lastException = MkException((elem)->GetName() + " :" + string(e.what()), LOC);
					NotifyException(m_lastException);
				}
				LOG_ERROR(m_logger, (elem)->GetName() << ": Exception raised (std::exception): " << e.what());
			}
			catch(string& str)
			{
				if(m_hasRecovered)
				{
					// This exception happens after a recovery, keep it!
					m_lastException = MkException((elem)->GetName() + " :" + string(str), LOC);
					NotifyException(m_lastException);
				}
				recover = m_hasRecovered = false;
				LOG_ERROR(m_logger, (elem)->GetName() << ": Exception raised (string): " << str);
			}
			catch(const char* str)
			{
				if(m_hasRecovered)
				{
					// This exception happens after a recovery, keep it!
					m_lastException = MkException((elem)->GetName() + " :" + string(str), LOC);
					NotifyException(m_lastException);
				}
				recover = m_hasRecovered = false;
				LOG_ERROR(m_logger, (elem)->GetName() << ": Exception raised (const char*): " << str);
			}
			catch(...)
			{
				if(m_hasRecovered)
				{
					// This exception happens after a recovery, keep it!
					m_lastException = MkException((elem)->GetName() + ": Unknown", LOC);
					NotifyException(m_lastException);
				}
				recover = m_hasRecovered = false;
				LOG_ERROR(m_logger, (elem)->GetName() << ": Unknown exception raised");
			}
		}

		// If a full processing cycle has been made without exception,
		// we consider that the manager has recovered from exceptions
		if(recover)
			m_hasRecovered = true;
		m_timerProcessing.Stop();
		m_frameCount++;
		if(m_frameCount % 100 == 0 && m_logger->isDebugEnabled())
		{
			PrintStatistics();
		}
	}
	//if(m_frameCount % 20 == 0)
	// usleep(20); // This keeps the manager unlocked to allow the sending of commands // TODO find a cleaner way

	// Send command generated by interruptions
	m_continueFlag = m_continueFlag && (m_param.nbFrames == 0 || m_param.nbFrames != m_frameCount);

	// note: we need send an event to stay in the loop
	if(!m_continueFlag && m_param.autoProcess)
		m_interruptionManager.AddEvent("event.stopped");
  
	vector<Command> commands = m_interruptionManager.ReturnCommandsToSend();
	for(const auto& command : commands)
	{
		SendCommand(command.name, command.value);
		m_continueFlag = true;
	}

	return m_continueFlag;
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
		throw MkException("Command must be in format \"module.controller.Command\": " + x_command, LOC);

	// Note: We cast module/manager twice since we need functions from both parents
	Controllable& contr  (elems.at(0) == "manager" ? dynamic_cast<Controllable&>(*this) : RefModuleByName(elems.at(0)));
	Processable&  process(elems.at(0) == "manager" ? dynamic_cast<Processable&>(*this) : RefModuleByName(elems.at(0)));

	WriteLock lock(process.RefLock());
	contr.FindController(elems.at(1)).CallAction(elems.at(2), &x_value);

	LOG_INFO(m_logger, "Command "<<x_command<<" returned value "<<x_value);
}

/**
* @brief Print the results of timings
*/
void Manager::PrintStatistics()
{
	double fps = m_frameCount / m_timerProcessing.GetSecDouble();
	LOG_INFO(m_logger, "Manager: "<<m_frameCount<<" frames processed in "<<m_timerProcessing.GetSecDouble()*1000<<" ms ("<< fps <<" frames/s)");
	// LOG_INFO("input convertion "                  <<m_timerConvertion<<" ms ("<<(1000.0 * m_frameCount) / m_timerConvertion<<" frames/s)");
	// LOG_INFO("Total time "<< m_timerProcessing + m_timerConvertion<<" ms ("<<     (1000.0 * m_frameCount) /(m_timerProcessing + m_timerConvertion)<<" frames/s)");

	// Create an XML file to summarize CPU usage
	//     if output dir is empty, write to /tmp
	string benchFileName = ((IsContextSet() && !GetContext().IsOutputDirEmpty()) ? GetContext().GetOutputDir() : "/tmp") + "/benchmark.xml";
	ConfigFile benchSummary(benchFileName, true);
	ConfigReader conf = benchSummary.FindRef("benchmark", true);


	// Write perf to output XML
	ConfigReader perfModule = conf.FindRef("manager", true);
	perfModule.FindRef("nb_frames", true).SetValue(m_frameCount);
	perfModule.FindRef("timer[name=\"processing\"]", true).SetValue(m_timerProcessing.GetSecDouble()*1000);
	perfModule.FindRef("fps", true).SetValue(fps);

	// Call for each module
	for(const auto& module : m_modules)
	{
		// LOG_INFO(cpt<<": ");
		module->PrintStatistics(conf);
	}
	benchSummary.SaveToFile(benchFileName);
}

/**
* @brief Check if end of all input streams
*
* @return True if all streams have ended
*/
bool Manager::EndOfAllStreams() const
{
	bool endOfStreams = true;
	for(const auto & elem : m_inputs)
	{
		if(!dynamic_cast<Input*>(elem)->IsEndOfStream())
			endOfStreams = false;
	}
	return endOfStreams;
}

/**
* @brief Export current configuration to xml: this is used to create the XML and JSON files to describe each module
*/
void Manager::CreateEditorFiles(const string& x_fileName)
{
	try
	{
		map<string,vector<string>> categories;
		Json::Value modules_json;

		SYSTEM("mkdir -p modules");
		vector<string> moduleTypes;
		mr_moduleFactory.List(moduleTypes);
		int id = 0;
		for(const auto& moduleType : moduleTypes)
		{
			createEmptyConfigFile("/tmp/config_empty.xml");
			ConfigFile config("/tmp/config_empty.xml");
			ConfigReader moduleConfig = config.FindRef("application>module[name=\"" + moduleType + "\"]", true);
			moduleConfig.SetAttribute("id", id);
			moduleConfig.FindRef("parameters>param[name=\"class\"]", true).SetValue(moduleType);

			ParameterStructure* parameters = mr_parameterFactory.Create(moduleType, moduleConfig);
			Module* module = mr_moduleFactory.Create(moduleType, *parameters);

			modules_json.append(moduleType);

			// Append to the category
			categories[module->GetCategory()].push_back(moduleType);
			categories["all"].push_back(moduleType);

			// Create the specific XML
			string file("editor/modules/" + moduleType + ".xml");
			ofstream os(file.c_str());
			os<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"<<endl;
			module->Export(os, 0);
			delete module;
			delete parameters;
			os.close();
			id++;
		}

		// Generate the js file containing the categories

		Json::Value categories_json;
		for(const auto& categ : categories)
		{
			for(const auto& mod : categ.second)
			{
				categories_json[categ.first].append(mod);
			}
		}
		ofstream of(x_fileName);
		of << "// This file contain the list of modules and modules categories. Generated automatically and used by the editor" << endl;
		of << "var availableCategories = " << categories_json << ";" << endl << endl;
		of << "var availableModules = "    << modules_json    << ";" << endl << endl;
		of.close();
	}
	catch(MkException& e)
	{
		LOG_ERROR(m_logger, "Exception in Manager::Export: "<<e.what());
		throw;
	}
}

Module& Manager::RefModuleById(int x_id) const
{
	Module* module = nullptr;
	int found = 0;
	for(const auto & elem : m_modules)
		if((elem)->GetId() == x_id)
		{
			module = elem;
			found++;
		}
	if(found == 1)
		return *module;
	if(found == 0)
		throw MkException("Module not found", LOC);
	else
		throw MkException("more than one module with id ", LOC);
}


Module& Manager::RefModuleByName(const string& x_name) const
{
	for(const auto & elem : m_modules)
		if((elem)->GetName() == x_name)
			return *elem;
	throw MkException("Cannot find module " + x_name, LOC);
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
	evt.Notify(GetContext(), true);
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
	for(auto & elem : m_parameters)
		elem->UpdateConfig();
	m_param.UpdateConfig();
}

/**
* @brief Write the states of all modules to disk
*
* @param x_directory Output directory
*/
void Manager::WriteStateToDirectory(const string& x_directory) const
{
	string directory = GetContext().GetOutputDir() + "/" + x_directory;
	SYSTEM("mkdir -p " + directory);
	for(const auto & elem : m_modules)
	{
		string fileName = directory + "/" + (elem)->GetName() + ".json";
		ofstream of;
		of.open(fileName.c_str());
		(elem)->Serialize(of, directory);
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
	InterruptionManager::GetInst().AddEvent("exception." + x_exception.GetName());// TODO keep this here ?
	stringstream ss;
	x_exception.Serialize(ss, "");
	Event ev;
	ev.AddExternalInfo("exception", ss);
	ev.Raise("exception");
	ev.Notify(GetContext(), true);
}
