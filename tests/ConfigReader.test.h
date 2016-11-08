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
public:
	/// Test new syntax
	void testSyntax()
	{
		TS_TRACE("\n# Test on the syntax of configurations");
		ConfigReader conf;
		conf["t1"]["t2"]["t3"]["bla"] = 333;
		writeToFile(conf, "tests/tmp/test3.json");
		ConfigReader conf2;
		readFromFile(conf2, "tests/tmp/test3.json");

		TS_ASSERT(!conf2["t1"].isNull());
		TS_ASSERT(!conf2["t1"]["t2"].isNull());
		TS_ASSERT(!conf2["t1"]["t2"]["t3"].isNull());
		TS_ASSERT( conf2["t1"]["t2"]["t3"]["bla"].asInt() == 333);
	}

	/// Load and save a config file
	void testLoad()
	{
		TS_TRACE("\n# Test the loading of configurations");

		ConfigReader appConf;
		readFromFile(appConf, "tests/config/config1.json");
		ConfigReader& module0conf(appConf["modules"]["Module0"]);
		ConfigReader& module1conf = appConf["modules"]["Module1"];

		// old access
		TS_ASSERT(module0conf == appConf["modules"]["Module0"]);
		TS_ASSERT(module1conf == appConf["modules"]["Module1"]);
		TS_ASSERT(! module0conf["inputs"].isNull());

		ConfigReader& param1 = module0conf["inputs"]["param_text"];
		TS_ASSERT(param1.asString() == "SomeText");
		ConfigReader& param2 = module0conf["inputs"]["param_int"];
		TS_ASSERT(param2.asInt() == 21);
		ConfigReader& param3 = module0conf["inputs"]["param_float"];
		TS_ASSERT(param3.asDouble() == 3.1415);

		ConfigReader conf1;
		readFromFile(conf1, "tests/config/config1.json");
		writeToFile(conf1, "tests/tmp/config1_copy.json");
		validate(conf1);

		// Compare with the initial config
		TS_ASSERT(compareJsonFiles("tests/config/config1.json", "tests/tmp/config1_copy.json"));
	}

	/// Generate a config from an empty file and test
	void testGenerate()
	{
		TS_TRACE("\n# Test the generation of configurations");
		ConfigReader appConf;
		appConf["aaa"]["nameX"]
		["bbb"]["nameY"]
		["ccc"]["nameZ"] = "someValue";
		writeToFile(appConf, "tests/tmp/config_generated.json");

		ConfigReader generatedConf;
		readFromFile(generatedConf, "tests/tmp/config_generated.json");
		TS_ASSERT(generatedConf
				  ["aaa"]["nameX"]
				  ["bbb"]["nameY"]
				  ["ccc"]["nameZ"]
				  .asString() == "someValue");
	}

	/// Override the original config
	void testOverride()
	{
		TS_TRACE("\n# Test the override of the original configuration");
		ConfigReader conf1;
		readFromFile(conf1, "tests/config/config1.json");

		ConfigReader conf5;
		readFromFile(conf5, "tests/config/config_part.json");

		TS_ASSERT(conf1["inputs"]["param_text"].asString() == "SomeText0");
		TS_ASSERT(conf1["inputs"]["param_int"].asInt() == 0);
		TS_ASSERT(conf1["inputs"]["param_float"].asInt() == 0);

		overrideWith(conf1, conf5);

		TS_ASSERT(conf1["inputs"]["param_text"].asString() == "NewText0");
		TS_ASSERT(conf1["inputs"]["param_int"].asInt() == 44);
		TS_ASSERT(conf1["inputs"]["param_float"].asDouble() == 0.51);

		TS_ASSERT(conf1["modules"]["Module0"]["inputs"]["param_text"].asString() == "a new text");
		TS_ASSERT(conf1["modules"]["Module0"]["inputs"]["param_int"].asInt() == 33);
		TS_ASSERT(conf1["modules"]["Module0"]["inputs"]["param_float"].asDouble() == 3.1415);
		TS_ASSERT(conf1["modules"]["Module1"]["inputs"]["param_float"].asDouble()  == 77.7);
		TS_ASSERT(conf1["modules"]["Module1"]["inputs"]["param_int"].asInt()  == 42);
	}
};

#endif
