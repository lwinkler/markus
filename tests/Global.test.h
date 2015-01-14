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
#ifndef TEST_GLOBAL_H
#define TEST_GLOBAL_H
#include <cxxtest/GlobalFixture.h>
#include <log4cxx/xml/domconfigurator.h>

#include "AllFeatures.h"
#include "AllModules.h"

using namespace std;

//
// Fixture1 counts its setUp()s and tearDown()s
//
class MarkusFixture : public CxxTest::GlobalFixture
{
public:
	bool setUp() {return true;}
	bool tearDown() {return true;}
	bool setUpWorld()
	{
		log4cxx::xml::DOMConfigurator::configure("tests/log4cxx.xml");
		registerAllFeatures();
		registerAllModules(Factories::modulesFactory());

		SYSTEM("rm -rf tests/out");
		SYSTEM("rm -rf tests/tmp");
		SYSTEM("mkdir -p tests/tmp");
		return true;
	}
	bool tearDownWorld()
	{
		stringstream ss1;
		execute("cat tests/markus.log | grep WARN | grep -v EVENT", ss1);
		int nWarn = std::count(std::istreambuf_iterator<char>(ss1), std::istreambuf_iterator<char>(), '\n');
		if(nWarn > 0)
		{
			cout << "Found " << nWarn << " warnings in tests/markus.log" << endl;
		}

		stringstream ss3;
		execute("cat tests/markus.log | grep ERROR", ss3);
		return true;
		int nErr = std::count(std::istreambuf_iterator<char>(ss3), std::istreambuf_iterator<char>(), '\n');
		if(nErr > 0)
		{
			cout << "Found " << nErr << " errors in tests/markus.log" << endl;
			return false;
		}
		return true;
	}
};
static MarkusFixture g_globalFixture;
#endif
