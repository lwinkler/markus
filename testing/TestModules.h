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

#include <cppunit/TestFixture.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include "Module.h"
#include "Controller.h"
#include "util.h"
#include "StreamEvent.h"
#include "StreamObject.h"
#include "StreamImage.h"
#include "StreamState.h"
#include "MkException.h"
#include "FeatureFloatInTime.h"
#include "FeatureVectorFloat.h"

// #define BLACKLIST(x) m_moduleTypes.erase(std::remove(m_moduleTypes.begin(), m_moduleTypes.end(), (x)), m_moduleTypes.end());

/// Unit testing class for ConfigReader class

class TestModules : public CppUnit::TestFixture
{
	private:
		static log4cxx::LoggerPtr m_logger;
	protected:
		std::vector<std::string> m_moduleTypes;
		FactoryModules  m_factoryModules;
		FactoryFeatures m_factoryFeatures;
		Module* mp_fakeInput;
		ConfigReader* mp_config;

		// Objects for streams
		cv::Mat m_image; // (module->GetHeight(), module->GetWidth(), module->GetType());
		bool m_state;
		Event m_event;
		std::vector<Object> m_objects;
		int m_cpt;
	public:
	void runTest()
	{
	}
	void setUp()
	{
		m_cpt = 0;
		m_factoryModules.ListModules(m_moduleTypes);

		createEmptyConfigFile("/tmp/config_empty.xml");
		mp_config = new ConfigReader("/tmp/config_empty.xml");
		addModuleToConfig("VideoFileReader", *mp_config)
			.RefSubConfig("parameters", "", true)
			.RefSubConfig("param", "fps", true).SetValue("22");
		mp_config->RefSubConfig("application").SetAttribute("name", "unitTest");
		mp_fakeInput = m_factoryModules.CreateModule("VideoFileReader", mp_config->GetSubConfig("application").GetSubConfig("module", "VideoFileReader0"));
		mp_fakeInput->AllowAutoProcess(false);
		// note: we need a fake module to create the input streams
		mp_fakeInput->SetAsReady();
		mp_fakeInput->Reset();
	}
	void tearDown()
	{
		delete mp_fakeInput;
		delete mp_config;
	}

	ConfigReader addModuleToConfig(const std::string& rx_type, ConfigReader& xr_config)
	{
		ConfigReader moduleConfig =  xr_config.RefSubConfig("application", "", true)
			.RefSubConfig("module", rx_type + "0", true);
		ConfigReader paramConfig  = moduleConfig.RefSubConfig("parameters", "", true);

		paramConfig.RefSubConfig("param" , "class"        , true).SetValue(rx_type);
		// paramConfig.RefSubConfig("param" , "auto_process" , true).SetValue("0");
		paramConfig.RefSubConfig("param" , "fps"          , true).SetValue("123");

		moduleConfig.RefSubConfig("inputs", "", true);
		moduleConfig.RefSubConfig("outputs", "", true);
		moduleConfig.SetAttribute("name", rx_type + "0");

		std::stringstream ss;
		ss<<m_cpt++;
		moduleConfig.SetAttribute("id", ss.str());
		return moduleConfig;
	}

	/// Create module and make it ready to process
	Module* createAndConnectModule(const std::string& x_type, const std::map<std::string, std::string>* xp_parameters = NULL)
	{
		LOG_DEBUG(m_logger, "Create and connect module of class "<<x_type);
		ConfigReader moduleConfig = addModuleToConfig(x_type, *mp_config);

		// Add parameters to override to the config
		if(xp_parameters != NULL)
			for(std::map<std::string, std::string>::const_iterator it = xp_parameters->begin() ; it != xp_parameters->end() ; it++)
				moduleConfig.RefSubConfig("parameters").RefSubConfig("param", it->first, true).SetValue(it->second);

		mp_config->SaveToFile("testing/tmp/tmp.xml");
		Module* module = m_factoryModules.CreateModule(x_type, moduleConfig);
		module->AllowAutoProcess(false);
		m_image = cv::Mat(module->GetHeight(), module->GetWidth(), module->GetImageType());

		const std::map<int, Stream*> inputs  = module->GetInputStreamList();

		// delete(mp_fakeInput);
		// mp_fakeInput = m_factoryModules.CreateModule("VideoFileReader", mp_config->GetSubConfig("application").GetSubConfig("module", "VideoFileReader0"));
		// mp_fakeInput->SetAsReady();
		// mp_fakeInput->Reset();
		
		// Create custom streams to feed each input of the module
		for(std::map<int, Stream*>::const_iterator it2 = inputs.begin() ; it2 != inputs.end() ; it2++)
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
				CPPUNIT_ASSERT_MESSAGE("Unknown input stream type", false);
			}
			inputStream.Connect(outputStream);
			CPPUNIT_ASSERT(outputStream != NULL);
			CPPUNIT_ASSERT(inputStream.IsConnected());
		}
		module->SetAsReady();
		if(module->IsUnitTestingEnabled())
			module->Reset();
		return module;
	}

	/// Generate a random string value for the parameter
	void GenerateValueFromRange(int x_nbSamples, std::vector<std::string>& rx_values, const std::string& x_range, const std::string& x_type)
	{
		LOG_DEBUG(m_logger, "Generate values for param of type "<<x_type<<" in range "<<x_range);
		rx_values.clear();
		// if(x_range == "")
			// return "";
		if(x_type == "calibrationByHeight")
			return; // TODO

		CPPUNIT_ASSERT(x_type != "string");
		CPPUNIT_ASSERT(x_range.size() > 0);
		CPPUNIT_ASSERT(x_range.at(0) == '[');
		CPPUNIT_ASSERT(x_range.at(x_range.size() - 1) == ']');

		double min = 0, max = 0;
		if (2 == sscanf(x_range.c_str(), "[%lf:%lf]", &min, &max))
		{
			if(x_type == "int" && max - min <= x_nbSamples)
			{
				for(int i = min ; i <= max ; i++)
				{
					std::stringstream ss;
					ss<<i;
					rx_values.push_back(ss.str());
					// rx_values.push_back(static_cast<int>(min + static_cast<int>(i/x_nbSamples) % static_cast<int>(max - min + 1)));
				}
			}
			else if(x_type == "int" || x_type == "bool")
			{
				// x_nbSamples values in range
				double incr = static_cast<double>(max - min) / x_nbSamples;
				for(int i = 0 ; i <= x_nbSamples ; i++)
				{
					std::stringstream ss;
					ss<<static_cast<int>(min + i * incr);
					rx_values.push_back(ss.str());
				}
			}
			else 
			{
				// x_nbSamples values in range
				double incr = static_cast<double>(max - min) / x_nbSamples;
				for(int i = 0 ; i <= x_nbSamples ; i++)
				{
					std::stringstream ss;
					ss<<(min + i * incr);
					rx_values.push_back(ss.str());
				}
			}
		}
		else
		{
			split(x_range.substr(1, x_range.size() - 2), ',', rx_values);
			// Remove last element if empty, due to an extra comma
			CPPUNIT_ASSERT(rx_values.size() > 0);
			if(rx_values.back() == "")
				rx_values.pop_back();
			assert(rx_values.size() > 0);
		}
	}

	/// Run the modules with different inputs generated randomly
	void testInputs()
	{
		LOG_TEST(m_logger, "\n# Test different inputs");
		unsigned int seed = 324234566;

		// Test on each type of module
		for(std::vector<std::string>::const_iterator it1 = m_moduleTypes.begin() ; it1 != m_moduleTypes.end() ; it1++)
		{
			LOG_TEST(m_logger, "## on module "<<*it1);
			Module* module = createAndConnectModule(*it1);
			if(module->IsUnitTestingEnabled())
			{
				for(int i = 0 ; i < 50 ; i++)
					module->ProcessRandomInput(seed);
			}
			else LOG_TEST(m_logger, "--> unit testing disabled on "<<*it1);
			delete module;
		}
	}


	/// Call each controller of each module
	void testControllers()
	{
		unsigned int seed = 324234566;
		LOG_TEST(m_logger, "\n# Test all controllers");
		
		// Test on each type of module
		for(std::vector<std::string>::const_iterator it1 = m_moduleTypes.begin() ; it1 != m_moduleTypes.end() ; it1++)
		{
			LOG_TEST(m_logger, "# on module "<<*it1);
			Module* module = createAndConnectModule(*it1);
			if(!module->IsUnitTestingEnabled())
			{
				LOG_TEST(m_logger, "--> unit testing disabled on "<<*it1);
				delete module;
				continue;
			}

			// Test on all controllers of the module
			for(std::map<std::string, Controller*>::const_iterator it2 = module->GetControllersList().begin() ; it2 != module->GetControllersList().end() ; it2++)
			{
				LOG_INFO(m_logger, "## on controller "<<it2->first<<" of class "<<it2->second->GetClass());
				std::vector<std::string> actions;
				it2->second->ListActions(actions);

				if(it2->second->GetClass() == "ControllerParameter")
				{
					// Test specific for controllers of type parameter
					std::string type, range, defval, newValue;
					assert(actions.size() == 5); // If not you need to write one more test

					type = "0";
					it2->second->CallAction("GetType", &type);
					LOG_INFO(m_logger, "###  "<<it2->first<<".GetType returned "<<type);

					range = "0";
					it2->second->CallAction("GetRange", &range);
					LOG_INFO(m_logger, "###  "<<it2->first<<".GetRange returned "<<range);

					defval = "0";
					it2->second->CallAction("GetDefault", &defval);
					LOG_INFO(m_logger, "###  "<<it2->first<<".GetDefault returned "<<defval);

					std::vector<std::string> values;
					if(type  == "string")
						values.push_back(defval);
					else GenerateValueFromRange(20, values, range, type);
  
					for(std::vector<std::string>::iterator it = values.begin() ; it != values.end() ; it++)
					{
						// For string type we cannot set random values
						// std::cout<<"set "<<value<<std::endl;
						it2->second->CallAction("Set", &(*it));

						// Test if the config is globally still valid
						try
						{
							module->CheckParameterRange();
						}
						catch(ParameterException& e)
						{
							LOG_INFO(m_logger, "Cannot set parameter, reason: "<<e.what());
							continue;
						}

						newValue = "0";
						it2->second->CallAction("Get", &newValue);

						CPPUNIT_ASSERT_MESSAGE("Value set must be returned by get", *it == newValue);

						module->Reset();
						for(int i = 0 ; i < 3 ; i++)
							module->ProcessRandomInput(seed);
						LOG_DEBUG(m_logger, "###  "<<it2->first<<".Set returned "<<*it);
						LOG_DEBUG(m_logger, "###  "<<it2->first<<".Get returned "<<newValue);
					}
					it2->second->CallAction("Set", &defval);
				}
				else
				{
					for(std::vector<std::string>::const_iterator it3 = actions.begin() ; it3 != actions.end() ; it3++)
					{
						std::string value = "0";
						// module->LockForWrite();
						it2->second->CallAction(*it3, &value);
						LOG_INFO(m_logger, "###  "<<it2->first<<"."<<*it3<<" returned "<<value);
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
		LOG_TEST(m_logger, "\n# Test all parameters");
		
		// Test on each type of module
		for(std::vector<std::string>::const_iterator it1 = m_moduleTypes.begin() ; it1 != m_moduleTypes.end() ; it1++)
		{
			Module* module = createAndConnectModule(*it1);
			if(!module->IsUnitTestingEnabled())
			{
				delete module;
				continue;
			}
			LOG_TEST(m_logger, "# on module "<<*it1);

			std::string lastParam = "";
			std::string lastDefault = "";

			// Test on all controllers of the module
			for(std::vector<Parameter*>::const_iterator it2 = module->GetParameters().GetList().begin() ; it2 != module->GetParameters().GetList().end() ; it2++)
			{
				// Create a second module with the parameter value (in case it is locked)
				// we already have tested the other parameters with controllers
				if(!(*it2)->IsLocked())
					continue;

				LOG_INFO(m_logger, "## on parameter "<<(*it2)->GetName()<<" of type "<<(*it2)->GetTypeString()<<" on range "<<(*it2)->GetRange());

				// Generate a new module with each value for locked parameter
				std::vector<std::string> values;

				if((*it2)->GetTypeString()  == "string")
					continue; // values.push_back(defval);
				else GenerateValueFromRange(10, values, (*it2)->GetRange(), (*it2)->GetTypeString());

				for(std::vector<std::string>::iterator it3 = values.begin() ; it3 != values.end() ; it3++)
				{
					// For each value
					std::map<std::string, std::string> params;
					LOG_DEBUG(m_logger, "Set param "<<(*it2)->GetName()<<" = "<<*it3<<" and "<<lastParam<<" = "<<lastDefault);
					params[(*it2)->GetName()] = *it3;
					params[lastParam] = lastDefault;

					Module* module2 = createAndConnectModule(*it1, &params);
					CPPUNIT_ASSERT(module2->IsUnitTestingEnabled());

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

	static CppUnit::Test *suite()
	{
		CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite("TestModules");
		suiteOfTests->addTest(new CppUnit::TestCaller<TestModules>("testInputs", &TestModules::testInputs));
		suiteOfTests->addTest(new CppUnit::TestCaller<TestModules>("testControllers", &TestModules::testControllers));
		suiteOfTests->addTest(new CppUnit::TestCaller<TestModules>("testParameters", &TestModules::testParameters));
		return suiteOfTests;
	}
};
log4cxx::LoggerPtr TestModules::m_logger(log4cxx::Logger::getLogger("TestModules"));
#endif
