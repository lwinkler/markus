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

#include <cppunit/TestFixture.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>

#include "Manager.h"
#include "util.h"
#include "MkException.h"

/// Unit testing class for ConfigReader class

class TestProjects : public CppUnit::TestFixture
{
	protected:
	public:
	/*void runTest()
	{
	}*/
	void setUp()
	{
	}
	void tearDown()
	{
	}

	void runConfig(const std::string& x_configFile)
	{
		LOG_INFO(Manager::Logger(), "Unit test with configuration "<<x_configFile);
		ConfigReader mainConfig(x_configFile);
		mainConfig.Validate();
		ConfigReader appConfig = mainConfig.GetSubConfig("application");
		CPPUNIT_ASSERT(!appConfig.IsEmpty());
		Manager manager(appConfig, 1);
		manager.Connect();
		manager.Reset();
		while(manager.Process())
		{
			// nothing 
		}
	}

	/// Run different existing configs
	void testSync()
	{
		runConfig("testing/projects/sync_test1.xml");
		runConfig("testing/projects/sync_test2.xml");
		runConfig("testing/projects/sync_test3.xml");
		runConfig("testing/projects/sync_test4.xml");
		runConfig("testing/projects/FaceAndTracker.xml");
	}

	static CppUnit::Test *suite()
	{
		CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite("TestProjects");
		suiteOfTests->addTest(new CppUnit::TestCaller<TestProjects>("testSync", &TestProjects::testSync));
		return suiteOfTests;
	}
};
#endif
