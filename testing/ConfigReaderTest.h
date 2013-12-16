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
		m_conf1 = new ConfigReader("testing/config1.xml");
	}
	void tearDown()
	{
		delete m_conf1;
	}

	void testLoad()
	{
		ConfigReader appConf = m_conf1->GetSubConfig("application");
		ConfigReader module0conf = appConf.GetSubConfig("module", "Module0");
		ConfigReader module1conf = appConf.GetSubConfig("module", "Module1");
		CPPUNIT_ASSERT(module0conf == appConf.GetSubConfig("module", "Module0"));
		CPPUNIT_ASSERT(module1conf == module0conf.NextSubConfig("module"));
		CPPUNIT_ASSERT(module1conf == module0conf.NextSubConfig("module", "Module1"));
		CPPUNIT_ASSERT(module1conf.NextSubConfig("module").IsEmpty());
		CPPUNIT_ASSERT(! module0conf.GetSubConfig("parameters").IsEmpty());

		ConfigReader param1 = module0conf.GetSubConfig("parameters").GetSubConfig("param", "param_text");
		CPPUNIT_ASSERT(param1.GetValue() == "SomeText");
		ConfigReader param2 = module0conf.GetSubConfig("parameters").GetSubConfig("param", "param_int");
		CPPUNIT_ASSERT(param2.GetValue() == "21");
		ConfigReader param3 = module0conf.GetSubConfig("parameters").GetSubConfig("param", "param_float");
		CPPUNIT_ASSERT(param3.GetValue() == "3.1415");

		CPPUNIT_ASSERT(atoi(param1.GetAttribute("id").c_str()) == 0);
		CPPUNIT_ASSERT(param1.GetAttribute("name") == "param_text");

		m_conf1->SaveToFile("testing/config1_copy.xml"); // TODO: Test that files are identical
		m_conf1->Validate();

/*
	ConfigReader RefSubConfig(const std::string& x_objectType, std::string x_objectName = "", bool x_allowCreation = false);
	void SetValue(const std::string& x_value);
	void Validate() const;
	*/
	}

	static CppUnit::Test *suite()
	{
		CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite("ConfigReaderTest");
		suiteOfTests->addTest(new CppUnit::TestCaller<ConfigReaderTest>("testLoad", &ConfigReaderTest::testLoad));
		return suiteOfTests;
	}
};
#endif
