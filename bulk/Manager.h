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
#include "Input.h"
#include "Module.h"
#include "Timer.h"
#include "FactoryModules.h"


class ManagerParameter : public ParameterStructure
{
public:
	ManagerParameter(const ConfigReader& x_confReader) : ParameterStructure(x_confReader)
	{
		// m_list.push_back(new ParameterString("mode",	"", 	&mode, 	"Mode"));
		ParameterStructure::Init();
	}
	// std::string mode; // unused
};

/**
* @brief The manager handles all modules
*/
class Manager : public Configurable, public Controllable
{
public:
	Manager(const ConfigReader& x_configReader, bool x_centralized);
	~Manager();
	void Reset(bool x_resetInputs = true);
	bool Process();
	void SendCommand(const std::string& x_command, std::string x_value);
	const std::vector<Module*>& GetModules() const {return m_modules; }
	
	void Connect();
	void Export();
	void PrintStatistics();
	void Pause(bool x_pause);
	inline void Quit(){m_continueFlag = false;}
	void Status() const;
	void PauseInputs(bool x_pause);
	bool EndOfAllStreams() const;
	static std::string Version(bool x_full);
	static void NotifyMonitoring(const std::string& x_eventName);
	static const std::string& OutputDir(const std::string& x_outputDir = "", const std::string& x_configFile = "");
	static inline void ListModules(std::vector<std::string>& xr_types) {m_factory.ListModules(xr_types);}
	void WriteStateToDirectory(const std::string& x_directory) const;
	void UpdateConfig();
	inline static const std::string& GetApplicationName(){return m_applicationName;}

protected:
	Module& RefModuleById(int x_id) const;
	Module& RefModuleByName(const std::string& x_name) const;
	inline virtual const ParameterStructure& GetParameters() const {return m_param;}
	void NotifyException(const MkException& x_exeption);

	ManagerParameter m_param;
	bool m_centralized;
	bool m_isConnected;
	// long long m_timerConvertion;
	long long m_timerProcessing;
	bool m_continueFlag; // Flag that is used to notify the manager of a Quit command, only working if centralized
	bool m_hasRecovered; // Flag to test if all modules have recovered from the last exception, only working if centralized
	MkException m_lastException; // Field to store the last exception

	std::vector<Module *> m_modules;
	std::vector<Input  *> m_inputs;

	long long m_frameCount;
	static std::string m_applicationName;
	static log4cxx::LoggerPtr m_logger;
	static std::string m_outputDir;
	static FactoryModules m_factory;

protected:
	QReadWriteLock m_lock;
};
#endif
