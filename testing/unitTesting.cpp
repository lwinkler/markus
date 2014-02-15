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
#include <cppunit/ui/text/TestRunner.h>
#include "ConfigReaderTest.h"
#include "TestProjects.h"
#include "TestModules.h"




bool run_tests()
{
	srand(12345); // Force random seed
	CppUnit::TextUi::TestRunner runner;
	Manager::SetConfigFile("testing/markus.log");
	SYSTEM("rm -rf testing/out");
	Manager::OutputDir("testing/out");
	log4cxx::xml::DOMConfigurator::configure("testing/log4cxx.xml");

	// runner.addTest(ConfigReaderTest::suite());
	// runner.addTest(TestProjects::suite());
	runner.addTest(TestModules::suite());
	runner.run();

	return 0;
}
