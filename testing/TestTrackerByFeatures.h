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
#ifndef FEATURE_TRACKER_TEST_H
#define FEATURE_TRACKER_TEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include "modules/TrackerByFeatures/TrackerByFeatures.h"
#include "MkException.h"


/// Unit testing class for ConfigReader class

class TestTrackerByFeatures : public CppUnit::TestFixture
{
private:
	static log4cxx::LoggerPtr m_logger;
protected:
	TrackerByFeatures* mp_module;
	Module* mp_fakeInput;
	Module* mp_fakeOutput;
	ConfigReader* mp_config;

	// Objects for streams
	std::vector<Object> m_objectsIn;
	std::vector<Object> m_objectsOut;
	int m_cpt;
public:
	void runTest()
	{
	}

	void setUp()
	{
		m_cpt = 0;
		createEmptyConfigFile("/tmp/config_empty.xml");
		mp_config = new ConfigReader("/tmp/config_empty.xml");
		ConfigReader params = addModuleToConfig("TrackerByFeatures", *mp_config)
			.RefSubConfig("parameters", true);
		params.RefSubConfig("param", "name", "fps", true).SetValue("22");
		params.RefSubConfig("param", "name", "max_matching_distance", true).SetValue("0.05");
		// mp_config->RefSubConfig("application", "", true).SetAttribute("name", "unitTest");

		mp_module = new TrackerByFeatures(mp_config->Find("application>module[name=\"TrackerByFeatures0\"]"));
		mp_module->SetAsReady();
		mp_module->Reset();
	}

	void tearDown()
	{
		delete mp_module;
		//delete mp_fakeInput;
		//delete mp_fakeOutput;
		delete mp_config;
	}

	ConfigReader addModuleToConfig(const std::string& rx_type, ConfigReader& xr_config)
	{
		ConfigReader moduleConfig =  xr_config.RefSubConfig("application", true)
			.RefSubConfig("module", "name", rx_type + "0", true);
		moduleConfig.RefSubConfig("parameters", true)
			.RefSubConfig("param", "name", "class", true)
			.SetValue(rx_type);
		moduleConfig.RefSubConfig("parameters", true)
			.RefSubConfig("param", "name", "auto_process", true)
			.SetValue("1");
		moduleConfig.RefSubConfig("parameters", true)
			.RefSubConfig("param", "name", "fps", true)
			.SetValue("123");
		moduleConfig.RefSubConfig("inputs", true);
		moduleConfig.RefSubConfig("outputs", true);
		moduleConfig.SetAttribute("name", rx_type + "0");
		std::stringstream ss;
		ss<<m_cpt++;
		moduleConfig.SetAttribute("id", ss.str());
		return moduleConfig;
	}

	/**
	* @brief Test all classes that inherit from Parameter
	*/
	void testInOut()
	{
		// Connect streams
		int cpt = 0;
		unsigned int seed = 3452352345;
		Stream* outputStream0 = new StreamObject("test", m_objectsIn, *mp_module, "Test input");
		Stream* inputStream2 = new StreamObject("test", m_objectsOut, *mp_module, "Test output");
		CPPUNIT_ASSERT(outputStream0 != NULL);
		CPPUNIT_ASSERT(inputStream2 != NULL);

		mp_module->GetInputStreamList().at(0)->Connect(outputStream0);
		inputStream2->Connect(mp_module->GetOutputStreamList().at(0));


		for(int i = 0 ; i < 100 ; i++)
		{
			if(i % 10 == 0)
			{
				// Add an object to track
				m_objectsIn.push_back(Object("test"));
				m_objectsIn.back().AddFeature("gt_id",  new FeatureInt(cpt));
				m_objectsIn.back().AddFeature("x",      new FeatureFloat(static_cast<float>(rand_r(&seed)) / RAND_MAX));
				m_objectsIn.back().AddFeature("y",      new FeatureFloat(static_cast<float>(rand_r(&seed)) / RAND_MAX));
				m_objectsIn.back().AddFeature("width",  new FeatureFloat(static_cast<float>(rand_r(&seed)) / RAND_MAX));
				m_objectsIn.back().AddFeature("height", new FeatureFloat(static_cast<float>(rand_r(&seed)) / RAND_MAX));
				cpt++;
			}
			if(i > 30 && i % 10 == 0)
			{
				// Remove an object
				m_objectsIn.erase (m_objectsIn.begin());
			}

			// Add random changes to features
			for(std::vector<Object>::iterator it = m_objectsIn.begin() ; it != m_objectsIn.end() ; ++it)
			{
				float fact = 0.05 * mp_module->GetParameters().maxMatchingDistance;
				it->AddFeature("x",      dynamic_cast<const FeatureFloat&>(it->GetFeature("x")).value      + (static_cast<float>(rand_r(&seed)) / RAND_MAX - 0.5) * fact);
				it->AddFeature("y",      dynamic_cast<const FeatureFloat&>(it->GetFeature("y")).value      + (static_cast<float>(rand_r(&seed)) / RAND_MAX - 0.5) * fact);
				it->AddFeature("width",  dynamic_cast<const FeatureFloat&>(it->GetFeature("width")).value  + (static_cast<float>(rand_r(&seed)) / RAND_MAX - 0.5) * fact);
				it->AddFeature("height", dynamic_cast<const FeatureFloat&>(it->GetFeature("height")).value + (static_cast<float>(rand_r(&seed)) / RAND_MAX - 0.5) * fact);
			}

			// Call Process
			mp_module->Process();

			inputStream2->ConvertInput();
			CPPUNIT_ASSERT(m_objectsIn.size() == m_objectsOut.size());

			// Verify that the ids are as expected
			for(std::vector<Object>::const_iterator it = m_objectsOut.begin() ; it != m_objectsOut.end() ; ++it)
			{
				// std::cout<<it->GetId()<<" == "<<dynamic_cast<const FeatureInt&>(it->GetFeature("gt_id")).value<<std::endl;
				CPPUNIT_ASSERT(it->GetId() == dynamic_cast<const FeatureInt&>(it->GetFeature("gt_id")).value);
			}
		}

		delete outputStream0;
		delete inputStream2;
	}

	static CppUnit::Test *suite()
	{
		CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite("TestTrackerByFeatures");
		// TODO: See if we keep: suiteOfTests->addTest(new CppUnit::TestCaller<TestTrackerByFeatures>("testTrackerByFeatures", &TestTrackerByFeatures::testInOut));
		return suiteOfTests;
	}
};
log4cxx::LoggerPtr TestTrackerByFeatures::m_logger(log4cxx::Logger::getLogger("TestTrackerByFeatures"));
#endif
