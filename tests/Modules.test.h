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
#ifndef MODULES_TEST_H
#define MODULES_TEST_H

#include <cxxtest/TestSuite.h>
#include "Module.h"
#include "Controller.h"
#include "util.h"
#include "Factories.h"
#include "StreamEvent.h"
#include "StreamObject.h"
#include "StreamImage.h"
#include "StreamState.h"
#include "StreamNum.h"
#include "MkException.h"
#include "FeatureFloatInTime.h"
#include "FeatureVector.h"
#include "Timer.h"

using namespace std;

// #define BLACKLIST(x) m_moduleTypes.erase(remove(m_moduleTypes.begin(), m_moduleTypes.end(), (x)), m_moduleTypes.end());

#include <string>
#include <tuple>
#include <iostream>
#include <stdio.h>


/// Unit testing class for ConfigReader class
class ModulesTestSuite : public CxxTest::TestSuite
{
public:
	ModulesTestSuite() :
		  m_factoryParameters(Factories::parametersFactory()),
		  m_factoryModules(Factories::modulesFactory()),
		  m_factoryFeatures(Factories::featuresFactory())
		  {}
protected:
	vector<string> m_moduleTypes;
	const FactoryParameters&  m_factoryParameters;
	const FactoryModules&  m_factoryModules;
	const FactoryFeatures& m_factoryFeatures;
	ParameterStructure* mp_fakeConfig     = nullptr;
	Module* mp_fakeInput                  = nullptr;
	ConfigFile* mp_configFile             = nullptr;
	Context::Parameters* mp_contextParams = nullptr;
	Context* mp_context                   = nullptr;

	// Objects for streams
	cv::Mat m_image;
	bool m_state = false;
	Event m_event;
	float  m_float   = 0;
	double m_double  = 0;
	int    m_int     = 0;
	uint   m_uint    = 0;
	bool   m_bool    = false;
	vector<Object> m_objects;
	int m_cpt = 0;

public:
	void setUp()
	{
		m_cpt = 0;
		char* tmp = getenv("MODULE_TO_TEST");
		if(tmp != nullptr)
		{
			m_moduleTypes.push_back(tmp);
			TS_WARN("$MODULE_TO_TEST should only be used for development purposes.");
		}
		else
			m_factoryModules.List(m_moduleTypes);

		createEmptyConfigFile("/tmp/config_empty.xml");
		mp_configFile = new ConfigFile("/tmp/config_empty.xml");
		addModuleToConfig("VideoFileReader", *mp_configFile)
		.RefSubConfig("parameters", true)
		.RefSubConfig("param", "name", "fps", true).SetValue("22");
		mp_configFile->RefSubConfig("application").SetAttribute("name", "unitTest");
		mp_contextParams = new Context::Parameters(mp_configFile->Find("application"), "/tmp/config_empty.xml", "TestModule", "tests/out");
		mp_contextParams->centralized = true;
		mp_context = new Context(*mp_contextParams);
		mp_fakeConfig = m_factoryParameters.Create("VideoFileReader", mp_configFile->Find("application>module[name=\"VideoFileReader0\"]"));
		mp_fakeInput  = m_factoryModules.Create("VideoFileReader", *mp_fakeConfig);
		mp_fakeInput->SetContext(*mp_context);
		// note: we need a fake module to create the input streams
		mp_fakeInput->Reset();
	}
	void tearDown()
	{
		CLEAN_DELETE(mp_fakeConfig);
		CLEAN_DELETE(mp_fakeInput);
		CLEAN_DELETE(mp_configFile);
		CLEAN_DELETE(mp_context);
		CLEAN_DELETE(mp_contextParams);
	}

	ConfigReader addModuleToConfig(const string& rx_type, ConfigReader& xr_config)
	{
		ConfigReader moduleConfig =  xr_config.RefSubConfig("application", true)
									 .RefSubConfig("module", "name", rx_type + "0", true);
		ConfigReader paramConfig  = moduleConfig.RefSubConfig("parameters", true);

		paramConfig.RefSubConfig("param" , "name", "class", true).SetValue(rx_type);
		// paramConfig.RefSubConfig("param" , "auto_process" , true).SetValue("0");
		paramConfig.RefSubConfig("param" , "name", "fps"  , true).SetValue("123");

		moduleConfig.RefSubConfig("inputs", true);
		moduleConfig.RefSubConfig("outputs", true);
		moduleConfig.SetAttribute("name", rx_type + "0");

		stringstream ss;
		ss<<m_cpt++;
		moduleConfig.SetAttribute("id", ss.str());
		return moduleConfig;
	}

	/// Create module and make it ready to process
	std::tuple<ParameterStructure*, Module*> createAndConnectModule(const string& x_type, const map<string, string>* xp_parameters = nullptr)
	{
		TS_TRACE("Create and connect module of class " + x_type);
		ConfigReader moduleConfig = addModuleToConfig(x_type, *mp_configFile);

		// Add parameters to override to the config
		if(xp_parameters != nullptr)
			for(const auto& elem : *xp_parameters)
				moduleConfig.RefSubConfig("parameters").RefSubConfig("param", "name", elem.first, true).SetValue(elem.second);

		mp_configFile->SaveToFile("tests/tmp/tmp.xml");

		ParameterStructure* parameters = nullptr;
		try
		{
			parameters = m_factoryParameters.Create(x_type, moduleConfig);
		}
		catch(ParameterException& e)
		{
			if(parameters != nullptr)
				delete(parameters);
			TS_TRACE("Cannot set parameter in createAndConnectModule, reason: " + string(e.what()));
			return std::make_pair(nullptr, nullptr);
		}
		Module* module                 = m_factoryModules.Create(x_type, *parameters);
		module->SetContext(*mp_context);
		m_image = cv::Mat(module->GetHeight(), module->GetWidth(), module->GetImageType());

		// Create custom streams to feed each input of the module
		for(const auto& elem : module->GetInputStreamList())
		{
			Stream& inputStream = module->RefInputStreamById(elem.first);
			Stream* outputStream = nullptr;

			if(elem.second->GetClass() == "StreamImage")
				outputStream = new StreamImage("test", m_image, *mp_fakeInput, "Test input");
			else if(elem.second->GetClass() == "StreamObjects")
				outputStream = new StreamObject("test", m_objects, *mp_fakeInput, "Test input", elem.second->GetRequirement());
			else if(elem.second->GetClass() == "StreamState")
				outputStream = new StreamState("test", m_state, *mp_fakeInput, "Test input");
			else if(elem.second->GetClass() == "StreamEvent")
				outputStream = new StreamEvent("test", m_event, *mp_fakeInput, "Test input");
			else if(elem.second->GetClass() == "StreamNum<float>")
				outputStream = new StreamNum<float>("test", m_float, *mp_fakeInput, "Test input");
			else if(elem.second->GetClass() == "StreamNum<double>")
				outputStream = new StreamNum<double>("test", m_double, *mp_fakeInput, "Test input");
			else if(elem.second->GetClass() == "StreamNum<int>")
				outputStream = new StreamNum<int>("test", m_int, *mp_fakeInput, "Test input");
			// else if(elem.second->GetClass() == "StreamNum<unsigned int>")
				// outputStream = new StreamNum<uint>("test", m_uint, *mp_fakeInput, "Test input");
			else if(elem.second->GetClass() == "StreamNum<bool>")
				outputStream = new StreamNum<bool>("test", m_bool, *mp_fakeInput, "Test input");
			else
			{
				TSM_ASSERT("Unknown input stream type", false);
			}
			inputStream.Connect(outputStream);
			TS_ASSERT(outputStream != nullptr);
			TS_ASSERT(inputStream.IsConnected());
		}
		if(module->IsUnitTestingEnabled())
			module->Reset();

		return std::make_tuple(parameters,module);
	}

	/// Run the modules with different inputs generated randomly
	void testInputs()
	{
		// TS_TRACE("\n# Test different inputs");
		unsigned int seed = 324234566;

		// Test on each type of module
		for(const auto& elem : m_moduleTypes)
		{
			TS_TRACE("## on module " + elem);
			Module* module;
			ParameterStructure* parameters;
			std::tie(parameters, module) = createAndConnectModule(elem);
			if(module->IsUnitTestingEnabled())
			{
				for(int i = 0 ; i < 50 ; i++)
					module->ProcessRandomInput(seed);
			}
			else TS_TRACE("--> unit testing disabled on " + elem);
			delete module;
			delete parameters;
		}
	}


	/// Call each controller of each module
	void testControllers()
	{
		unsigned int seed = 324234566;
		// TS_TRACE("\n# Test all controllers");

		// Test on each type of module
		for(const auto& modType : m_moduleTypes)
		{
			Timer timer;
			timer.Start();
			try
			{
				TS_TRACE("# on module " + modType);
				Module* module;
				ParameterStructure* parameters;
				tie(parameters, module) = createAndConnectModule(modType);
				if(!module->IsUnitTestingEnabled())
				{
					TS_TRACE("--> unit testing disabled on " + modType);
					delete module;
					delete parameters;
					continue;
				}

				// Test on all controllers of the module
				for(const auto& elemCtr : module->GetControllersList())
				{
					TS_TRACE("## on controller " + elemCtr.first + " of class " + elemCtr.second->GetClass());
					vector<string> actions;
					elemCtr.second->ListActions(actions);

					if(elemCtr.second->GetClass() == "ControllerParameter")
					{
						// note: this parameter cannot be tested as will always create an error
						// TODO: Fix this cleanly
						if(elemCtr.second->GetName() == "prepend_output_directory")
							continue;

						// Test specific for controllers of type parameter
						string type, range, defval, newValue;
						assert(actions.size() == 5); // If not you need to write one more test

						type = "0";
						elemCtr.second->CallAction("GetType", &type);
						TS_TRACE("###  " + elemCtr.first + ".GetType returned " + type);

						range = "0";
						elemCtr.second->CallAction("GetRange", &range);
						TS_TRACE("###  " + elemCtr.first + ".GetRange returned " + range);

						defval = "0";
						elemCtr.second->CallAction("GetDefault", &defval);
						TS_TRACE("###  " + elemCtr.first + ".GetDefault returned " + defval);

						vector<string> values;
						TS_TRACE("Generate values for param of type " + type + " in range " + range);
						module->GetParameters().GetParameterByName(elemCtr.first).GenerateValues(10, values, range);

						for(auto& elemVal : values)
						{
							// For string type we cannot set random values
							// cout<<"set "<<value<<endl;
							elemCtr.second->CallAction("Set", &elemVal);

							// Test if the config is globally still valid
							try
							{
								module->CheckParameterRange();
							}
							catch(ParameterException& e)
							{
								TS_TRACE("Cannot set parameter, reason: " + string(e.what()));
								continue;
							}

							newValue = "0";
							elemCtr.second->CallAction("Get", &newValue);

							TSM_ASSERT("Value set must be returned by get: " + elemVal + "!=" + newValue, elemVal == newValue);

							module->Reset();
							for(int i = 0 ; i < 3 ; i++)
								module->ProcessRandomInput(seed);
							TS_TRACE("###  " + elemCtr.first + ".Set returned " + elemVal);
							TS_TRACE("###  " + elemCtr.first + ".Get returned " + newValue);
						}
						elemCtr.second->CallAction("Set", &defval);
					}
					else
					{
						for(const auto& elemAction : actions)
						{
							string value = "0";
							elemCtr.second->CallAction(elemAction, &value);
							TS_TRACE("###  " + elemCtr.first + "." + elemAction + " returned " + value);

							for(int i = 0 ; i < 3 ; i++)
								module->ProcessRandomInput(seed);
						}
					}
				}

				delete module;
				delete parameters;
			}
			catch(ParameterException &e)
			{
				TS_TRACE("Parameter exception caught: " + std::string(e.what()));
			}
			timer.Stop();
			if(timer.GetSecDouble() > 10)
				TS_WARN("Module " + modType + " took " + std::to_string(timer.GetSecDouble()) + "s");
		}
	}

	/// All parameters that were locked need to be tested
	/// 	we generate a new module for each parameter
	void testParameters()
	{
		unsigned int seed = 324223426;
		// TS_TRACE("\n# Test all parameters");

		// Test on each type of module
		for(const auto& modType : m_moduleTypes)
		{
			Timer timer;
			timer.Start();
			Module* module;
			ParameterStructure* parameters;
			std::tie(parameters, module) = createAndConnectModule(modType);
			TS_TRACE("# on module " + modType);

			string lastParam = "";
			string lastDefault = "";

			// Test on all controllers of the module
			for(const auto& elem : module->GetParameters().GetList())
			{
				// Create a second module with the parameter value (in case it is locked)
				// we already have tested the other parameters with controllers
				if(!elem->IsLocked())
					continue;

				TS_TRACE("## on parameter " + elem->GetName() + " of type " + elem->GetTypeString() + " on range " + elem->GetRange());

				// Generate a new module with each value for locked parameter
				vector<string> values;

				TS_TRACE("Generate values for param of type " + elem->GetTypeString() + " in range " + elem->GetRange());
				elem->GenerateValues(10, values);
			
				for(const auto& elemVal : values)
				{
					// For each value
					map<string, string> params;
					TS_TRACE("Set param " + elem->GetName() + " = " + elemVal + " and " + lastParam + " = " + lastDefault);
					params[elem->GetName()] = elemVal;
					if(lastParam != "")
						params[lastParam] = lastDefault;

					Module* module2;
					ParameterStructure* parameters2;
					std::tie(parameters2, module2) = createAndConnectModule(modType, &params);
					if(module2 == nullptr || parameters2 == nullptr || !module2->IsUnitTestingEnabled())
					{
						CLEAN_DELETE(module2);
						CLEAN_DELETE(parameters2);
						continue;
					}
					TS_ASSERT(module2->IsUnitTestingEnabled());

					for(int i = 0 ; i < 3 ; i++)
						module2->ProcessRandomInput(seed);

					delete module2;
					delete parameters2;
				}
				lastParam = elem->GetName();
				lastDefault = elem->GetDefaultString();
			}
			delete module;
			delete parameters;
			timer.Stop();
			if(timer.GetSecDouble() > 10)
				TS_WARN("Module " + modType + " took " + std::to_string(timer.GetSecDouble()) + "s");
		}
	}


	// Test by searching the XML files that were created specially to unit test one modules (ModuleX.test.xml)
	void testBySpecificXmlProjects()
	{
		vector<string> result1;
		execute("xargs -a modules.txt -I{} find {} -name \"testing*.xml\"", result1);

		for(auto elem : result1)
		{
			vector<string> result2;
			// For each xml, execute the file with Markus executable
			TS_TRACE("Testing XML project " + elem);
			string outDir = "out/test_" + basename(elem) + "_" + timeStamp();
			string cmd = "./markus -ncf " + elem + " -o " + outDir + " > /dev/null";
			TS_TRACE("Execute " + cmd);
			SYSTEM(cmd);
			// gather possible errors and warnings
			execute("cat " + outDir + "/markus.log | grep WARN", result2);
			if(!result2.empty())
				TS_WARN("Warning(s) found in " + outDir + "/markus.log");
			execute("cat " + outDir + "/markus.log | grep ERROR", result2);
			if(!result2.empty())
				TS_FAIL("Error(s) found in " + outDir + "/markus.log");
		}

	}

	/// Test export
	void testExport(const Module& xr_module)
	{
		string fileName = "tests/tmp/" + xr_module.GetName() + ".xml";
		ofstream of(fileName.c_str());
		xr_module.Export(of, 0);
		of.close();
		TS_ASSERT(compareFiles(fileName, "tests/modules/" + xr_module.GetName() + ".xml"));
	}

	/// Test export
	void testExport()
	{
		std::vector<std::string> moduleTypes = {"VideoFileReader", "SlitCam", "BgrSubMOG2", "RenderObjects"};

		for(auto& modType : moduleTypes)
		{
			TS_TRACE("# on module " + modType);
			Module* module;
			ParameterStructure* parameters;
			std::tie(parameters, module) = createAndConnectModule(modType);
			testExport(*module);
			delete module;
			delete parameters;
		}
	}
};
#endif
