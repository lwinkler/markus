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

#ifndef MANAGER_H
#define MANAGER_H

#include <QReadWriteLock>

#include "ConfigReader.h"
#include "Controller.h"
#include "Timer.h"
#include "Processable.h"
#include "InterruptionManager.h"
#include "Factories.h"
#include "ParameterString.h"
#include "Module.h"


/**
* @brief The manager handles all modules
*/
class Manager : public Processable, public Controllable
{
public:
	class Parameters : public Processable::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) : Processable::Parameters(x_confReader)
		{
			m_list.push_back(new ParameterBool("fast", 0, 0, 1,           &fast, "Run as fast as possible: Inputs are not in real-time"));
			m_list.push_back(new ParameterInt("nb_frames", 0, 0, INT_MAX, &nbFrames, "Number of frames to process. 0 for infinite. Only works in centralized mode"));
			m_list.push_back(new ParameterString("arguments", "",         &arguments, "Command-line arguments, for storage only"));
			ParameterStructure::Init();
		}
		bool fast;
		int nbFrames;
		std::string arguments; // TODO: See what to do in normal case
	};

	Manager(ParameterStructure& x_configReader);
	~Manager();
	virtual void Reset(bool x_resetInputs = true);
	virtual bool Process();
	void SendCommand(const std::string& x_command, std::string x_value);
	const std::vector<Module*>& GetModules() const {return m_modules; }
	inline const Processable& GetModuleByName(const std::string& x_name) const {if(x_name == "manager") assert(false); else return RefModuleByName(x_name);}

	void Connect();
	void Export();
	void PrintStatistics();
	virtual void Pause(bool x_pause);
	inline void Quit() {m_continueFlag = false;}
	void Status() const;
	void PauseInputs(bool x_pause);
	bool EndOfAllStreams() const;
	static std::string CreateOutputDir(const std::string& x_outputDir = "", const std::string& x_configFile = "");
	inline void ListModulesTypes(std::vector<std::string>& xr_types) {mr_moduleFactory.List(xr_types);}
	void WriteStateToDirectory(const std::string& x_directory) const;
	void UpdateConfig();
	inline virtual void SetContext(const Context& x_context)
	{
		Processable::SetContext(x_context);
		for(auto& elem : m_modules)
			elem->SetContext(x_context);
	}

protected:
	Module& RefModuleById(int x_id) const;
	Module& RefModuleByName(const std::string& x_name) const;
	void NotifyException(const MkException& x_exeption);

	bool m_isConnected;
	// long long m_timerConvertion;
	Timer m_timerProcessing;
	bool m_continueFlag;           // Flag that is used to notify the manager of a Quit command, only working if centralized
	bool m_hasRecovered;           // Flag to test if all modules have recovered from the last exception, only working if centralized
	MkException m_lastException;   // Field to store the last exception

	std::vector<Module *> m_modules;
	std::vector<Module *> m_inputs;
	std::vector<ParameterStructure *> m_parameters;

	long long m_frameCount;
	const FactoryParameters& mr_parametersFactory;
	const FactoryModules& mr_moduleFactory;
	const FactoryParameters& mr_parameterFactory;
	InterruptionManager& m_interruptionManager;

private:
	static log4cxx::LoggerPtr m_logger;
	Parameters& m_param;
};
#endif
