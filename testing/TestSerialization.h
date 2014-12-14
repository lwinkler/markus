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
#include "CalibrationByHeight.h"
#include "CalibrationByModel.h"
#include "Polygon.h"

#include "FeatureStd.h"
#include "FeatureFloatInTime.h"
#include "FeatureVector.h"
#include "FeatureOpenCv.h"

/// Test class for serialization
class TestObject : public Serializable
{
public:
	TestObject()
	: m_obj("test object"),
	  m_string("test_string")
	{
		m_int    = 333;
		m_float  = 55.1233234;
		m_double = 34.444;

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
public:
	TestSerialization() 
	 : mp_fakeInput(NULL),
	 m_factoryModules(Factories::modulesFactory()),
	 m_factoryFeatures(Factories::featuresFactory()),
	 mp_config(NULL){}

private:
		static log4cxx::LoggerPtr m_logger;
		const FactoryModules& m_factoryModules;
		const FactoryFeatures& m_factoryFeatures;
		Module* mp_fakeInput;
		ConfigReader* mp_config;

public:
	void setUp()
	{
		createEmptyConfigFile("/tmp/config_empty.xml");
		mp_config = new ConfigReader("testing/serialize/module.xml");
		mp_fakeInput = m_factoryModules.Create("VideoFileReader", mp_config->GetSubConfig("module"));
		// note: we need a fake module to create the input streams
		mp_fakeInput->SetAsReady();
		mp_fakeInput->Reset();
	}
	void tearDown()
	{
		CLEAN_DELETE(mp_fakeInput);
		CLEAN_DELETE(mp_config);
	}

	/// Test the serialization of one serializable class
	void testSerialization(Serializable& obj, const std::string& name)
	{
		std::string fileName1 = "testing/tmp/serialize1_" + name + ".json";
		std::string fileName2 = "testing/tmp/serialize2_" + name + ".json";
		std::string fileName3 = "testing/serialize/" + name + ".json";
		std::string dir = "testing/serialize/image";
		SYSTEM("mkdir -p " + dir);
		LOG_TEST(m_logger, "Test serialization of "<<name<<" = "<<obj.SerializeToString() << " with signature = " << obj.Signature());
		std::ofstream of1(fileName1.c_str());
		obj.Serialize(of1, dir);
		of1.close();

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
		// Values for testing
		std::vector<float> myVect;
		myVect.push_back(33.66);
		myVect.push_back(1e4);
		myVect.push_back(0.000234);
		cv::KeyPoint kp;
		cv::Point2f pt2f(33.5, 1e-4);
		cv::Point3f pt3f(33, 134.5, 1e4);

		// TODO: There is a problem of inprecision with floating points. See how we handle this !
		TestObject obj1;
		testSerialization(obj1, "TestObject");


		Object obj2("test");
		obj2.AddFeature("feat1", 77);
		obj2.AddFeature("feat2", 3453.444e-123);
		obj2.AddFeature("ff", new FeatureFloat(0.93));
		obj2.AddFeature("ff", new FeatureInt(93));
		obj2.AddFeature("ffif", new FeatureFloatInTime(0.132));
		obj2.AddFeature("fstr", new FeatureString("someString"));
		obj2.AddFeature("fvf", new FeatureVectorFloat(myVect));
		obj2.AddFeature("fkp", new FeatureKeyPoint(kp));
		obj2.AddFeature("fp2f", new FeaturePoint2f(pt2f));
		obj2.AddFeature("fp3f", new FeaturePoint3f(pt3f));
		testSerialization(obj2, "Object");

		Event evt1;
		testSerialization(evt1, "Event1");
		evt1.Raise("name1");
		testSerialization(evt1, "Event2");
		Event evt2;
		evt2.Raise("name2", obj2);
		testSerialization(evt2, "Event3");

		cv::Mat image1(cv::Size(640, 480), CV_8U);
		StreamImage stream1("streamImage", image1, *mp_fakeInput, "A stream of image");
		testSerialization(stream1, "StreamImage");

		std::vector<Object> objects;
		StreamObject stream2("streamObject", objects, *mp_fakeInput, "A stream of objects");
		stream2.AddObject(obj2);
		testSerialization(stream2, "StreamObject");

		bool state;
		StreamState stream3("streamState", state, *mp_fakeInput, "A stream of state");
		testSerialization(stream3, "StreamState");

		StreamEvent stream4("streamEvent", evt2, *mp_fakeInput, "A stream of event");
		testSerialization(stream4, "StreamEvent");

#ifdef MARKUS_DEBUG_STREAMS
		cv::Mat image2(cv::Size(640, 480), CV_8U);
		StreamDebug stream5("streamDebug", image2, *mp_fakeInput, "A stream of debug");
		testSerialization(stream5, "StreamDebug");
#endif

		MkException excep("test", LOC);
		testSerialization(excep, "MkException");

		CalibrationByHeight calib1;
		testSerialization(calib1, "CalibrationByHeight1");

		CalibrationByHeight calib2(0.33, 0.6, 0.25);
		testSerialization(calib2, "CalibrationByHeight2");
		
		CalibrationByModel calibModel1;
		testSerialization(calibModel1,"CalibrationByModel1");
		
		CalibrationByModel calibModel2(2404.2260764154521, -1.2035892526534258, 137.47118203741616,240.0, 480 , 640);
		testSerialization(calibModel2,"CalibrationByModel2");

		std::vector<cv::Point2f> pts;
		pts.push_back(cv::Point2f(34.4, 34));
		pts.push_back(cv::Point2f(3.4, -455.5));
		pts.push_back(cv::Point2f(344, 0.34));
		Polygon polygon1(pts);
		testSerialization(polygon1,"Polygon");

		// TODO: test the serialization of all modules
		testSerialization(*mp_fakeInput, "Module");

		// Serialize all features
		FeatureFloat ff(0.93);
		testSerialization(ff, "FeatureFloat");

		FeatureFloatInTime ffit(0.93);
		testSerialization(ffit, "FeatureFloatInTime");

		FeatureString fstr("mystring");
		testSerialization(fstr, "FeatureString");

		FeatureVectorFloat fvf(myVect);
		testSerialization(fvf, "FeatureVectorFloat");

		FeatureKeyPoint fkp(kp);
		testSerialization(fkp, "FeatureKeyPoint");

		FeaturePoint2f fpt2f(pt2f);
		testSerialization(fpt2f, "FeaturePoint2f");

		FeaturePoint3f fpt3f(pt3f);
		testSerialization(fpt3f, "FeaturePoint3f");

		std::vector<std::string> listFeatures;
		m_factoryFeatures.ListFeatures(listFeatures);
		unsigned int seed = 242343332;
		for(std::vector<std::string>::const_iterator it = listFeatures.begin() ; it != listFeatures.end() ; ++it)
		{
			Feature* feat = m_factoryFeatures.CreateFeature(*it);
			feat->Randomize(seed, "");
			testSerialization(*feat, *it);
			delete(feat);
		}
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
