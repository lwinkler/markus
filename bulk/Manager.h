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
#include "FactoryModules.h"
#include "Context.h"


class Input;

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
			m_list.push_back(new ParameterBool("auto_clean", 0, 0, 1,     &autoClean, "Automatically clean the temporary directory when the application closes"));
			m_list.push_back(new ParameterBool("fast", 0, 0, 1,           &fast, "Run as fast as possible: Inputs are not in real-time"));
			m_list.push_back(new ParameterString("archive_dir", "",       &archiveDir, "If specified the data is copied inside this directory for archive"));
			m_list.push_back(new ParameterInt("nb_frames", 0, 0, INT_MAX, &nbFrames, "Number of frames to process. 0 for infinite. Only works in centralized mode"));
			ParameterStructure::Init();
		}
		bool autoClean;
		bool fast;
		std::string archiveDir;
		int nbFrames;
	};

	Manager(const ConfigReader& x_configReader);
	~Manager();
	virtual void Reset(bool x_resetInputs = true);
	virtual bool Process();
	void SendCommand(const std::string& x_command, std::string x_value);
	const std::vector<Module*>& GetModules() const {return m_modules; }
	
	void Connect();
	void Export();
	void PrintStatistics();
	virtual void Pause(bool x_pause);
	inline void Quit(){m_continueFlag = false;}
	void Status() const;
	void PauseInputs(bool x_pause);
	bool EndOfAllStreams() const;
	static std::string Version(bool x_full);
	// static void NotifyMonitoring(const std::string& x_eventName);
	static std::string CreateOutputDir(const std::string& x_outputDir = "", const std::string& x_configFile = "");
	static inline void ListModules(std::vector<std::string>& xr_types) {m_factory.ListModules(xr_types);}
	void WriteStateToDirectory(const std::string& x_directory) const;
	void UpdateConfig();
	// inline static const std::string& GetApplicationName(){return m_applicationName;}
	inline void SetContext(const Context& x_context)
	{
		m_context = x_context;
		for(std::vector<Module*>::iterator it = m_modules.begin() ; it != m_modules.end() ; ++it)
			(*it)->SetContext(x_context);
	}

protected:
	Module& RefModuleById(int x_id) const;
	Module& RefModuleByName(const std::string& x_name) const;
	inline virtual const Parameters& GetParameters() const {return m_param;}
	void NotifyException(const MkException& x_exeption);

	bool m_isConnected;
	// long long m_timerConvertion;
	long long m_timerProcessing;
	bool m_continueFlag;           // Flag that is used to notify the manager of a Quit command, only working if centralized
	bool m_hasRecovered;           // Flag to test if all modules have recovered from the last exception, only working if centralized
	MkException m_lastException;   // Field to store the last exception
	Context m_context;    // Context of the running program

	std::vector<Module *> m_modules;
	std::vector<Module  *> m_inputs;

	long long m_frameCount;
	static log4cxx::LoggerPtr m_logger;
	static const FactoryModules m_factory;

protected:
	QReadWriteLock m_lock;

private:
        inline Parameters& RefParameters() {return m_param;}
	Parameters m_param;
};
#endif
