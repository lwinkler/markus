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
#include <boost/filesystem.hpp>
#include <log4cxx/xml/domconfigurator.h>

#include "Factories.h"
#include "util.h"

using namespace std;

//
// Fixture1 counts its setUp()s and tearDown()s
//
class MarkusFixture : public CxxTest::GlobalFixture
{
public:
	bool setUp() {return true;}
	bool tearDown()
	{
		static int lastNWarn = 0;
		static int lastNErr  = 0;
		stringstream ss1;
		execute("cat tests/markus.log | grep WARN | grep -v EVENT", ss1);
		int nWarn = std::count(std::istreambuf_iterator<char>(ss1), std::istreambuf_iterator<char>(), '\n');
		if(nWarn - lastNWarn > 0)
		{
			cout << "Found " << nWarn - lastNWarn << " new warning(s) in tests/markus.log, total " << nWarn << endl;
		}
		lastNWarn = nWarn;

		stringstream ss3;
		execute("cat tests/markus.log | grep ERROR", ss3);
		int nErr = std::count(std::istreambuf_iterator<char>(ss3), std::istreambuf_iterator<char>(), '\n');
		if(nErr - lastNErr > 0)
			cout << "Found " << nErr - lastNErr << " new errors in tests/markus.log, total " << nErr << endl;
		lastNErr  = nErr;
		return nErr - lastNErr == 0;
	}
	bool setUpWorld()
	{
		log4cxx::xml::DOMConfigurator::configure("tests/log4cxx.xml");
		Factories::RegisterAll();

		boost::filesystem::remove_all("tests/out");
		boost::filesystem::remove_all("tests/tmp");
		boost::filesystem::create_directory("tests/tmp");
		return true;
	}
	bool tearDownWorld(){return true;}
};
static MarkusFixture g_globalFixture;
#endif
