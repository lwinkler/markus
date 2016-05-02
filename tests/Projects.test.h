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
		CLEAN_DELETE(mp_context);
		CLEAN_DELETE(mp_contextParams);
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
		try
		{
			TS_TRACE("## Unit test with configuration " + x_configFile);
			ConfigFile mainConfig(x_configFile);
			mainConfig.Validate();
			ConfigReader appConfig = mainConfig.GetSubConfig("application");
			// Note: Added this to avoid deleting the output directory
			TS_ASSERT(!appConfig.IsEmpty());
			Manager::Parameters params(appConfig);
			params.aspectRatio = x_aspectRatio;
			// params.autoProcess = false;
			Manager manager(params);
			CLEAN_DELETE(mp_context);
			CLEAN_DELETE(mp_contextParams);
			mp_contextParams = new Context::Parameters(appConfig, "/tmp/config_empty.xml", "TestProjects", "tests/out");
			mp_contextParams->centralized = true;
			mp_contextParams->autoClean   = false;
			mp_context = new Context(*mp_contextParams);
			manager.SetContext(*mp_context);
			manager.Connect();
			manager.Reset();

			for(auto& module : manager.GetModules())
			{
				// cout << module->GetHeight() << " * " << x_aspectRatio << " == " <<  module->GetWidth() << endl;
				TS_ASSERT(static_cast<int>(module->GetHeight() * convertAspectRatio(x_aspectRatio)) == module->GetWidth() 
				|| static_cast<int>(module->GetWidth() / convertAspectRatio(x_aspectRatio)) == module->GetHeight());
			}

			for(int i = 0 ; i < 10 ; i++)
				if(!manager.ProcessAndCatch())
					break;
		}
		catch(ParameterException& e)
		{
			TS_TRACE("Exception probably while setting aspect ratio: " + string(e.what()));
		}
	}

	Context::Parameters* mp_contextParams = nullptr;
	Context* mp_context                   = nullptr;

public:
	/// Run different existing configs
	void testSync()
	{
		TS_TRACE("\n# Unit test with different test projects");
		runConfig("tests/projects/sync_test1.xml");
		runConfig("tests/projects/sync_test2.xml");
		runConfig("tests/projects/sync_test3.xml");
		runConfig("tests/projects/sync_test4.xml");
		runConfig("tests/projects/FaceAndTracker.xml");
	}
};
#endif
