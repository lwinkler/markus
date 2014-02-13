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
#ifndef CONFIG_READER_TEST_H
#define CONFIG_READER_TEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include "ConfigReader.h"
#include "util.h"
#include "Manager.h"
#include "MkException.h"

#define LOG_TEST(str) {\
	std::cout<<std::endl<<str<<std::endl;\
	LOG_INFO(Manager::Logger(), str);\
}

/// Unit testing class for ConfigReader class

class ConfigReaderTest : public CppUnit::TestFixture
{
	protected:
		ConfigReader* m_conf1;
		ConfigReader* m_conf2;
	public:
	void runTest()
	{
	}
	void setUp()
	{
		m_conf1 = new ConfigReader("testing/config1.xml");
		m_conf2 = new ConfigReader("config_empty.xml");
	}
	void tearDown()
	{
		delete m_conf1;
		delete m_conf2;
	}

	/// Load and save a config file
	void testLoad()
	{
		LOG_TEST("# Test the loading of configurations");

		ConfigReader appConf = m_conf1->GetSubConfig("application");
		ConfigReader module0conf = appConf.GetSubConfig("module", "Module0");
		ConfigReader module1conf = appConf.GetSubConfig("module", "Module1");
		CPPUNIT_ASSERT(module0conf == appConf.GetSubConfig("module", "Module0"));
		CPPUNIT_ASSERT(module1conf == module0conf.NextSubConfig("module"));
		CPPUNIT_ASSERT(module1conf == module0conf.NextSubConfig("module", "Module1"));
		CPPUNIT_ASSERT(module1conf.NextSubConfig("module").IsEmpty());
		CPPUNIT_ASSERT(! module0conf.GetSubConfig("parameters").IsEmpty());

		ConfigReader param1 = module0conf.GetSubConfig("parameters").GetSubConfig("param", "param_text");
		CPPUNIT_ASSERT(param1.GetValue() == "SomeText");
		ConfigReader param2 = module0conf.GetSubConfig("parameters").GetSubConfig("param", "param_int");
		CPPUNIT_ASSERT(param2.GetValue() == "21");
		ConfigReader param3 = module0conf.GetSubConfig("parameters").GetSubConfig("param", "param_float");
		CPPUNIT_ASSERT(param3.GetValue() == "3.1415");

		CPPUNIT_ASSERT(atoi(param1.GetAttribute("id").c_str()) == 0);
		CPPUNIT_ASSERT(param1.GetAttribute("name") == "param_text");

		m_conf1->SaveToFile("testing/tmp/config1_copy.xml");
		m_conf1->Validate();

		// Compare with the initial config
		// note: this is kind of hackish ... can you find a better way :-)
		SYSTEM("diff testing/config1.xml testing/tmp/config1_copy.xml | xargs -i{} ERROR_non_identical_files");
	}

	/// Generate a config from an empty file and test
	void testGenerate()
	{
		LOG_TEST("# Test the generation of configurations");
		ConfigReader appConf = m_conf2->RefSubConfig("application");
		appConf.RefSubConfig("aaa", "nameX", true)
			.RefSubConfig("bbb", "nameY", true)
			.RefSubConfig("ccc", "nameZ", true).SetValue("someValue");
		m_conf2->SaveToFile("testing/config_generated.xml");

		ConfigReader generatedConf("testing/config_generated.xml");
		CPPUNIT_ASSERT(generatedConf.GetSubConfig("application")
				.GetSubConfig("aaa", "nameX")
				.GetSubConfig("bbb", "nameY")
				.GetSubConfig("ccc", "nameZ")
				.GetValue() == "someValue");
	}


	static CppUnit::Test *suite()
	{
		CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite("ConfigReaderTest");
		suiteOfTests->addTest(new CppUnit::TestCaller<ConfigReaderTest>("testLoad", &ConfigReaderTest::testLoad));
		suiteOfTests->addTest(new CppUnit::TestCaller<ConfigReaderTest>("testGenerate", &ConfigReaderTest::testGenerate));
		return suiteOfTests;
	}
};
#endif
