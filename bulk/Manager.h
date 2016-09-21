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
		Parameters(const ConfigReader& x_confReader) : Processable::Parameters(x_confReader.GetAttribute("name")), config(x_confReader)
		{
			AddParameter(new ParameterInt("nb_frames", 0, 0, INT_MAX, &nbFrames, "Number of frames to process. 0 for infinite. Only works in centralized mode"));
			AddParameter(new ParameterString("arguments", "",         &arguments, "Command-line arguments, for storage only"));
			AddParameter(new ParameterString("aspect_ratio", "", &aspectRatio, "If non-empty, at creation each module width/height are changed to match this aspect ratio. E.g. \"4:3\"."));
		}
		int nbFrames;
		std::string arguments; // note: This is used in simulations, see what to do in normal case
		std::string aspectRatio;
		ConfigReader config;
	};

	Manager(ParameterStructure& xr_params);
	virtual ~Manager();
	virtual void Reset(bool x_resetInputs = true);
	virtual void Process() override;
	virtual bool AbortCondition() const override;
	int ReturnCode() const;
	virtual void Start() override;
	virtual void Stop() override;

	// Interface for interaction with external objects
	void SendCommand(const std::string& x_command, std::string x_value);
	void ConnectExternalInput(Stream& xr_input, const std::string& x_moduleName, int x_outputId);

	const std::vector<Module*> RefModules() const
	{
		std::vector<Module*> modules; 
		for(auto elem : m_modules)
		{
			modules.push_back(elem.second);
		}
		return modules;
	}
	inline const Module& GetModuleByName(const std::string& x_name) const {return RefModuleByName(x_name);}
	inline const Processable& GetProcessableByName(const std::string& x_name) const {if(x_name == "manager") assert(false); else return RefModuleByName(x_name);}

	void Rebuild();
	void Connect();
	void Check() const;
	void CreateEditorFiles(const std::string& x_fileName);
	void PrintStatistics();
	virtual bool ManageInterruptions(bool x_continueFlag) override;
	inline void Quit() {m_quitting = true;}
	inline void ListModulesTypes(std::vector<std::string>& xr_types) const {mr_moduleFactory.List(xr_types);}
	inline void ListModulesNames(std::vector<std::string>& xr_names) const
	{
		for(auto elem : m_modules)
		{
			xr_names.push_back(elem.second->GetName());
		}
	}
	void WriteStateToDirectory(const std::string& x_directory);
	virtual void WriteConfig(ConfigReader xr_config) const override;
	inline virtual void SetContext(Context& x_context) override
	{
		Processable::SetContext(x_context);
		for(auto& elem : m_modules)
			elem.second->SetContext(x_context);
	}
	virtual const std::string& GetName() const override {static std::string str = "manager"; return str;}
	virtual double GetRecordingFps() const override
	{
		assert(!m_inputs.empty());
		return m_inputs.at(0)->GetRecordingFps();
	}
	// virtual void Status() const override;

private:
	void Build();
	void Destroy();

	Module& RefModuleById(int x_id) const;
	Module& RefModuleByName(const std::string& x_name) const;
	void ConnectInput(const ConfigReader& x_inputConfig, Module& xr_module, int x_inputId) const;

	int64_t m_frameCount = 0;
	bool m_isConnected   = false;
	bool m_quitting      = false;

	std::map<int, Module *> m_modules;
	std::vector<Input *>    m_inputs;
	std::vector<Module *>   m_autoProcessedModules;
	std::map<int, ParameterStructure *> m_parameters;

	const FactoryParameters& mr_parametersFactory;
	const FactoryModules& mr_moduleFactory;

	static log4cxx::LoggerPtr m_logger;
	const Parameters& m_param;
};
#endif
