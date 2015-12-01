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

#include "ConfigReader.h"
#include "Controller.h"
#include "Timer.h"
#include "Processable.h"
#include "InterruptionManager.h"
#include "Factories.h"
#include "ParameterString.h"
#include "Module.h"
#include "Input.h"


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
			m_list.push_back(new ParameterInt("nb_frames", 0, 0, INT_MAX, &nbFrames, "Number of frames to process. 0 for infinite. Only works in centralized mode"));
			m_list.push_back(new ParameterString("arguments", "",         &arguments, "Command-line arguments, for storage only"));
			ParameterStructure::Init();
		}
		int nbFrames;
		std::string arguments; // TODO: This is used in simulations, see what to do in normal case
	};

	Manager(ParameterStructure& x_configReader);
	~Manager();
	virtual void Reset(bool x_resetInputs = true);
	virtual void Process() override;
	virtual bool AbortCondition() const override;
	virtual void Start() override;
	virtual void Stop() override;
	void SendCommand(const std::string& x_command, std::string x_value);
	const std::vector<Module*>& GetModules() const {return m_modules; }
	inline const Processable& GetModuleByName(const std::string& x_name) const {if(x_name == "manager") assert(false); else return RefModuleByName(x_name);}

	void Connect();
	void Check() const;
	void CreateEditorFiles(const std::string& x_fileName);
	void PrintStatistics();
	inline void Quit() {Stop(); m_quitting = true;} // TODO: Implement this ?
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
	virtual const std::string& GetName() const override {static std::string str = "manager"; return str;}
	virtual double GetRecordingFps() const override
	{
		assert(!m_inputs.empty());
		return m_inputs.at(0)->GetRecordingFps();
	}

protected:
	Module& RefModuleById(int x_id) const;
	Module& RefModuleByName(const std::string& x_name) const;
	void NotifyException(const MkException& x_exeption);

	int64_t m_frameCount = 0;
	bool m_isConnected   = false;
	bool m_quitting      = false;

	std::vector<Module *>  m_modules;
	std::vector<Input *>   m_inputs;
	std::vector<Module *>  m_autoProcessedModules;
	std::vector<ParameterStructure *> m_parameters;

	const FactoryParameters& mr_parametersFactory;
	const FactoryModules& mr_moduleFactory;

private:
	static log4cxx::LoggerPtr m_logger;
	Parameters& m_param;
};
#endif
