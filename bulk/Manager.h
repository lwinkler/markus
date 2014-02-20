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
#include "Input.h"
#include "Module.h"
#include "Timer.h"
#include "FactoryModules.h"


/// Parameter class

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

/// Class for managing all modules

class Manager : public Configurable
{
public:
	Manager(const ConfigReader& x_configReader, const ConfigReader& x_mainConfigReader, bool x_centralized);
	~Manager();
	void Reset();
	bool Process();
	void SendCommand(const std::string& x_command, std::string x_value);
	// const std::vector<Input*> & GetInputList()  const {return m_inputs; };
	const std::vector<Module*>& GetModules() const {return m_modules; }

	//std::vector<Input*> & GetInputListVar()  {return m_inputs; };
	//std::vector<Module*>& RefModules() {return m_modules; }
	
	Module& RefModuleById(int x_id) const;
	Module& RefModuleByName(const std::string& x_name) const;
	
	void Connect();
	void Export();
	void PrintTimers();
	void Pause(bool x_pause);
	void PauseInputs(bool x_pause);
	bool EndOfAllStreams() const;
	static const std::string& OutputDir(const std::string& x_outputDir = "");
	inline static log4cxx::LoggerPtr& Logger(){return m_logger;}
	static inline void SetConfigFile(const std::string& x_configFile){
		assert(m_configFile.size() == 0);
		m_configFile = x_configFile;
	}
	static inline const std::string& GetConfigFile(){return m_configFile;}
	static inline void ListModules(std::vector<std::string>& xr_types) {m_factory.ListModules(xr_types);};
	void SaveConfigToFile(const std::string& x_fileName);

private:
	ManagerParameter m_param;

	bool m_centralized;
	bool m_isConnected;
	// long long m_timerConvertion;
	long long m_timerProcessing;

	//clock_t m_timeLastProcess;
	
	std::vector<Module *> m_modules;
	std::vector<Input  *> m_inputs;

	long long m_frameCount;
	static log4cxx::LoggerPtr m_logger;
	static std::string m_configFile;
	static std::string m_outputDir;
	static FactoryModules m_factory;
	const ConfigReader& m_mainConfig; // Only used to write the xml file at destruction

protected:
	inline virtual const ParameterStructure& GetParameters() const {return m_param;}
	QReadWriteLock m_lock;
};
#endif
