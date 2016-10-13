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
	ConfigReader* m_conf5;
public:
	void setUp()
	{
		m_conf1 = new ConfigReader;
		m_conf2 = new ConfigReader;
		m_conf3 = new ConfigReader;
		m_conf4 = new ConfigReader;
		m_conf5 = new ConfigReader;
		readFromFile(*m_conf1, "tests/config/config1.json");
		createEmptyConfigFile("/tmp/config_empty.json");
		readFromFile(*m_conf2, "/tmp/config_empty.json");
		readFromFile(*m_conf3, "/tmp/config_empty.json");
		readFromFile(*m_conf4, "/tmp/config_empty.json");
		readFromFile(*m_conf5, "tests/config/config_part.json");
	}
	void tearDown()
	{
		delete m_conf1;
		delete m_conf2;
		delete m_conf3;
		delete m_conf4;
		delete m_conf5;
	}


	/// Test new syntax
	void testSyntax()
	{
		TS_TRACE("\n# Test on the syntax of configurations");
		ConfigReader& conf(*m_conf3);
		conf["t1"]["t2"]["t3"]["bla"] = 333;
		writeToFile(*m_conf3, "tests/tmp/test3.json");
		ConfigReader& conf2(*m_conf4);
		conf2["t1"]["t2"]["t3"] = "333";
		conf2["t1"]["t2"]["t3"]["blo"] = "555";
		writeToFile(*m_conf4, "tests/tmp/test4.json");

		TS_ASSERT(!conf2["t1"].isNull());
		TS_ASSERT(!conf2["t1"]["t2"].isNull());
		TS_ASSERT(!conf2["t1"]["t2"]["t3"].isNull());
		TS_ASSERT(conf2["t1"]["t2"]["t3"]["bla"].size() == 1);
		TS_ASSERT( conf2["t1"]["t2"]["t3"].asInt() == 333);
		TS_ASSERT(conf2["t1"]["t2"]["t3"].size() == 2);
	}

	/// Load and save a config file
	void testLoad()
	{
		TS_TRACE("\n# Test the loading of configurations");

		ConfigReader& appConf((*m_conf1));
		ConfigReader& module0conf(appConf["module"]["Module0"]);
		ConfigReader& module1conf = appConf["module"]["Module1"];

		// old access
		TS_ASSERT(module0conf == appConf["module"]["Module0"]);
		TS_ASSERT(module1conf == appConf["modules"]);
		// TS_ASSERT(module1conf == appConf.FindAll("module[name=\"Module1\"]").at(0));
		TS_ASSERT(! module0conf["inputs"].isNull());

		ConfigReader& param1 = module0conf["inputs"]["param"]["name"]["param_text"];
		TS_ASSERT(param1.asString() == "SomeText");
		ConfigReader& param2 = module0conf["inputs"]["param"]["name"]["param_int"];
		TS_ASSERT(param2.asString() == "21");
		ConfigReader& param3 = module0conf["inputs"]["param"]["name"]["param_float"];
		TS_ASSERT(param3.asString() == "3.1415");

		TS_ASSERT(param1["name"].asString() == "param_text");

		writeToFile(*m_conf1, "tests/tmp/config1_copy.json");
		validate(*m_conf1);

		// Compare with the initial config
		TS_ASSERT(compareFiles("tests/config/config1.json", "tests/tmp/config1_copy.json"));
	}

	/// Generate a config from an empty file and test
	void testGenerate()
	{
		TS_TRACE("\n# Test the generation of configurations");
		ConfigReader& appConf((*m_conf2));
		appConf["aaa"]["name"]["nameX"]
		["bbb"]["name"]["nameY"]
		["ccc"]["name"]["nameZ"] = "someValue";
		writeToFile(*m_conf2, "tests/config/config_generated.json");

		ConfigReader generatedConf("tests/config/config_generated.json");
		TS_ASSERT(generatedConf
				  ["aaa"]["name"]["nameX"]
				  ["bbb"]["name"]["nameY"]
				  ["ccc"]["name"]["nameZ"]
				  .asString() == "someValue");
	}

	/// Override the original config
	void testOverride()
	{
		TS_TRACE("\n# Test the override of the original configuration");

		TS_ASSERT((*m_conf1)["inputs"]["param_text]"].asString() == "SomeText0");
		TS_ASSERT((*m_conf1)["inputs"]["param_int]"].asString() == "0");
		TS_ASSERT((*m_conf1)["inputs"]["param_float]"].asString() == "0");

		overrideWith(*m_conf1, *m_conf5);

		TS_ASSERT((*m_conf1)["inputs"]["param_text"].asString() == "NewText0");
		TS_ASSERT((*m_conf1)["inputs"]["param_int"].asString() == "44");
		TS_ASSERT((*m_conf1)["inputs"]["param_float"].asString() == "0.51");

		TS_ASSERT((*m_conf1)["modules"]["Module0"]["inputs"]["param_text"].asString() == "a new text");
		TS_ASSERT((*m_conf1)["modules"]["Module0"]["inputs"]["param_int"].asString() == "33");
		TS_ASSERT((*m_conf1)["modules"]["Module0"]["inputs"]["param_float"].asString() == "3.1415");
		TS_ASSERT((*m_conf1)["modules"]["Module1"]["inputs"]["param_float"].asString()  == "77.7");
		TS_ASSERT((*m_conf1)["modules"]["Module1"]["inputs"]["param_int"].asString()  == "42");
	}
};

#endif
