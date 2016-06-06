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
#include <future>
#include <thread>
#include <chrono>
#include <boost/lexical_cast.hpp>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>

#define PROCESS_TIMEOUT 60 // 1 min timeout

using namespace std;

log4cxx::LoggerPtr Manager::m_logger(log4cxx::Logger::getLogger("Manager"));


Manager::Manager(ParameterStructure& xr_params) :
	Processable(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params)),
	mr_parametersFactory(Factories::parametersFactory()),
	mr_moduleFactory(Factories::modulesFactory())
{
	LOG_INFO(m_logger, "Create manager");
	Build();
	m_interruptionManager.Configure(m_param.config);
}

Manager::~Manager()
{
	try
	{
		Destroy();
	}
	catch(exception &e)
	{
		LOG_ERROR(m_logger, "Fatal exception while destroying the manager: " << e.what());
	}
}

/**
* @brief Create all modules
*/
void Manager::Build()
{
	for(auto& moduleConfig : m_param.config.FindAll("module"))
	{
		// Read parameters
		if(moduleConfig.Find("parameters").IsEmpty())
			throw MkException("Impossible to find <parameters> section for module " +  moduleConfig.GetAttribute("name", "(unknown)"), LOC);
		string moduleType = moduleConfig.Find("parameters>param[name=\"class\"]").GetValue();
		ParameterStructure * tmp2 = mr_parametersFactory.Create(moduleType, moduleConfig);
		tmp2->Read(moduleConfig);
		tmp2->CheckRange(moduleConfig);

		if(!m_param.aspectRatio.empty())
		{
			// Force the aspect ratio to given value
			Module::Parameters& mp = dynamic_cast<Module::Parameters&>(*tmp2);
			double ar = convertAspectRatio(m_param.aspectRatio);
			if(ar >= 1)
			{
				mp.height = mp.width / ar;
			}
			else
			{
				mp.height = mp.width * ar;
				swap(mp.width, mp.height);
			}
			// cout << mp.width << "x" << mp.height << endl;
			LOG_INFO(m_logger, "Change aspect ratio of module of type " + moduleType + " to " + to_string(mp.width) + "x" + to_string(mp.height) + " to match " + m_param.aspectRatio);
		}
		Module * tmp1 = mr_moduleFactory.Create(moduleType, *tmp2);

		LOG_DEBUG(m_logger, "Add module " << tmp1->GetName() << " to list input=" << (tmp1->IsInput() ? "yes" : "no"));
		int id = boost::lexical_cast<int>(moduleConfig.GetAttribute("id"));
		auto ret = m_modules.insert(std::pair<int,Module*>(id, tmp1));
		if(ret.second == false)
		{
			throw MkException("Module with id " + moduleConfig.GetAttribute("id") + " is listed more than one time in config", LOC);
		}
		m_parameters.insert(std::pair<int, ParameterStructure*>(id, tmp2));

		// Add to inputs if an input
		if(tmp1->IsInput())
		{
			Input* tmpi = dynamic_cast<Input*>(tmp1);
			assert(tmpi != nullptr);
			m_inputs.push_back(tmpi);
		}
		if(tmp1->IsAutoProcessed())
			m_autoProcessedModules.push_back(tmp1);
	}
}

/**
* @brief Destroy all modules
*/
void Manager::Destroy()
{
	PrintStatistics();

	for(auto & elem : m_modules)
		delete elem.second;
	m_modules.clear();

	for(auto & elem : m_parameters)
		delete elem.second;
	m_parameters.clear();
	m_inputs.clear();
	m_autoProcessedModules.clear();
}

/**
* @brief Destroy and rebuild all modules
*/
void Manager::Rebuild()
{
// #ifdef MARKUS_NO_GUI
	Stop();
	Destroy();
	Build();
	for(auto& elem : m_modules)
		elem.second->SetContext(RefContext());
	m_isConnected = false;
	Connect();
	Reset();
	Start();
	/*
#else
	// TODO: Maybe one day improve the GUI in order to handle rebuild and then handle decentralized
	LOG_WARN(m_logger, "Manager::Rebuild can only be called when compiled without GUI.");
#endif
	*/
}

void Manager::Start()
{
	for(auto & elem : m_autoProcessedModules)
		elem->Start();
	Processable::Start();
}


void Manager::Stop()
{
	for(auto & elem : m_autoProcessedModules)
		elem->Stop();
	Processable::Stop();
}


/**
* @brief Connect the different modules
*/
void Manager::Connect()
{
	Processable::Reset();
	m_interruptionManager.Reset();

	if(m_isConnected)
		throw MkException("Manager can only connect modules once", LOC);

	// Connect input and output streams (re-read the config once since we need all modules to be connected)
	for(const auto& moduleConfig : m_param.config.FindAll("module"))
	{
		int moduleId = boost::lexical_cast<int>(moduleConfig.GetAttribute("id"));
		Module& module = RefModuleById(moduleId);

		// For each module
		// Read conections of inputs
		for(const auto& inputConfig : moduleConfig.Find("inputs").FindAll("input"))
		{
			ConnectInput(inputConfig, module, boost::lexical_cast<int>(inputConfig.GetAttribute("id")));

			// Connect all sub-inputs: only used in case of multiple streams
			for(const auto& subInputConfig : inputConfig.FindAll("input"))
			{
				ConnectInput(subInputConfig, module, boost::lexical_cast<int>(inputConfig.GetAttribute("id")));
			}
		}

		// Add to depending modules, using master parameter
		if(!module.GetParameters().GetParameterByName("master").GetValueString().empty())
			RefModuleByName(module.GetParameters().GetParameterByName("master").GetValueString()).AddDependingModule(module);
	}
	Check();
	m_isConnected = true;
}

/**
* @brief Check if modules are correctly connected
*
*/
void Manager::Check() const
{
	for(auto& elem : m_modules)
	{
		if(!elem.second->IsInput())
		{
			bool bc = false;
			// Check that at least one blocking input is connected
			for(auto& input : elem.second->GetInputStreamList())
			{
				if(input.second->IsBlocking() && input.second->IsConnected())
				{
					bc = true;
					break;
				}
			}
			if(!bc)
				throw MkException("Module " + elem.second->GetName() + " must have at least one blocking input that is connected", LOC);
		}
	}
}

/**
* @brief Reset the manager: must be called externally after initialization
*
* @param x_resetInputs Also reset input modules (true by default)
*/
void Manager::Reset(bool x_resetInputs)
{
	Processable::Reset();

	// Reset timers
	// m_timerConvertion = 0;

	// Reset all modules (to set the module timer)
	for(auto & elem : m_modules)
	{
		if(x_resetInputs || !elem.second->IsInput())
		{
			// If manager is in autoprocess, modules must not be
			Processable::WriteLock lock(elem.second->RefLock());
			elem.second->Reset();
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
		Controller* ctr = Factories::parameterControllerFactory().Create(elem->GetParameterType(), *elem, *this);
		if(ctr == nullptr)
			throw MkException("Controller creation failed", LOC);
		else AddController(ctr);
	}
	m_frameCount      = 0;
}

/**
* @brief All modules process one frame
*
* @return False if the processing must be stopped
*/
void Manager::Process()
{
	WriteLock lock(RefLock());
	assert(m_isConnected); // Modules must be connected before processing
	MkException lastException(MK_EXCEPTION_NORMAL, "normal", "No exception was thrown", "", "");

	// To avoid freeze and infinite loops, use a timer
	std::future<int> ret = std::async(std::launch::async, [this, &lastException]()
	{
		int cptExceptions = 0;
		if(m_autoProcessedModules.empty())
			throw FatalException("Manager must contain at least one auto processed module (i.e. an input module). Possibly a rebuild command went wrong", LOC);

		for(auto & elem : m_autoProcessedModules)
		{
			LOG_DEBUG(m_logger, "Call Process on module " << elem->GetName());
			elem->ProcessAndCatch();

			if(!elem->HasRecovered())
			{
				cptExceptions++;
				LOG_WARN(m_logger, "The manager found an exception in " << elem->GetName());
				lastException = elem->LastException();
			}
		}
		m_frameCount++;
		if(m_frameCount % 100 == 0 && m_logger->isDebugEnabled())
		{
			PrintStatistics();
		}

		return cptExceptions;
	});

	std::future_status status = ret.wait_for(std::chrono::seconds(PROCESS_TIMEOUT));
	if (status != std::future_status::ready)
	{
		throw ProcessFreezeException("Timeout while processing. Check for freezes and infinite loops.", LOC);
	}
	int result = ret.get();
	if(result > 0)
	{
		LOG_WARN(m_logger, "Found " << result << " exception(s), the last one is " << lastException.SerializeToString());
		throw lastException;
	}
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
	contr.FindController(elems.at(1)).CallAction(elems.at(2), &x_value);

	LOG_INFO(m_logger, "Command " << x_command << " returned value '" << TRUNCATE_STRING(x_value) << "'");
}

/**
* @brief Print the results of timings
*/
void Manager::PrintStatistics()
{
	double fps = m_frameCount / m_timerProcessable.GetSecDouble();
	LOG_INFO(m_logger, "Manager: " << m_frameCount << " frames processed in " << m_timerProcessable.GetSecDouble() * 1000 << " ms, " << fps << " fps");
	// LOG_INFO("input convertion "                  <<m_timerConvertion<<" ms ("<<(1000.0 * m_frameCount) / m_timerConvertion<<" frames/s)");
	// LOG_INFO("Total time "<< m_timerProcessable + m_timerConvertion<<" ms ("<<     (1000.0 * m_frameCount) /(m_timerProcessable + m_timerConvertion)<<" frames/s)");

	// Create an XML file to summarize CPU usage
	//     if output dir is empty, write to /tmp
	// TODO: Remove previous file if exist
	bool notEmpty = IsContextSet() && !RefContext().IsOutputDirEmpty(); // must be called before ReserveFile
	string benchFileName = notEmpty ? RefContext().ReserveFile("benchmark.xml") : "/tmp/benchmark" + timeStamp() +  ".xml";
	ConfigFile benchSummary(benchFileName, true);
	ConfigReader conf = benchSummary.FindRef("benchmark", true);

	// Write perf to output XML
	ConfigReader perfModule = conf.FindRef("manager", true);
	perfModule.FindRef("nb_frames", true).SetValue(m_frameCount);
	perfModule.FindRef("timer[name=\"processable\"]", true).SetValue(m_timerProcessable.GetSecDouble()*1000);
	perfModule.FindRef("timer[name=\"processing\"]", true).SetValue(m_timerProcessable.GetSecDouble()*1000); // note: this line is kept for backward compatibility
	perfModule.FindRef("fps", true).SetValue(fps);

	// Call for each module
	for(const auto& module : m_modules)
	{
		// LOG_INFO(cpt<<": ");
		module.second->PrintStatistics(conf);
	}
	benchSummary.SaveToFile(benchFileName);
}

/**
* @brief Check if end of all input streams
*
* @return True if all streams have ended
*/
bool Manager::AbortCondition() const
{
	if(m_quitting || (m_param.nbFrames != 0 && m_frameCount >= m_param.nbFrames))
		return true;

	// Check that all input streams are finished
	for(const auto & elem : m_inputs)
	{
		if(!elem->AbortCondition())
		{
			return false;
		}
	}
	return true;
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

			ParameterStructure* parameters = mr_parametersFactory.Create(moduleType, moduleConfig);
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
	{
		if(elem.first == x_id)
		{
			module = elem.second;
			found++;
		}
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
		if(elem.second->GetName() == x_name)
			return *elem.second;
	throw MkException("Cannot find module " + x_name, LOC);
}

/**
* @brief Save the configuration of manager and modules to file
*/
void Manager::WriteConfig(ConfigReader xr_config) const // TODO unit test
{
	// Set all config ready to be saved
	for(auto & elem : m_modules)
		elem.second->WriteConfig(xr_config.FindRef("module[name=\"" + elem.second->GetName() + "\"]"));
	m_param.Write(xr_config);
	GetContext().WriteConfig(xr_config);
}

/**
* @brief Write the states of all modules to disk
*
* @param x_directory Output directory
*/
void Manager::WriteStateToDirectory(const string& x_directory)
{
	RefContext().MkDir(x_directory);
	for(const auto & elem : m_modules)
	{
		ofstream of(RefContext().ReserveFile(x_directory + "/" + elem.second->GetName() + ".json"));
		elem.second->Serialize(of, GetContext().GetOutputDir() + "/" + x_directory);
	}
	LOG_INFO(m_logger, "Written state of the manager and all modules to " << GetContext().GetOutputDir() + "/" + x_directory);
}


/**
* @brief Connect one input given the content of the config. Used by Connect
*
* @param x_inputConfig Config of the input
* @param xr_module     Module to connect
* @param x_inputId     Id of input
*/
void Manager::ConnectInput(const ConfigReader& x_inputConfig, Module& xr_module, int x_inputId) const
{
	// Check if connected to our previous module
	try
	{
		const string& tmp1 = x_inputConfig.GetAttribute("moduleid", "");
		const string& tmp2 = x_inputConfig.GetAttribute("outputid", "");
		const string& tmp3 = x_inputConfig.GetAttribute("block", "1");
		const string& tmp4 = x_inputConfig.GetAttribute("sync", "1");

		// Connection of a simple input
		if(tmp1 != "" && tmp2 != "")
		{
			LOG_DEBUG(m_logger, "Connect input " + to_string(x_inputId) + " of module " + xr_module.GetName() + " to output " + tmp1 + ":" + tmp2);

			int outputModuleId   = boost::lexical_cast<int>(tmp1);
			int outputId         = boost::lexical_cast<int>(tmp2);
			Stream& inputStream  = xr_module.RefInputStreamById(x_inputId);
			inputStream.SetBlocking(boost::lexical_cast<bool>(tmp3));
			inputStream.SetSynchronized(boost::lexical_cast<bool>(tmp4));
			Stream& outputStream = RefModuleById(outputModuleId).RefOutputStreamById(outputId);

			// Connect input and output streams
			inputStream.Connect(&outputStream);
			if(xr_module.GetParameters().GetParameterByName("master").GetValueString().empty())
				RefModuleById(outputModuleId).AddDependingModule(xr_module);
		}
		else if(tmp1.empty() && ! tmp2.empty()) throw MkException("Exception while connecting input: missing moduleid in config", LOC);
		else if(! tmp1.empty() && tmp2.empty()) throw MkException("Exception while connecting input: missing outputid in config", LOC);
	}
	catch(MkException& e)
	{
		LOG_ERROR(m_logger, "Cannot connect input "<<x_inputConfig.GetAttribute("id", "(unknown)")<<" of module "<<xr_module.GetName());
		throw;
	}
}


/**
* @brief Manage all interruptions
*
*/
bool Manager::ManageInterruptions(bool x_continueFlag)
{
	// the quit command was sent. Quit anyway
	if(m_quitting)
		return false;

	if(m_param.nbFrames != 0 && m_frameCount >= m_param.nbFrames)
		x_continueFlag = false;

	if(!x_continueFlag && AbortCondition())
		InterruptionManager::GetInst().AddEvent("event.stopped");

	//if(m_frameCount % 20 == 0)
	usleep(20); // This keeps the manager unlocked to allow the sending of commands
	vector<Command> commands = m_interruptionManager.ReturnCommandsToSend();
	for(const auto& command : commands)
	{
		try
		{
			SendCommand(command.name, command.value);
		}
		catch(std::exception& e)
		{
			LOG_WARN(m_logger, "Cannot execute command \"" << command.name << "\" "<<e.what());
		}
		catch(...)
		{
			LOG_WARN(m_logger, "Cannot execute command \"" << command.name << "\"");
		}
	}
	if(x_continueFlag)
		return true;

	if(AbortCondition())
	{
		LOG_INFO(m_logger, "Abort condition has been fulfilled (end of all streams)");
		return false;
	}
	else
	{
		LOG_INFO(m_logger, "Abort condition is not fulfilled. Continue processing.");
		return true;
	}
}

/**
* @brief Log the status of the application (last exception)
*/
/* Enable this for a more verbose exception status
void Manager::Status() const
{
	Json::Value root;

	for(auto module : m_modules)
	{
		if(module->LastException().GetCode() != 1000)
		{
			stringstream ss;
			module->LastException().Serialize(ss, "");
			ss >> root[module->GetName()];
			root[module->GetName()]["recovered"] = module->HasRecovered();
		}
	}
	stringstream ss;
	LastException().Serialize(ss, "");
	ss >> root["manager"];
	root["manager"]["recovered"] = HasRecovered();

	Event evt;
	// note: it is difficult to associate a time stamp with events
	evt.Raise("status", 0, 0);
	ss.clear();
	ss << root;
	evt.AddExternalInfo("exceptions", ss);
	evt.Notify(GetContext(), true);
}
*/


int Manager::ReturnCode() const
{
	// Return no error if end-of-stream or if has recovered
	if(HasRecovered() || LastException().GetCode() == MK_EXCEPTION_ENDOFSTREAM)
		return MK_EXCEPTION_NORMAL - MK_EXCEPTION_FIRST;
	else
		return LastException().GetCode() - MK_EXCEPTION_FIRST;
}

