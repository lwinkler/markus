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
private:
	static log4cxx::LoggerPtr m_logger;
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
		LOG_TEST(m_logger, "## Unit test with configuration "<<x_configFile);
		ConfigReader mainConfig(x_configFile);
		mainConfig.Validate();
		ConfigReader appConfig = mainConfig.GetSubConfig("application");
		// Note: Added this to avoid deleting the output directory // TODO: Output dir should not be static probably
		appConfig.RefSubConfig("parameters", true).RefSubConfig("param", "name", "auto_clean", true).SetValue("0");
		appConfig.RefSubConfig("parameters", true).RefSubConfig("param", "name", "auto_process", true).SetValue("1");
		TS_ASSERT(!appConfig.IsEmpty());
		Manager manager(appConfig);
		Context context("", "ProjectsTestSuite", "tests/out");
		manager.SetContext(context);
		manager.AllowAutoProcess(false);
		manager.Connect();
		manager.Reset();

		for(int i = 0 ; i < 10 ; i++)
			if(!manager.Process())
				break;
	}

public:
	/// Run different existing configs
	void testSync()
	{
		LOG_TEST(m_logger, "\n# Unit test with different test projects");
		runConfig("tests/projects/sync_test1.xml");
		runConfig("tests/projects/sync_test2.xml");
		runConfig("tests/projects/sync_test3.xml");
		runConfig("tests/projects/sync_test4.xml");
		runConfig("tests/projects/FaceAndTracker.xml");
	}
};
log4cxx::LoggerPtr ProjectsTestSuite::m_logger(log4cxx::Logger::getLogger("ProjectsTestSuite"));
#endif
