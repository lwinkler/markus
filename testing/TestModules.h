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
		mp_config = new ConfigReader("config_empty.xml");
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
		delete mp_config;
		delete mp_fakeInput;
	}

	ConfigReader addModuleToConfig(const std::string& rx_type, ConfigReader& xr_config)
	{
		ConfigReader moduleConfig =  xr_config.RefSubConfig("application", "")
			.RefSubConfig("module", rx_type + "0", true);
		moduleConfig.RefSubConfig("parameters", "", true)
			.RefSubConfig("param", "class", true)
			.SetValue(rx_type);
		moduleConfig.RefSubConfig("inputs", "", true);
		moduleConfig.RefSubConfig("outputs", "", true);
		moduleConfig.SetAttribute("name", rx_type + "0");
		return moduleConfig;
	}


	/// Create random object
	Object createRandomObject()
	{
		Object obj("test");
		obj.posX  = rand() % m_image.cols;
		obj.posY  = rand() % m_image.cols;
		obj.width  = rand() % (m_image.cols - (int)obj.posX);
		obj.height = rand() % (m_image.rows - (int)obj.posY);
		int nb = rand() % 100;
		for(int i = 0 ; i < nb ; i++)
		{
			std::stringstream name;
			name<<"feat"<<i;
			obj.AddFeature(name.str(), static_cast<float>(rand()) / RAND_MAX);
		}
		obj.AddFeature("x", static_cast<float>(rand()) / RAND_MAX);
		obj.AddFeature("y", static_cast<float>(rand()) / RAND_MAX);
		obj.AddFeature("width", static_cast<float>(rand()) / RAND_MAX);
		obj.AddFeature("height", static_cast<float>(rand()) / RAND_MAX);
		obj.AddFeature("ellipse_angle", static_cast<float>(rand()) / RAND_MAX);
		obj.AddFeature("ellipse_ratio", static_cast<float>(rand()) / RAND_MAX);
		return obj;
	}

	/// Randomize input values
	void randomizeInputs()
	{
		// random event
		m_event.Empty();
		if(rand() < RAND_MAX /10)
		{
			if(rand() < RAND_MAX /10)
			{
				m_event.Raise("random");
			}
			else
			{
				m_event.Raise("random", createRandomObject());
			}
		}

		// random objects
		m_objects.clear();
		int nb = rand() % 100;
		for(int i = 0 ; i < nb ; i++)
		{
			m_objects.push_back(createRandomObject());
		}


		// random state
		if(rand() < RAND_MAX /10)
			m_state = !m_state;


		// random image
		m_image.setTo(0);
		nb = rand() % 100;
		for ( int i = 0; i < nb; i++ )
		{
			cv::Point center;
			center.x = rand() % m_image.cols;
			center.y = rand() % m_image.rows;

			cv::Size axes;
			axes.width  = rand() % 200;
			axes.height = rand() % 200;

			double angle = rand() % 180;
			cv::Scalar randomColor(rand() % 255, rand() % 255, rand() % 255);

			ellipse(m_image, center, axes, angle, angle - 100, angle + 200,
					randomColor, (rand() % 10) - 1);
		}
	}

	/// Create module and make it ready to process
	Module* createAndConnectModule(const std::string& x_type)
	{
		ConfigReader moduleConfig = addModuleToConfig(x_type, *mp_config);
		mp_config->SaveToFile("testing/tmp.xml");
		Module* module = m_factory.CreateModule(x_type, moduleConfig);
		m_image = cv::Mat(module->GetHeight(), module->GetWidth(), module->GetType());

		const std::map<int, Stream*> inputs  = module->GetInputStreamList();
		
		// Create custom streams to feed each input of the module
		for(std::map<int, Stream*>::const_iterator it2 = inputs.begin() ; it2 != inputs.end() ; it2++)
		{
			Stream& inputStream = module->RefInputStreamById(it2->first);
			Stream* outputStream = NULL;

			if(it2->second->GetTypeString() == "Image")
				outputStream = new StreamImage("test", m_image, *mp_fakeInput, "Test input");
			else if(it2->second->GetTypeString() == "Objects")
				outputStream = new StreamObject("test", m_objects, *mp_fakeInput, "Test input");
			else if(it2->second->GetTypeString() == "State")
				outputStream = new StreamState("test", m_state, *mp_fakeInput, "Test input");
			else if(it2->second->GetTypeString() == "Event")
				outputStream = new StreamEvent("test", m_event, *mp_fakeInput, "Test input");
			else
			{
				LOG_ERROR(Manager::Logger(), "Unknown input stream type "<<it2->second->GetTypeString());
				CPPUNIT_ASSERT_MESSAGE("Unknown input stream type", false);
			}
			inputStream.Connect(outputStream);
			CPPUNIT_ASSERT(outputStream != NULL);
			CPPUNIT_ASSERT(inputStream.IsConnected());
		}
		module->SetAsReady();
		module->Reset();
		return module;
	}

	/// Load and save a config file
	void testInputs()
	{
		LOG_TEST("# Test the creation of modules");
		
		for(std::vector<std::string>::const_iterator it1 = moduleTypes.begin() ; it1 != moduleTypes.end() ; it1++)
		{
			LOG_TEST("## create module "<<*it1);

			Module* module = createAndConnectModule(*it1);



			// const std::map<int, Stream*> outputs = module->GetOutputStreamList();
			// const std::map<int, Stream*> debugs  = module->GetDebugStreamList();

			for(int i = 0 ; i < 50 ; i++)
			{
				randomizeInputs();
				module->Process();
			}
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
