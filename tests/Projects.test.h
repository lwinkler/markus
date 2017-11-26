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
#ifndef TEST_PROJECTS_H
#define TEST_PROJECTS_H

#include <cxxtest/TestSuite.h>

#include "util.h"
#include "MkException.h"
#include "Manager.h"

using namespace std;

/// Unit testing class by choosing a set of test projects


class ProjectsTestSuite : public CxxTest::TestSuite
{
public:
	void setUp()
	{
	}
	void tearDown()
	{
	}

protected:
	void runConfig(const string& x_configFile)
	{
		vector<string> aspectRatios = {"4:3", "16:9", "10:1", "1:1", "3:4", "9:16", "1:10"};
		for(auto& elem : aspectRatios)
		{
			runConfig(x_configFile, elem);
		}
	}

	void runConfig(const string& x_configFile, const string& x_aspectRatio)
	{
		TS_TRACE("## Unit test with configuration " + x_configFile);
		ConfigReader appConfig;
		readFromFile(appConfig, x_configFile);
		validate(appConfig);
		// Note: Added this to avoid deleting the output directory
		TS_ASSERT(!appConfig.is_null());
		Manager::Parameters params(appConfig);
		params.aspectRatio     = x_aspectRatio;
		params.autoProcess     = false;
		Context::Parameters contextParams(appConfig["name"].get<string>());
		contextParams.configFile      = x_configFile;
		contextParams.outputDir       = "";
		contextParams.applicationName = "TestProjects";
		contextParams.centralized = true;
		contextParams.autoClean   = true;
		contextParams.Read(appConfig);
		Context context(contextParams);

		try
		{
			Manager manager(params, context);
			manager.Connect();
			manager.Reset();

			for(auto& module : manager.RefModules())
			{
				// cout << module->GetHeight() << " * " << x_aspectRatio << " == " <<  module->GetWidth() << endl;
				TS_ASSERT(static_cast<int>(module->GetHeight() * convertAspectRatio(x_aspectRatio)) == module->GetWidth() 
				|| static_cast<int>(module->GetWidth() / convertAspectRatio(x_aspectRatio)) == module->GetHeight());
			}

			for(int i = 0 ; i < 10 ; i++)
				TS_ASSERT(manager.ProcessAndCatch())
		}
		catch(ParameterException& e)
		{
			TS_TRACE("Exception probably while setting aspect ratio: " + string(e.what()));
		}
	}

public:
	/// Run different existing configs
	void testProjects1()
	{
		TS_TRACE("\n# Unit test with different test projects");
		runConfig("tests/projects/sync_test1.json");
		runConfig("tests/projects/sync_test2.json");
		runConfig("tests/projects/sync_test3.json");
		runConfig("tests/projects/sync_test4.json");
		runConfig("tests/projects/FaceAndTracker.json");
	}

	/// Run different existing configs: JSONs ending in testing.json
	// disabled since this would log a lot of errors
	void disabled_testProjects2()
	{
		TS_TRACE("\n# Unit test with different test projects: JSONs ending in ...testing.json");
		vector<string> result1;
		execute("xargs -a modules.txt -I{} find {} -name \"testing*.json\"", result1);

		// Since most JSONs make tests and would output errors with a different aspect ratio,
		// we will redirect the logs where they will not be checked for errors
		for(auto& elem : result1)
		{
			runConfig(elem);
		}
	}

	void testBySpecificXmlProjects()
	{
		vector<string> result1;
		execute("xargs -a modules.txt -I{} find {} -name \"testing*.json\"", result1);

		for(auto elem : result1)
		{
			// For each json, execute the file with Markus executable
			TS_TRACE("Testing JSON project " + elem);
			string outDir = "out/test_" + basename(elem) + "_" + timeStamp();
			string cmd = "./markus -ncf " + elem + " -o " + outDir + " > /dev/null";
			TS_TRACE("Execute " + cmd);
			SYSTEM(cmd);
			// gather possible errors and warnings
			stringstream ssWarn;
			stringstream ssErr;
			execute("cat " + outDir + "/markus.log | grep WARN | grep -v @notif@", ssWarn);
			int nWarn = std::count(std::istreambuf_iterator<char>(ssWarn), std::istreambuf_iterator<char>(), '\n');
			if(nWarn > 0)
			{
				// Log warnings
				TS_WARN("Found " + to_string(nWarn) + " warning(s) in " + outDir + "/markus.log");
				cout << ssWarn.str() << endl;
			}
			execute("cat " + outDir + "/markus.log | grep ERROR", ssErr);
			int nErr = std::count(std::istreambuf_iterator<char>(ssErr), std::istreambuf_iterator<char>(), '\n');
			if(nErr > 0)
			{
				// Log errors
				TS_FAIL("Found " + to_string(nErr) + " error(s) in " + outDir + "/markus.log");
				cout << ssErr.str() << endl;
			}
		}

	}

};
#endif
