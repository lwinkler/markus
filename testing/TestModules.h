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
#include "util.h"
#include "Manager.h"
#include "StreamEvent.h"
#include "StreamObject.h"
#include "StreamImage.h"
#include "StreamState.h"
#include "MkException.h"

#define LOG_TEST(str) {\
	std::cout<<std::endl<<str<<std::endl;\
	LOG_INFO(Manager::Logger(), str);\
}

/// Unit testing class for ConfigReader class

class TestModules : public CppUnit::TestFixture
{
	protected:
		std::vector<std::string> moduleTypes;
		FactoryModules m_factory;
		Module* m_inputModuleImages;
	public:
	void runTest()
	{
	}
	void setUp()
	{
		m_factory.ListModules(moduleTypes);
	}
	void tearDown()
	{
		delete m_inputModuleImages;
	}

	void addModuleToConfig(const std::string& rx_type, ConfigReader& xr_config)
	{
		ConfigReader moduleConfig =  xr_config.RefSubConfig("application", "")
			.RefSubConfig("module", rx_type + "0", true);
		moduleConfig.RefSubConfig("parameters", "", true)
			.RefSubConfig("param", "class", true)
			.SetValue(rx_type);
		moduleConfig.RefSubConfig("inputs", "", true);
		moduleConfig.RefSubConfig("outputs", "", true);
		moduleConfig.SetAttribute("name", rx_type + "0");
	}

	/// Load and save a config file
	void testInputs()
	{
		LOG_TEST("# Test the creation of modules");
		
		for(std::vector<std::string>::const_iterator it1 = moduleTypes.begin() ; it1 != moduleTypes.end() ; it1++)
		{
			LOG_TEST("## create module "<<*it1);

			ConfigReader config("config_empty.xml");
			addModuleToConfig(*it1, config);
					config.SaveToFile("a.xml");
			Module* module = m_factory.CreateModule(*it1, config.GetSubConfig("application").GetSubConfig("module", *it1 + "0"));
			const std::map<int, Stream*> inputs  = module->GetInputStreamList();
			// std::vector<Module*> modules;

			// Objects for streams
			cv::Mat image(module->GetHeight(), module->GetWidth(), module->GetType());
			bool state;
			Event event;
			std::vector<Object> objects;
			
			// Create custom streams to feed each input of the module
			for(std::map<int, Stream*>::const_iterator it2 = inputs.begin() ; it2 != inputs.end() ; it2++)
			{
				Stream& inputStream = module->RefInputStreamById(it2->first);
				Stream* outputStream = NULL;

				if(it2->second->GetTypeString() == "Image")
					outputStream = new StreamImage("test", image, *module, "Test input");
				else if(it2->second->GetTypeString() == "Objects")
					outputStream = new StreamObject("test", objects, *module, "Test input");
				else if(it2->second->GetTypeString() == "State")
					outputStream = new StreamState("test", state, *module, "Test input");
				else if(it2->second->GetTypeString() == "Event")
					outputStream = new StreamEvent("test", event, *module, "Test input");
				else
				{
					LOG_ERROR(Manager::Logger(), "Unknown input stream type "<<it2->second->GetTypeString());
					CPPUNIT_ASSERT_MESSAGE("Unknown input stream type", false);
				}
				CPPUNIT_ASSERT(outputStream != NULL);

				inputStream.Connect(outputStream);
			}
			module->SetAsReady();
			module->Reset();


			// const std::map<int, Stream*> outputs = module->GetOutputStreamList();
			// const std::map<int, Stream*> debugs  = module->GetDebugStreamList();

			for(int i = 0 ; i < 100 ; i++)
				module->Process();
			delete module;
		}
	}


	static CppUnit::Test *suite()
	{
		CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite("TestModules");
		suiteOfTests->addTest(new CppUnit::TestCaller<TestModules>("testInputs", &TestModules::testInputs));
		return suiteOfTests;
	}
};
#endif
