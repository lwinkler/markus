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
#ifndef TEST_SERIALIZATION_H
#define TEST_SERIALIZATION_H

#include <cppunit/TestFixture.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include <sstream>

#include "util.h"
#include "MkException.h"
#include "Serializable.h"

// #include "StreamObject.h"

#define LOG_TEST(logger, str) {\
	std::cout<<str<<std::endl;\
	LOG_INFO((logger), str);\
}


/// Test class for serialization
class TestObject : public Serializable
{
public:
	TestObject()
	{
		m_int = 333;
	}
	virtual void Serialize(std::ostream& x_out, const std::string& x_dir) const
	{
		Json::Value root;
		root["int1"] = m_int;
		// Serialize("int1", m_int, root, x_dir);
		x_out << root;
	}
	virtual void Deserialize(std::istream& x_in, const std::string& x_dir)
	{
		Json::Value root;
		x_in >> root;
		m_int = root["int1"].asInt();

	}
	
protected:
	int m_int;
};

/// Unit testing class for serializable classes


class TestSerialization : public CppUnit::TestFixture
{
	private:
		static log4cxx::LoggerPtr m_logger;
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

	/// Test the serialization of one class
	void testSerialization(Serializable& obj, const std::string& name)
	{
		std::stringstream ss;
		obj.Serialize(ss, "tmp/serialize_" + name);
		std::cout<<ss.str()<<std::endl;
	}


	void testSerialization1()
	{
		TestObject obj;
		testSerialization(obj, "testObject");
	}

	static CppUnit::Test *suite()
	{
		CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite("TestSerialization");
		suiteOfTests->addTest(new CppUnit::TestCaller<TestSerialization>("testSerialization1", &TestSerialization::testSerialization1));
		return suiteOfTests;
	}
};
log4cxx::LoggerPtr TestSerialization::m_logger(log4cxx::Logger::getLogger("TestSerialization"));
#endif
