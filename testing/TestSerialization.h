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
#include <jsoncpp/json/writer.h>
#include <jsoncpp/json/reader.h>
#include <sstream>

#include "util.h"
#include "MkException.h"
#include "Serializable.h"

#include "StreamImage.h"
#include "StreamObject.h"
#include "StreamDebug.h"

#define LOG_TEST(logger, str) {\
	std::cout<<str<<std::endl;\
	LOG_INFO((logger), str);\
}


/// Test class for serialization
class TestObject : public Serializable
{
public:
	TestObject()
	: m_obj("test object")
	{
		m_int    = 333;
		m_float  = 55.1233234;
		m_double = 34.444;
		m_string = "test_string";

		m_obj.AddFeature("some_feat", 46.30);
	}
	virtual void Serialize(std::ostream& x_out, const std::string& x_dir) const
	{
		Json::Value root;
		root["int1"] = m_int;
		root["float1"] = m_float;
		root["double1"] = m_double;
		root["string1"] = m_string;
		std::stringstream ss;
		m_obj.Serialize(ss, x_dir);
		ss >> root["object"];
		x_out << root;
	}
	virtual void Deserialize(std::istream& x_in, const std::string& x_dir)
	{
		Json::Value root;
		x_in >> root;
		m_int = root["int1"].asInt();
		m_float = root["float1"].asFloat();
		m_double = root["double1"].asDouble();
		m_string = root["string1"].asString();
		std::stringstream ss;
		ss << root["object"];
		m_obj.Deserialize(ss, x_dir);
	}
	
protected:
	int m_int;
	float m_float;
	double m_double;
	std::string m_string;
	Object m_obj;
};

/// Unit testing class for serializable classes


class TestSerialization : public CppUnit::TestFixture
{
	private:
		static log4cxx::LoggerPtr m_logger;
		FactoryModules m_factory;
		Module* mp_fakeInput;
		ConfigReader* mp_config;
	public:
	/*void runTest()
	{
	}*/
	void setUp()
	{
		createEmtpyConfigFile("/tmp/config_empty.xml");
		mp_config = new ConfigReader("testing/serialize/module.xml");
		mp_fakeInput = m_factory.CreateModule("VideoFileReader", mp_config->GetSubConfig("module"));
		// note: we need a fake module to create the input streams
		mp_fakeInput->SetAsReady();
		mp_fakeInput->Reset();
	}
	void tearDown()
	{
		delete mp_fakeInput;
		delete mp_config;
	}

	/// Test the serialization of one serializable class
	void testSerialization(Serializable& obj, const std::string& name)
	{
		std::string fileName1 = "testing/tmp/serialize1_" + name + ".json";
		std::string fileName2 = "testing/tmp/serialize2_" + name + ".json";
		std::string fileName3 = "testing/serialize/" + name + ".json";
		std::string dir = "testing/tmp/serialize_" + name;
		SYSTEM("mkdir -p " + dir);
		LOG_TEST(m_logger, "Test serialization of "<<name);
		std::ofstream of1(fileName1.c_str());
		obj.Serialize(of1, dir);
		of1.close();

		LOG_TEST(m_logger, "Test deserialization and serialization of "<<name);
		// std::stringstream ss2;

		std::ifstream inf(fileName3.c_str());
		CPPUNIT_ASSERT(inf.is_open());
		obj.Deserialize(inf, dir);
		std::ofstream of2(fileName2.c_str());
		obj.Serialize(of2, dir);

		// Compare with the initial config
		inf.close();
		of2.close();
		CPPUNIT_ASSERT(compareFiles(fileName2, fileName3));
	}


	void testSerialization1()
	{
		// TODO: There is a problem of inprecision with floating points. See how we handle this !
		TestObject obj1;
		testSerialization(obj1, "TestObject");

		Object obj2("test");
		obj2.AddFeature("feat1", 77);
		obj2.AddFeature("feat2", 3453.444e-123);
		testSerialization(obj2, "Object");

		Event evt1;
		testSerialization(evt1, "Event1");
		evt1.Raise("fff");
		testSerialization(evt1, "Event2");
		Event evt2;
		evt2.Raise("ggg", obj2);
		testSerialization(evt2, "Event3");

		cv::Mat image1(cv::Size(640, 480), CV_8U);
		StreamImage stream1("streamImage", image1, *mp_fakeInput, "A stream of image");
		testSerialization(stream1, "StreamImage");

		cv::Mat image2(cv::Size(640, 480), CV_8U);
		StreamDebug stream5("streamDebug", image2, *mp_fakeInput, "A stream of debug");
		testSerialization(stream5, "StreamDebug");

		/*
		StreamObject stream2;
		testSerialization(stream2, "streamObject");
		StreamState stream3;
		testSerialization(stream3, "streamState");
		StreamEvent stream4;
		testSerialization(stream4, "streamEvent");
		*/
		// TODO: test the serialization of modules
		MkException excep;
		testSerialization(excep, "MkException");

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
