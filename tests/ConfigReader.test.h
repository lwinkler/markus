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
#include "config.h"
#include "util.h"
#include "MkException.h"

using namespace std;
using namespace mk;

class mkconfTestSuite : public CxxTest::TestSuite
{
public:
	/// Test new syntax
	void testSyntax()
	{
		TS_TRACE("\n# Test on the syntax of configurations");
		mkconf conf;
		conf["t1"]["t2"]["t3"]["bla"] = 333;
		writeToFile(conf, "tests/tmp/test3.json");
		mkconf conf2;
		readFromFile(conf2, "tests/tmp/test3.json");

		TS_ASSERT(!conf2["t1"].is_null());
		TS_ASSERT(!conf2["t1"]["t2"].is_null());
		TS_ASSERT(!conf2["t1"]["t2"]["t3"].is_null());
		TS_ASSERT( conf2["t1"]["t2"]["t3"]["bla"].get<int>() == 333);
	}

	/// Load and save a config file
	void testLoad()
	{
		TS_TRACE("\n# Test the loading of configurations");

		mkconf appConf;
		readFromFile(appConf, "tests/config/config1.json");
		mkconf& module0conf(findFirstInArray(appConf.at("modules"), "name", "Module0"));
		mkconf& module1conf = findFirstInArray(appConf.at("modules"), "name", "Module1");

		TS_ASSERT(! module0conf["inputs"].is_null());
		TS_ASSERT(! module1conf["inputs"].is_null());

		mkconf& param1 = findFirstInArray(module0conf.at("inputs"), "name", "param_text");
		TS_ASSERT(param1["value"].get<string>() == "SomeText");
		mkconf& param2 = findFirstInArray(module0conf.at("inputs"), "name", "param_int");
		TS_ASSERT(param2["value"].get<int>() == 21);
		mkconf& param3 = findFirstInArray(module0conf.at("inputs"), "name", "param_float");
		TS_ASSERT(param3["value"].get<double>() == 3.1415);

		mkconf conf1;
		readFromFile(conf1, "tests/config/config1.json");
		writeToFile(conf1, "tests/tmp/config1_copy.json");

		// Compare with the initial config
		TS_ASSERT(compareJsonFiles("tests/config/config1.json", "tests/tmp/config1_copy.json"));
	}

	/// Generate a config from an empty file and test
	void testGenerate()
	{
		TS_TRACE("\n# Test the generation of configurations");
		mkconf appConf;
		appConf["aaa"]["nameX"]
		["bbb"]["nameY"]
		["ccc"]["nameZ"] = "someValue";
		writeToFile(appConf, "tests/tmp/config_generated.json");

		mkconf generatedConf;
		readFromFile(generatedConf, "tests/tmp/config_generated.json");
		TS_ASSERT(generatedConf
				  ["aaa"]["nameX"]
				  ["bbb"]["nameY"]
				  ["ccc"]["nameZ"]
				  .get<string>() == "someValue");
	}

	/// Override the original config
	void testOverride()
	{
		TS_TRACE("\n# Test the override of the original configuration");
		mkconf conf1;
		readFromFile(conf1, "tests/config/config1.json");
		mkconf& conf2(conf1["modules"][0]);

		mkconf conf5;
		readFromFile(conf5, "tests/config/config_part.json");

		TS_ASSERT(findFirstInArray(conf2.at("inputs"), "name", "param_text").at("value").get<string>() == "SomeText");
		TS_ASSERT(findFirstInArray(conf2.at("inputs"), "name", "param_int").at("value").get<int>() == 21);
		TS_ASSERT(findFirstInArray(conf2.at("inputs"), "name", "param_float").at("value").get<double>() == 3.1415000000000002);

		overrideWith(conf1, conf5);

		TS_ASSERT(findFirstInArray(findFirstInArray(conf1.at("modules"), "name", "Module0")["inputs"], "name", "param_text")["value"].get<string>() == "a new text");
		TS_ASSERT(findFirstInArray(findFirstInArray(conf1.at("modules"), "name", "Module0")["inputs"], "name", "param_int")["value"].get<int>() == 33);
		TS_ASSERT(findFirstInArray(findFirstInArray(conf1.at("modules"), "name", "Module0")["inputs"], "name", "param_float")["value"].get<double>() == 3.1415);
		TS_ASSERT(findFirstInArray(findFirstInArray(conf1.at("modules"), "name", "Module1")["inputs"], "name", "param_float")["value"].get<double>()  == 77.7);
		TS_ASSERT(findFirstInArray(findFirstInArray(conf1.at("modules"), "name", "Module1")["inputs"], "name", "param_int")["value"].get<int>()  == 42);
	}
};

#endif
