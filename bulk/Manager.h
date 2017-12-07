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

#include "Configurable.h"
#include "Controller.h"
#include "Timer.h"
#include "Processable.h"
#include "InterruptionManager.h"
#include "Factories.h"
#include "Module.h"
#include "Input.h"
#include "ConfigReader.h"


/**
* @brief The manager handles all modules
*/
class Manager : public Processable, public Controllable
{
public:
	class Parameters : public Processable::Parameters
	{
	public:
		explicit Parameters(const ConfigReader& x_confReader) : Processable::Parameters("manager"), config(x_confReader)
		{
			AddParameter(new ParameterInt("nbFrames", 0, 0, INT_MAX, &nbFrames, "Number of frames to process. 0 for infinite. Only works in centralized mode"));
			AddParameter(new ParameterString("arguments", "",         &arguments, "Command-line arguments, for storage only"));
			AddParameter(new ParameterString("aspectRatio", "", &aspectRatio, "If non-empty, at creation each module width/height are changed to match this aspect ratio. E.g. \"4:3\"."));
		}
		int nbFrames;
		std::string arguments; // note: This is used in simulations, see what to do in normal case
		std::string aspectRatio;
		ConfigReader config;
	};

	Manager(ParameterStructure& xr_params, Context& xr_context);
	virtual ~Manager();
	virtual void Reset(bool x_resetInputs = true);
	void Process() override;
	bool AbortCondition() const override;
	int ReturnCode() const;
	void Start() override;
	void Stop() override;

	// Interface for interaction with external objects
	void SendCommand(const std::string& x_command, mkjson x_value);
	void SendCommand(const std::string& x_command);
	void ConnectExternalInput(Stream& xr_input, const std::string& x_moduleName, const std::string& x_outputStreamName);
	inline void CallModuleMethod(const std::string& x_moduleName, std::function<void(Module*)> x_method)
	{
		Module& mod(RefModuleByName(x_moduleName));
		// WriteLock lock(mod.RefLock());
		x_method(&mod);
	}
	inline Lock& LockModuleByName(const std::string& x_moduleName) {return RefModuleByName(x_moduleName).RefLock();}

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
	inline const Processable& GetProcessableByName(const std::string& x_name) const
	{
		if(x_name == "manager")
			assert(false);
		else 
			return RefModuleByName(x_name);
	}

#ifndef MARKUS_NO_GUI
	inline QWidget* CreateControllerWidget(const std::string& x_moduleName, const std::string& x_controllerName)
	{
		return RefModuleByName(x_moduleName).FindController(x_controllerName).CreateWidget();
	}
#endif

	void Rebuild();
	void Connect();
	void Check() const;
	void CreateEditorFiles(const std::string& x_fileName);
	void PrintStatistics();
	bool ManageInterruptions(bool x_continueFlag) override;
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
	void WriteConfig(ConfigReader& xr_config) const override;
	void SetContext(Context& x_context) override
	{
		Processable::SetContext(x_context);
		for(auto& elem : m_modules)
			elem.second->SetContext(x_context);
	}
	const std::string& GetName() const override {static std::string str = "manager"; return str;}
	double GetRecordingFps() const override
	{
		assert(!m_inputs.empty());
		return m_inputs.front()->GetRecordingFps();
	}
	void BuildModule(const std::string& x_name, const ConfigReader& x_moduleConfig);

private:
	void Build();
	void Destroy();
	int Process2(MkException& xr_lastException);

	Module& RefModuleByName(const std::string& x_name) const;
	void ConnectInput(const ConfigReader& x_inputConfig, Module& xr_module, const std::string& x_input) const;

	int64_t m_frameCount = 0;
	bool m_isConnected   = false;
	bool m_quitting      = false;

	std::map<std::string, Module *> m_modules;
	std::vector<Input *>    m_inputs;
	std::vector<Module *>   m_autoProcessedModules;
	std::vector<ParameterStructure *> m_parameters;

	const FactoryParameters& mr_parametersFactory;
	const FactoryModules& mr_moduleFactory;

	static log4cxx::LoggerPtr m_logger;
	const Parameters& m_param;
};
#endif
