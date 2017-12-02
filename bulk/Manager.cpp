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
#include "json.hpp"

#include <fstream>
#include <future>
#include <thread>
#include <chrono>
#include <boost/lexical_cast.hpp>

#define PROCESS_TIMEOUT 60 // 1 min timeout

using namespace std;

log4cxx::LoggerPtr Manager::m_logger(log4cxx::Logger::getLogger("Manager"));


Manager::Manager(ParameterStructure& xr_params, Context& xr_context) :
	Processable(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params)),
	mr_parametersFactory(Factories::parametersFactory()),
	mr_moduleFactory(Factories::modulesFactory())
{
	LOG_INFO(m_logger, "Create manager");
	SetContext(xr_context);
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
	if(m_param.config["modules"].is_null())
		throw MkException("Config is null", LOC);
	for(const auto& elem : m_param.config["modules"])
	{
		BuildModule(elem["name"].get<string>(), elem);
	}
}

void Manager::BuildModule(const string& x_name, const ConfigReader& x_moduleConfig)
{
	// Read parameters
	string moduleType = x_moduleConfig["class"].get<string>();
	ParameterStructure * tmp2 = mr_parametersFactory.Create(moduleType, x_name);
	tmp2->Read(x_moduleConfig);

	// note: Commented for now: TODO: initialize the inputs as real parameters
	// tmp2->CheckRange(x_moduleConfig);

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
	m_modules[tmp1->GetName()] = tmp1;
	m_parameters.push_back(tmp2);

	// Add to inputs if an input
	if(tmp1->IsInput())
	{
		Input* tmpi = dynamic_cast<Input*>(tmp1);
		assert(tmpi != nullptr);
		m_inputs.push_back(tmpi);
	}
	if(tmp1->IsAutoProcessed())
		m_autoProcessedModules.push_back(tmp1);

	tmp1->SetContext(RefContext());
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
		delete elem;
	m_parameters.clear();
	m_inputs.clear();
	m_autoProcessedModules.clear();
}

/**
* @brief Destroy and rebuild all modules
*/
void Manager::Rebuild()
{
	Stop();
	Destroy();
	Build();
	m_isConnected = false;
	Connect();
	Reset();
	Check();
	for(const auto& elem : m_modules)
		elem.second->CheckParameterRange();
	Start();
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
	for(const auto& conf : m_param.config["modules"])
	{
		Module& module = RefModuleByName(conf.at("name").get<string>());

		// For each module
		// Read conections of inputs
		for(const auto& inputConf : conf.at("inputs"))
		{
			const auto& inputConfig(inputConf);
			if(!inputConfig.is_object() || inputConfig.find("connected") == inputConfig.end())
				continue;
			ConnectInput(inputConfig, module, inputConf.at("name").get<string>());

			// Connect all sub-inputs: only used in case of multiple streams
			if(inputConfig.find("inputs") == inputConfig.end())
				continue;
			for(const auto& subInputConfig : inputConfig.at("inputs"))
			{
				ConnectInput(subInputConfig, module, inputConf.at("name").get<string>());
			}
		}

		// Add to depending modules, using master parameter
		const auto& mas(module.GetParameters().GetParameterByName("master").GetValue());
		if(!mas.is_null() && !mas.get<string>().empty())
			RefModuleByName(mas.get<string>()).AddDependingModule(module);
	}
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
		if(!elem.second->IsAutoProcessed())
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
		Controller* ctr = Factories::parameterControllerFactory().Create(elem->GetType(), *elem, *this);
		if(ctr == nullptr)
			throw MkException("Controller creation failed", LOC);
		else AddController(ctr);
	}
	m_frameCount      = 0;
}


int Manager::Process2(MkException& rx_lastException)
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
			// note: we already log a warning later
			LOG_INFO(m_logger, "The manager found an exception in " << elem->GetName());
			rx_lastException = elem->LastException();
		}
	}
	m_frameCount++;
	if(m_frameCount % 100 == 0 && m_logger->isDebugEnabled())
	{
		PrintStatistics();
	}
	if(cptExceptions > 0)
		throw rx_lastException;

	return cptExceptions;
}

/**
* @brief All modules process one frame
*
* @return False if the processing must be stopped
*/
void Manager::Process()
{
	// WriteLock lock(RefLock());
	assert(m_isConnected); // Modules must be connected before processing
	MkException lastException(MK_EXCEPTION_NORMAL, "normal", "No exception was thrown", "", "");

	// TODO: Something goes wrong in opencv resize if we use the timer. Why ?
// #define MANAGER_TIMER
#ifdef MANAGER_TIMER
	// To avoid freeze and infinite loops, use a timer
	future<int> ret = async(launch::async, [this, &lastException]()
	{
		return Process2(lastException);
	});

	future_status status = ret.wait_for(chrono::seconds(PROCESS_TIMEOUT));
	if (status != future_status::ready)
	{
		throw ProcessFreezeException("Timeout while processing. Check for freezes and infinite loops.", LOC);
	}
	int result = ret.get();
	if(result > 0)
	{
		// note: we already log as a warning later in ProcessAndCatch
		LOG_INFO(m_logger, "Found " << result << " exception(s), the last one is " << lastException.SerializeToString());
		throw lastException;
	}
#else
	if(Process2(lastException) > 0)
		throw lastException;
#endif
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
* @brief Send a command
*
* @param x_command Command in format "module.controller.Command"
*/
void Manager::SendCommand(const string& x_command)
{
	vector<string> elems;
	split(x_command, '.', elems);
	if(elems.size() != 3)
		throw MkException("Command must be in format \"module.controller.Command\": " + x_command, LOC);

	// Note: We cast module/manager twice since we need functions from both parents
	Controllable& contr  (elems.at(0) == "manager" ? dynamic_cast<Controllable&>(*this) : RefModuleByName(elems.at(0)));
	contr.FindController(elems.at(1)).CallAction(elems.at(2), nullptr);
}

/**
* @brief Connect an input of an external module to one internal output
*
* @param xr_input     External output
* @param x_moduleName Internal module name
* @param x_outputId   Internal output id
*/
void Manager::ConnectExternalInput(Stream& xr_input, const string& x_moduleName, const std::string& x_outputStreamName)
{
	Module& mod(RefModuleByName(x_moduleName));
	// WriteLock lock(mod.RefLock());
	xr_input.Connect(mod.RefOutputStreamByName(x_outputStreamName));
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

	// Create a JSON file to summarize CPU usage
	//     if output dir is empty, write to /tmp
	bool notEmpty = IsContextSet() && !RefContext().RefOutputDir().IsEmpty(); // must be called before ReserveFile
	if(RefContext().RefOutputDir().FileExists("benchmark.json"))
		RefContext().RefOutputDir().Rm("benchmark.json");
	string benchFileName = notEmpty ? RefContext().RefOutputDir().ReserveFile("benchmark.json") : "/tmp/benchmark" + timeStamp() +  ".json";

	ConfigReader benchSummary;
	readFromFile(benchSummary, benchFileName, true);
	ConfigReader& conf(benchSummary["benchmark"]);

	// Write perf to output XML
	ConfigReader& perfModule(conf["manager"]);
	perfModule["nb_frames"]             = m_frameCount;
	perfModule["timers"]["processable"] = m_timerProcessable.GetMsLong();
	// perfModule["timers"]["processing"]  = Json::UInt64(m_timerProcessFrame.GetMsLong());
	perfModule["fps"]                   = fps;

	// Call for each module
	for(const auto& module : m_modules)
	{
		// LOG_INFO(cpt<<": ");
		module.second->PrintStatistics(conf);
	}
	writeToFile(benchSummary, benchFileName);
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
* @brief Export current configuration to json: this is used to create the JSON files to describe each module
*/
void Manager::CreateEditorFiles(const string& x_fileName)
{
	try
	{
		map<string,vector<string>> moduleCategories;
		mkjson moduleDescriptionsJson = mkjson::array();

		vector<string> moduleTypes;
		mr_moduleFactory.List(moduleTypes);
		for(const auto& moduleType : moduleTypes)
		{
			ParameterStructure* parameters = mr_parametersFactory.Create(moduleType, moduleType);
			Module* module = mr_moduleFactory.Create(moduleType, *parameters);

			moduleCategories[module->GetCategory()].push_back(moduleType);
			moduleCategories["all"].push_back(moduleType);

			// JSON file containing all module descriptions
			// TODO Improve
			stringstream os;
			module->Export(os);
			mkjson json;
			os >> json;
			moduleDescriptionsJson.push_back(json);
		}
		mkjson moduleCategoriesJson = mkjson::array();
		for(const auto& elem : moduleCategories) {
			mkjson json;
			json["name"]    = elem.first;
			json["modules"] = mkjson::array();
			for(const auto& mod : elem.second)
				json["modules"].push_back(mod);
			moduleCategoriesJson.push_back(json);
		}

		// Generate the js files containing
		ofstream os1("editor/modules/moduleDescriptions.json"); // all module descriptions
		os1 << moduleDescriptionsJson << endl;
		os1.close();
		ofstream os2("editor/modules/moduleCategories.json");
		os2 << moduleCategoriesJson << endl;
		os2.close();
	}
	catch(MkException& e)
	{
		LOG_ERROR(m_logger, "Exception in Manager::Export: "<<e.what());
		throw;
	}
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
void Manager::WriteConfig(ConfigReader& xr_config) const
{
	// Set all config ready to be saved
	for(auto & elem : m_modules)
		elem.second->WriteConfig(findFirstInArray(xr_config["modules"], "name", elem.second->GetName()));
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
	MkDirectory dir(x_directory, RefContext().RefOutputDir(), false);
	for(const auto & elem : m_modules)
	{
		ofstream of(dir.ReserveFile(elem.second->GetName() + ".json"));
		mkjson json;
		elem.second->Serialize(json, &dir);
		of << json;
	}
	LOG_INFO(m_logger, "Written state of the manager and all modules to " << dir.GetPath());
}


/**
* @brief Connect one input given the content of the config. Used by Connect
*
* @param x_inputConfig Config of the input
* @param xr_module     Module to connect
* @param x_inputName   Name of input
*/
void Manager::ConnectInput(const ConfigReader& x_inputConfig, Module& xr_module, const string& x_inputName) const
{
	// Check if connected to our previous module
	try
	{
		string conMod = x_inputConfig.at("connected").at("module").get<string>();
		string conOut = x_inputConfig.at("connected").at("output").get<string>();
		// Connection of a simple input
		LOG_DEBUG(m_logger, "Connect input " + x_inputName + " of module " + xr_module.GetName() + " to output " + conMod + ":" + conOut);

		Stream& inputStream  = xr_module.RefInputStreamByName(x_inputName);
		inputStream.SetBlocking(x_inputConfig.value<int>("block", 1));
		inputStream.SetSynchronized(x_inputConfig.value<int>("sync", 1));
		Stream& outputStream = RefModuleByName(conMod).RefOutputStreamByName(conOut);

		// Connect input and output streams
		inputStream.Connect(outputStream);
		if(xr_module.GetParameters().GetParameterByName("master").GetValue().get<string>().empty())
			RefModuleByName(conMod).AddDependingModule(xr_module);
	}
	catch(MkException& e)
	{
		LOG_ERROR(m_logger, "Cannot connect input " << x_inputName << " of module " << xr_module.GetName() << ". Reason: " << string(e.what()));
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
	usleep(0); // This keeps the manager unlocked to allow the sending of commands
	vector<Command> commands = m_interruptionManager.ReturnCommandsToSend();
	for(const auto& command : commands)
	{
		try
		{
			SendCommand(command.name, command.value);
		}
		catch(exception& e)
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
	mkjson root;

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

