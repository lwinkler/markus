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
#include <log4cxx/xml/domconfigurator.h>
#include "ConfigReaderTest.h"
#include "TestProjects.h"
#include "TestModules.h"
#include "TestSerialization.h"
#include "TestParameters.h"


using namespace std;


int main(int argc, char **argv)
{
	CppUnit::TextUi::TestRunner runner;
	SYSTEM("rm -rf testing/out");
	SYSTEM("rm -rf testing/tmp");
	SYSTEM("mkdir -p testing/tmp");
	Manager::OutputDir("testing/out");
	log4cxx::xml::DOMConfigurator::configure("testing/log4cxx.xml");

	runner.addTest(ConfigReaderTest::suite());
	runner.addTest(TestProjects::suite());
	runner.addTest(TestModules::suite());
	runner.addTest(TestSerialization::suite());
	runner.addTest(TestParameters::suite());

	try
	{
		if(argc <= 1)
			runner.run();
		else
			runner.run(argv[1]);
	}
	catch(exception& e)
	{
		cout<<"Error running tests: "<<e.what()<<endl;
	}

	return 0;
}
