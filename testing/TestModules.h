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


/// Unit testing class for ConfigReader class

class TestModules : public CppUnit::TestFixture
{
	private:
		static log4cxx::LoggerPtr m_logger;
	protected:
		std::vector<std::string> moduleTypes;
		FactoryModules m_factory;
		Module* mp_fakeInput;
		ConfigReader* mp_config;

		// Objects for streams
		cv::Mat m_image; // (module->GetHeight(), module->GetWidth(), module->GetType());
		bool m_state;
		Event m_event;
		std::vector<Object> m_objects;
	public:
	void runTest()
	{
	}
	void setUp()
	{
		m_factory.ListModules(moduleTypes);
		createEmtpyConfigFile("/tmp/config_empty.xml");
		mp_config = new ConfigReader("/tmp/config_empty.xml");
		addModuleToConfig("VideoFileReader", *mp_config)
			.RefSubConfig("parameters", "", true)
			.RefSubConfig("param", "fps", true).SetValue("22");
		mp_fakeInput = m_factory.CreateModule("VideoFileReader", mp_config->GetSubConfig("application").GetSubConfig("module", "VideoFileReader0"));
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
		moduleConfig.RefSubConfig("parameters", "", true)
			.RefSubConfig("param", "class", true)
			.SetValue(rx_type);
		moduleConfig.RefSubConfig("parameters", "", true)
			.RefSubConfig("param", "auto_process", true)
			.SetValue("1");
		moduleConfig.RefSubConfig("parameters", "", true)
			.RefSubConfig("param", "fps", true)
			.SetValue("123");
		moduleConfig.RefSubConfig("inputs", "", true);
		moduleConfig.RefSubConfig("outputs", "", true);
		moduleConfig.SetAttribute("name", rx_type + "0");
		return moduleConfig;
	}


	/// Create random object
	Object createRandomObject(unsigned int* xp_seed, const std::string& x_featureNames)
	{
		// std::cout<<m_image.size()<<std::endl;
		assert(m_image.size() != cv::Size(0,0));
		Object obj("test", cv::Rect(
			cv::Point(rand_r(xp_seed) % m_image.cols, rand_r(xp_seed) % m_image.rows), 
			cv::Point(rand_r(xp_seed) % m_image.cols, rand_r(xp_seed) % m_image.rows))
		);

		if(x_featureNames != "")
		{
			// If a list of features is specified
			std::vector<std::string> feats;
			split(x_featureNames, ',', feats);
			for(std::vector<std::string>::const_iterator it = feats.begin() ; it != feats.end() ; it++)
			{
				obj.AddFeature(*it, static_cast<float>(rand_r(xp_seed)) / RAND_MAX);
			}
		}

		int nb = rand_r(xp_seed) % 100;
		for(int i = 0 ; i < nb + 1 ; i++) // TODO notify event does not accept empty features, remove +1
		{
			std::stringstream name;
			name<<"rand"<<i;
			obj.AddFeature(name.str(), static_cast<float>(rand_r(xp_seed)) / RAND_MAX);
		}
		return obj;
	}

	/// Randomize input values
	void randomizeInputs(const Module* x_module, const std::string& x_moduleClass, unsigned int* xp_seed)
	{
		assert(x_module->GetClass() == x_moduleClass);
		std::string features = "";

		// Some modules require a specific set of features in input
		if(x_moduleClass == "FallDetection")
			features = "x,y,ellipse_angle,ellipse_ratio";
		else if(x_moduleClass == "FilterObjects")
			features = "x,y,width,height";
		else if(x_moduleClass == "FilterPython")
		{
			Controller* ctr = x_module->FindController("features");
			assert(ctr != NULL);
			ctr->CallAction("Get", &features);
		}
		else if(x_moduleClass == "Intrusion")
			features = "x,y";
		else if(x_moduleClass == "TrackerByFeatures")
			x_module->FindController("features")->CallAction("Get", &features);

		// random event
		m_event.Empty();
		if(rand_r(xp_seed) < RAND_MAX /10)
		{
			if(rand_r(xp_seed) < RAND_MAX /10)
			{
				m_event.Raise("random");
			}
			else
			{
				m_event.Raise("random", createRandomObject(xp_seed, features));
			}
		}

		// random objects
		m_objects.clear();
		int nb = rand_r(xp_seed) % 10;
		for(int i = 0 ; i < nb ; i++)
		{
			m_objects.push_back(createRandomObject(xp_seed, features));
		}


		// random state
		if(rand_r(xp_seed) < RAND_MAX /10)
			m_state = !m_state;


		// random image
		m_image.setTo(0);
		nb = rand_r(xp_seed) % 100;
		for ( int i = 0; i < nb; i++ )
		{
			cv::Point center;
			center.x = rand_r(xp_seed) % m_image.cols;
			center.y = rand_r(xp_seed) % m_image.rows;

			cv::Size axes;
			axes.width  = rand_r(xp_seed) % 200;
			axes.height = rand_r(xp_seed) % 200;

			double angle = rand_r(xp_seed) % 180;
			cv::Scalar randomColor(rand_r(xp_seed) % 255, rand_r(xp_seed) % 255, rand_r(xp_seed) % 255);

			ellipse(m_image, center, axes, angle, angle - 100, angle + 200,
					randomColor, (rand_r(xp_seed) % 10) - 1);
		}
	}

	/// Create module and make it ready to process
	Module* createAndConnectModule(const std::string& x_type)
	{
		ConfigReader moduleConfig = addModuleToConfig(x_type, *mp_config);
		mp_config->SaveToFile("testing/tmp.xml");
		Module* module = m_factory.CreateModule(x_type, moduleConfig);
		m_image = cv::Mat(module->GetHeight(), module->GetWidth(), module->GetImageType());

		const std::map<int, Stream*> inputs  = module->GetInputStreamList();
		
		// Create custom streams to feed each input of the module
		for(std::map<int, Stream*>::const_iterator it2 = inputs.begin() ; it2 != inputs.end() ; it2++)
		{
			Stream& inputStream = module->RefInputStreamById(it2->first);
			Stream* outputStream = NULL;

			if(it2->second->GetClass() == "StreamImage")
				outputStream = new StreamImage("test", m_image, *mp_fakeInput, "Test input");
			else if(it2->second->GetClass() == "StreamObjects")
				outputStream = new StreamObject("test", m_objects, *mp_fakeInput, "Test input");
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
		// It makes no sense to test inputs if input source is missing
		if(module->IsInput())
		{
			delete module;
			return NULL;
		}
		module->SetAsReady();
		module->Reset();
		return module;
	}

	/// Generate a random string value for the parameter
	void GenerateValueFromRange(std::vector<std::string>& rx_values, const std::string& x_range, const std::string& x_type, unsigned int* xp_seed)
	{
		LOG_DEBUG(m_logger, "Generate values for param of type "<<x_type<<" in range "<<x_range);
		rx_values.clear();
		// if(x_range == "")
			// return "";
		if(x_type == "calibrationByHeight")
			return; // TODO
		if(x_type == "bool")
		{
			rx_values.push_back("0");
			rx_values.push_back("1");
			return;
		}
		CPPUNIT_ASSERT(x_type != "string");
		CPPUNIT_ASSERT(x_range.size() > 0);
		CPPUNIT_ASSERT(x_range.at(0) == '[');
		CPPUNIT_ASSERT(x_range.at(x_range.size() - 1) == ']');

		double min = 0, max = 0;
		if (2 == sscanf(x_range.c_str(), "[%lf:%lf]", &min, &max))
		{
			for(int i = 0 ; i < 100 ; i++)
			{
				std::stringstream ss;
				// Generate 100 random values
				// TODO: used values in range, non-random
				if(x_type == "int")
					ss<<static_cast<int>(min + rand_r(xp_seed) % static_cast<int>(max - min + 1));
				else 
					ss<<(min + rand_r(xp_seed) * (max - min) / RAND_MAX);
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
		for(std::vector<std::string>::const_iterator it1 = moduleTypes.begin() ; it1 != moduleTypes.end() ; it1++)
		{
			LOG_TEST(m_logger, "## on module "<<*it1);

			Module* module = createAndConnectModule(*it1);
			if(module != NULL)
			{
				for(int i = 0 ; i < 50 ; i++)
				{
					randomizeInputs(module, *it1, &seed);
					module->Process();
				}
				delete module;
			}
		}
	}


	/// Call each controller of each module
	void testControllers()
	{
		unsigned int seed = 324234566;
		LOG_TEST(m_logger, "\n# Test all controllers");
		
		// Test on each type of module
		for(std::vector<std::string>::const_iterator it1 = moduleTypes.begin() ; it1 != moduleTypes.end() ; it1++)
		{
			LOG_TEST(m_logger, "# on module "<<*it1);

			Module* module = createAndConnectModule(*it1);
			if(module == NULL)
				continue;

			randomizeInputs(module, *it1, &seed);

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
					else GenerateValueFromRange(values, range, type, &seed);

					for(std::vector<std::string>::iterator it = values.begin() ; it != values.end() ; it++)
					{
						// For string type we cannot set random values
						// std::cout<<"set "<<value<<std::endl;
						it2->second->CallAction("Set", &(*it));

						newValue = "0";
						it2->second->CallAction("Get", &newValue);

						CPPUNIT_ASSERT_MESSAGE("Value set must be returned by get", *it == newValue);

						for(int i = 0 ; i < 3 ; i++)
							module->Process();
						LOG_DEBUG(m_logger, "###  "<<it2->first<<".Set returned "<<*it);
						LOG_DEBUG(m_logger, "###  "<<it2->first<<".Get returned "<<newValue);
					}
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

						for(int i = 0 ; i < 3 ; i++) module->Process();
					}
				}
			}

			delete module;
		}
		// TODO test locked parameters too
	}

	static CppUnit::Test *suite()
	{
		CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite("TestModules");
		suiteOfTests->addTest(new CppUnit::TestCaller<TestModules>("testInputs", &TestModules::testInputs));
		suiteOfTests->addTest(new CppUnit::TestCaller<TestModules>("testControllers", &TestModules::testControllers));
		return suiteOfTests;
	}
};
log4cxx::LoggerPtr TestModules::m_logger(log4cxx::Logger::getLogger("TestModules"));
#endif
