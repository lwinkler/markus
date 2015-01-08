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
#include "MkException.h"
#include "FeatureFloatInTime.h"
#include "FeatureVector.h"

using namespace std;

// #define BLACKLIST(x) m_moduleTypes.erase(remove(m_moduleTypes.begin(), m_moduleTypes.end(), (x)), m_moduleTypes.end());

#include <string>
#include <iostream>
#include <stdio.h>

void exec(const string& x_cmd, vector<string>& xr_result)
{
	FILE* pipe = popen(x_cmd.c_str(), "r");
	xr_result.clear();
	if (!pipe)
		throw MkException("Error at execution of command: " + x_cmd, LOC);
	char buffer[128];
	while(!feof(pipe))
	{
		if(fgets(buffer, 128, pipe) != NULL)
			xr_result.push_back(buffer);
	}
	pclose(pipe);
}

/// Unit testing class for ConfigReader class
class ModulesTestSuite : public CxxTest::TestSuite
{
	public:
		ModulesTestSuite()
		: mp_fakeInput(NULL),
		  mp_config(NULL),
		  mp_context(NULL),
		  m_state(false),
		  m_factoryModules(Factories::modulesFactory()),
		  m_factoryFeatures(Factories::featuresFactory()),
		  m_cpt(0){}
	protected:
		vector<string> m_moduleTypes;
		const FactoryModules&  m_factoryModules;
		const FactoryFeatures& m_factoryFeatures;
		Module* mp_fakeInput;
		ConfigReader* mp_config;
		Context* mp_context;

		// Objects for streams
		cv::Mat m_image;
		bool m_state;
		Event m_event;
		vector<Object> m_objects;
		int m_cpt;

	public:
	void setUp()
	{
		m_cpt = 0;
		char* tmp = getenv("MODULE_TO_TEST");
		if(tmp != NULL)
		{
			m_moduleTypes.push_back(tmp);
			TS_WARN("$MODULE_TO_TEST should only be used for development purposes.");
		}
		else
			m_factoryModules.List(m_moduleTypes);

		mp_context = new Context("", "TestModule", "testing/out");
		createEmptyConfigFile("/tmp/config_empty.xml");
		mp_config = new ConfigReader("/tmp/config_empty.xml");
		addModuleToConfig("VideoFileReader", *mp_config)
			.RefSubConfig("parameters", true)
			.RefSubConfig("param", "name", "fps", true).SetValue("22");
		mp_config->RefSubConfig("application").SetAttribute("name", "unitTest");
		mp_fakeInput = m_factoryModules.Create("VideoFileReader", mp_config->Find("application>module[name=\"VideoFileReader0\"]"));
		mp_fakeInput->AllowAutoProcess(false);
		// note: we need a fake module to create the input streams
		mp_fakeInput->SetAsReady();
		mp_fakeInput->Reset();
	}
	void tearDown()
	{
		CLEAN_DELETE(mp_fakeInput);
		CLEAN_DELETE(mp_config);
		CLEAN_DELETE(mp_context);
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
	Module* createAndConnectModule(const string& x_type, const map<string, string>* xp_parameters = NULL)
	{
		TS_TRACE("Create and connect module of class " + x_type);
		ConfigReader moduleConfig = addModuleToConfig(x_type, *mp_config);

		// Add parameters to override to the config
		if(xp_parameters != NULL)
			for(map<string, string>::const_iterator it = xp_parameters->begin() ; it != xp_parameters->end() ; ++it)
				moduleConfig.RefSubConfig("parameters").RefSubConfig("param", "name", it->first, true).SetValue(it->second);

		mp_config->SaveToFile("testing/tmp/tmp.xml");
		Module* module = m_factoryModules.Create(x_type, moduleConfig);
		module->SetContext(*mp_context);
		module->AllowAutoProcess(false);
		m_image = cv::Mat(module->GetHeight(), module->GetWidth(), module->GetImageType());

		const map<int, Stream*> & inputs(module->GetInputStreamList());

		// delete(mp_fakeInput);
		// mp_fakeInput = m_factoryModules.CreateModule("VideoFileReader", mp_config->GetSubConfig("application").GetSubConfig("module", "VideoFileReader0"));
		// mp_fakeInput->SetAsReady();
		// mp_fakeInput->Reset();
		
		// Create custom streams to feed each input of the module
		for(map<int, Stream*>::const_iterator it2 = inputs.begin() ; it2 != inputs.end() ; ++it2)
		{
			Stream& inputStream = module->RefInputStreamById(it2->first);
			Stream* outputStream = NULL;

			if(it2->second->GetClass() == "StreamImage")
				outputStream = new StreamImage("test", m_image, *mp_fakeInput, "Test input");
			else if(it2->second->GetClass() == "StreamObjects")
				outputStream = new StreamObject("test", m_objects, *mp_fakeInput, "Test input", it2->second->GetRequirement());
			else if(it2->second->GetClass() == "StreamState")
				outputStream = new StreamState("test", m_state, *mp_fakeInput, "Test input");
			else if(it2->second->GetClass() == "StreamEvent")
				outputStream = new StreamEvent("test", m_event, *mp_fakeInput, "Test input");
			else
			{
				TSM_ASSERT("Unknown input stream type", false);
			}
			inputStream.Connect(outputStream);
			TS_ASSERT(outputStream != NULL);
			TS_ASSERT(inputStream.IsConnected());
		}
		module->SetAsReady();
		if(module->IsUnitTestingEnabled())
			module->Reset();
		return module;
	}

	/// Run the modules with different inputs generated randomly
	void testInputs()
	{
		// TS_TRACE("\n# Test different inputs");
		unsigned int seed = 324234566;

		// Test on each type of module
		for(vector<string>::const_iterator it1 = m_moduleTypes.begin() ; it1 != m_moduleTypes.end() ; ++it1)
		{
			TS_TRACE("## on module " + *it1);
			Module* module = createAndConnectModule(*it1);
			if(module->IsUnitTestingEnabled())
			{
				for(int i = 0 ; i < 50 ; i++)
					module->ProcessRandomInput(seed);
			}
			else TS_TRACE("--> unit testing disabled on " + *it1);
			delete module;
		}
	}


	/// Call each controller of each module
	void testControllers()
	{
		unsigned int seed = 324234566;
		// TS_TRACE("\n# Test all controllers");
		
		// Test on each type of module
		for(vector<string>::const_iterator it1 = m_moduleTypes.begin() ; it1 != m_moduleTypes.end() ; ++it1)
		{
			TS_TRACE("# on module " + *it1);
			Module* module = createAndConnectModule(*it1);
			if(!module->IsUnitTestingEnabled())
			{
				TS_TRACE("--> unit testing disabled on " + *it1);
				delete module;
				continue;
			}

			// Test on all controllers of the module
			for(map<string, Controller*>::const_iterator it2 = module->GetControllersList().begin() ; it2 != module->GetControllersList().end() ; ++it2)
			{
				TS_TRACE("## on controller " + it2->first + " of class " + it2->second->GetClass());
				vector<string> actions;
				it2->second->ListActions(actions);

				if(it2->second->GetClass() == "ControllerParameter")
				{
					// Test specific for controllers of type parameter
					string type, range, defval, newValue;
					assert(actions.size() == 5); // If not you need to write one more test

					type = "0";
					it2->second->CallAction("GetType", &type);
					TS_TRACE("###  " + it2->first + ".GetType returned " + type);

					range = "0";
					it2->second->CallAction("GetRange", &range);
					TS_TRACE("###  " + it2->first + ".GetRange returned " + range);

					defval = "0";
					it2->second->CallAction("GetDefault", &defval);
					TS_TRACE("###  " + it2->first + ".GetDefault returned " + defval);

					vector<string> values;
					TS_TRACE("Generate values for param of type " + type + " in range " + range);
					module->GetParameters().GetParameterByName(it2->first).GenerateValues(20, values, range);
  
					for(vector<string>::iterator it = values.begin() ; it != values.end() ; ++it)
					{
						// For string type we cannot set random values
						// cout<<"set "<<value<<endl;
						it2->second->CallAction("Set", &(*it));

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
						it2->second->CallAction("Get", &newValue);

						TSM_ASSERT("Value set must be returned by get", *it == newValue);

						module->Reset();
						for(int i = 0 ; i < 3 ; i++)
							module->ProcessRandomInput(seed);
						TS_TRACE("###  " + it2->first + ".Set returned " + *it);
						TS_TRACE("###  " + it2->first + ".Get returned " + newValue);
					}
					it2->second->CallAction("Set", &defval);
				}
				else
				{
					for(vector<string>::const_iterator it3 = actions.begin() ; it3 != actions.end() ; ++it3)
					{
						string value = "0";
						// module->LockForWrite();
						it2->second->CallAction(*it3, &value);
						TS_TRACE("###  " + it2->first + "." + *it3 + " returned " + value);
						// module->Unlock();

						for(int i = 0 ; i < 3 ; i++)
							module->ProcessRandomInput(seed);
					}
				}
			}

			delete module;
		}
	}

	/// All parameters that were locked need to be tested
	/// 	we generate a new module for each parameter
	void testParameters()
	{
		unsigned int seed = 324223426;
		// TS_TRACE("\n# Test all parameters");
		
		// Test on each type of module
		for(vector<string>::const_iterator it1 = m_moduleTypes.begin() ; it1 != m_moduleTypes.end() ; ++it1)
		{
			Module* module = createAndConnectModule(*it1);
			if(!module->IsUnitTestingEnabled())
			{
				delete module;
				continue;
			}
			TS_TRACE("# on module " + *it1);

			string lastParam = "";
			string lastDefault = "";

			// Test on all controllers of the module
			for(vector<Parameter*>::const_iterator it2 = module->GetParameters().GetList().begin() ; it2 != module->GetParameters().GetList().end() ; ++it2)
			{
				// Create a second module with the parameter value (in case it is locked)
				// we already have tested the other parameters with controllers
				if(!(*it2)->IsLocked())
					continue;

				TS_TRACE("## on parameter " + (*it2)->GetName() + " of type " + (*it2)->GetTypeString() + " on range " + (*it2)->GetRange());

				// Generate a new module with each value for locked parameter
				vector<string> values;

				TS_TRACE("Generate values for param of type " + (*it2)->GetTypeString() + " in range " + (*it2)->GetRange());
				(*it2)->GenerateValues(10, values);

				for(vector<string>::iterator it3 = values.begin() ; it3 != values.end() ; ++it3)
				{
					// For each value
					map<string, string> params;
					TS_TRACE("Set param " + (*it2)->GetName() + " = " + *it3 + " and " + lastParam + " = " + lastDefault);
					params[(*it2)->GetName()] = *it3;
					if(lastParam != "")
						params[lastParam] = lastDefault;

					Module* module2 = createAndConnectModule(*it1, &params);
					TS_ASSERT(module2->IsUnitTestingEnabled());

					for(int i = 0 ; i < 3 ; i++)
						module2->ProcessRandomInput(seed);

					delete module2;
				}
				lastParam = (*it2)->GetName();
				lastDefault = (*it2)->GetDefaultString();
			}
			delete module;
		}
	}


	// Test by searching the XML files that were created specially to unit test one modules (ModuleX.test.xml)
	void testBySpecificXmlProjects()
	{
		vector<string> result;
		exec("find modules/ modules2/ -name \"testing*.xml\"", result);

		for(auto elem : result)
		{
			// For each xml, execute the file with Markus executable
			TS_TRACE("Testing XML project " + elem);
			SYSTEM("./markus -ncf " + elem);
		}
		
	}
};
#endif
