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
#ifndef CONFIG_READER_TEST_H
#define CONFIG_READER_TEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include "ConfigReader.h"

class ConfigReaderTest : public CppUnit::TestFixture
{
	protected:
		ConfigReader* m_conf1;
	public:
	void runTest()
	{
	}
	void setUp()
	{
		m_conf1 = new ConfigReader();
	}
	void tearDown()
	{
	}

	void testA()
	{
		// CPPUNIT_ASSERT(Complex(10, 1) == Complex(10, 1));
		// CPPUNIT_ASSERT(!(Complex(1, 1) == Complex(2, 2)));
	}

	static CppUnit::Test *suite()
	{
		CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite("ConfigReaderTest");
		suiteOfTests->addTest(new CppUnit::TestCaller<ConfigReaderTest>("testEquality", &ConfigReaderTest::testA));
		suiteOfTests->addTest(new CppUnit::TestCaller<ConfigReaderTest>("testAddition", &ConfigReaderTest::testA));
		return suiteOfTests;
	}
};
#endif
