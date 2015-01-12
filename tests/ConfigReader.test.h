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
#ifndef TEST_CONFIGREADER_H
#define TEST_CONFIGREADER_H

#include <cxxtest/TestSuite.h>
#include "ConfigReader.h"
#include "util.h"
#include "MkException.h"

using namespace std;

class ConfigReaderTestSuite : public CxxTest::TestSuite
{
protected:
	ConfigReader* m_conf1;
	ConfigReader* m_conf2;
	ConfigReader* m_conf3;
	ConfigReader* m_conf4;
public:
	void setUp()
	{
		m_conf1 = new ConfigReader("tests/config1.xml");
		createEmptyConfigFile("/tmp/config_empty.xml");
		m_conf2 = new ConfigReader("/tmp/config_empty.xml");
		m_conf3 = new ConfigReader("/tmp/config_empty.xml");
		m_conf4 = new ConfigReader("/tmp/config_empty.xml");
	}
	void tearDown()
	{
		delete m_conf1;
		delete m_conf2;
		delete m_conf3;
		delete m_conf4;
	}


	/// Test new syntax
	void testSyntax()
	{
		TS_TRACE("\n# Test on the syntax of configurations");
		ConfigReader conf(*m_conf3);
		conf.RefSubConfig("t1", true).RefSubConfig("t2", true).RefSubConfig("t3", "name", "bla", true).SetValue("333");
		m_conf3->SaveToFile("tests/tmp/test3.xml");
		conf = *m_conf4;
		conf.FindRef("t1>t2>t3[name=\"bla\"]", true).SetValue("333");
		conf.FindRef("t1>t2>t3[name=\"blo\"]", true).SetValue("555");
		m_conf4->SaveToFile("tests/tmp/test4.xml");
		// TODO: Test that both files are similar

		TS_ASSERT(!conf.Find("t1").IsEmpty());
		TS_ASSERT(!conf.Find("t1>t2").IsEmpty());
		TS_ASSERT(!conf.Find("t1>t2>t3[name=\"bla\"]").IsEmpty());
		TS_ASSERT( conf.Find("t1>t2>t3[name=\"bla\"]").GetValue() == "333");
		TS_ASSERT(conf.FindAll("t1>t2>t3[name=\"bla\"]").size() == 1);
		TS_ASSERT(conf.FindAll("t1>t2>t3").size() == 2);
		TS_ASSERT(conf.Find("t1>t2").FindAll("t3").size() == 2);
	}

	/// Load and save a config file
	void testLoad()
	{
		TS_TRACE("\n# Test the loading of configurations");

		ConfigReader appConf = m_conf1->GetSubConfig("application");
		ConfigReader module0conf = appConf.GetSubConfig("module", "name", "Module0");
		ConfigReader module1conf = appConf.GetSubConfig("module", "name", "Module1");

		// old access
		TS_ASSERT(module0conf == appConf.GetSubConfig("module", "name", "Module0"));
		TS_ASSERT(module1conf == module0conf.NextSubConfig("module"));
		TS_ASSERT(module1conf == module0conf.NextSubConfig("module", "name", "Module1"));
		TS_ASSERT(module1conf.NextSubConfig("module").IsEmpty());
		TS_ASSERT(! module0conf.GetSubConfig("parameters").IsEmpty());

		ConfigReader param1 = module0conf.GetSubConfig("parameters").GetSubConfig("param", "name", "param_text");
		TS_ASSERT(param1.GetValue() == "SomeText");
		ConfigReader param2 = module0conf.GetSubConfig("parameters").GetSubConfig("param", "name", "param_int");
		TS_ASSERT(param2.GetValue() == "21");
		ConfigReader param3 = module0conf.GetSubConfig("parameters").GetSubConfig("param", "name", "param_float");
		TS_ASSERT(param3.GetValue() == "3.1415");

		TS_ASSERT(atoi(param1.GetAttribute("id").c_str()) == 0);
		TS_ASSERT(param1.GetAttribute("name") == "param_text");

		m_conf1->SaveToFile("tests/tmp/config1_copy.xml");
		m_conf1->Validate();

		// Compare with the initial config
		// note: this is kind of hackish ... can you find a better way :-)
		TS_ASSERT(compareFiles("tests/config1.xml", "tests/tmp/config1_copy.xml"));
	}

	/// Generate a config from an empty file and test
	void testGenerate()
	{
		TS_TRACE("\n# Test the generation of configurations");
		ConfigReader appConf = m_conf2->RefSubConfig("application", true);
		appConf.RefSubConfig("aaa", "name", "nameX", true)
			.RefSubConfig("bbb", "name", "nameY", true)
			.RefSubConfig("ccc", "name", "nameZ", true).SetValue("someValue");
		m_conf2->SaveToFile("tests/config_generated.xml");

		ConfigReader generatedConf("tests/config_generated.xml");
		TS_ASSERT(generatedConf.GetSubConfig("application")
				.GetSubConfig("aaa", "name", "nameX")
				.GetSubConfig("bbb", "name", "nameY")
				.GetSubConfig("ccc", "name", "nameZ")
				.GetValue() == "someValue");
	}
};

#endif
