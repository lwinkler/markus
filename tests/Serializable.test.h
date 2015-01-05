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

#include <cxxtest/TestSuite.h>
#include <sstream>

#include "util.h"
#include "MkException.h"
#include "Serializable.h"

#include "StreamImage.h"
#include "StreamObject.h"
#include "StreamDebug.h"
#include "StreamState.h"
#include "StreamEvent.h"
#include "CalibrationByHeight.h"
#include "CalibrationByModel.h"
#include "Polygon.h"
#include "Event.h"

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
	virtual void Serialize(ostream& x_out, const string& x_dir) const
	{
		Json::Value root;
		root["int1"] = m_int;
		root["float1"] = m_float;
		root["double1"] = m_double;
		root["string1"] = m_string;
		stringstream ss;
		m_obj.Serialize(ss, x_dir);
		ss >> root["object"];
		x_out << root;
	}
	virtual void Deserialize(istream& x_in, const string& x_dir)
	{
		Json::Value root;
		x_in >> root;
		m_int = root["int1"].asInt();
		m_float = root["float1"].asFloat();
		m_double = root["double1"].asDouble();
		m_string = root["string1"].asString();
		stringstream ss;
		ss << root["object"];
		m_obj.Deserialize(ss, x_dir);
	}
	
protected:
	int m_int;
	float m_float;
	double m_double;
	string m_string;
	Object m_obj;
};

/// Unit testing class for serializable classes


class SerializableTestSuite : public CxxTest::TestSuite
{
public:
	SerializableTestSuite() 
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

	// Values for testing
	cv::KeyPoint  m_kp;
	cv::Point2f   m_pt2f;
	cv::Point3f   m_pt3f;
	vector<float> m_vect;
	Object        m_obj2;
	Event m_evt2;

public:
	void setUp()
	{
		createEmptyConfigFile("/tmp/config_empty.xml");
		mp_config = new ConfigReader("tests/serialize/module.xml");
		mp_fakeInput = m_factoryModules.Create("VideoFileReader", mp_config->GetSubConfig("module"));
		// note: we need a fake module to create the input streams
		mp_fakeInput->SetAsReady();
		mp_fakeInput->Reset();

		m_vect.push_back(33.66);
		m_vect.push_back(1e4);
		m_vect.push_back(0.000234);

		m_pt2f = cv::Point2f(33.5, 1e-4);
		m_pt3f = cv::Point3f(33, 134.5, 1e4);

		m_obj2 = Object("test");
		m_obj2.AddFeature("feat1", 77);
		m_obj2.AddFeature("feat2", 3453.444e-123);
		m_obj2.AddFeature("ff", new FeatureFloat(0.93));
		m_obj2.AddFeature("ff", new FeatureInt(93));
		m_obj2.AddFeature("ffif", new FeatureFloatInTime(0.132));
		m_obj2.AddFeature("fstr", new FeatureString("someString"));
		m_obj2.AddFeature("fvf", new FeatureVectorFloat(m_vect));
		m_obj2.AddFeature("fkp", new FeatureKeyPoint(m_kp));
		m_obj2.AddFeature("fp2f", new FeaturePoint2f(m_pt2f));
		m_obj2.AddFeature("fp3f", new FeaturePoint3f(m_pt3f));

		m_evt2.Raise("name2", m_obj2);
	}
	void tearDown()
	{
		CLEAN_DELETE(mp_fakeInput);
		CLEAN_DELETE(mp_config);
	}

protected:
	/// Test the serialization of one serializable class
	void testSerialization(Serializable& obj, const string& name)
	{
		string fileName1 = "tests/tmp/serialize1_" + name + ".json";
		string fileName2 = "tests/tmp/serialize2_" + name + ".json";
		string fileName3 = "tests/serialize/" + name + ".json";
		string dir = "tests/serialize/image";
		SYSTEM("mkdir -p " + dir);
		LOG_TEST(m_logger, "Test serialization of "<<name<<" = "<<obj.SerializeToString() << " with signature = " << obj.Signature());
		ofstream of1(fileName1.c_str());
		obj.Serialize(of1, dir);
		of1.close();

		// stringstream ss2;

		ifstream inf(fileName3.c_str());
		TS_ASSERT(inf.is_open());
		obj.Deserialize(inf, dir);
		ofstream of2(fileName2.c_str());
		obj.Serialize(of2, dir);

		// Compare with the initial config
		inf.close();
		of2.close();
		TS_ASSERT(compareFiles(fileName2, fileName3));
	}

public:
	void testTestObject()
	{

		// TODO: There is a problem of inprecision with floating points. See how we handle this !
		TestObject obj1;
		testSerialization(obj1, "TestObject");
	}

	void testObject()
	{
		testSerialization(m_obj2, "Object");
	}

	void testEvent()
	{
		Event evt1;
		testSerialization(evt1, "Event1");
		evt1.Raise("name1");
		testSerialization(evt1, "Event2");
		testSerialization(m_evt2, "Event3");
	}

	void testStreamImage()
	{
		cv::Mat image1(cv::Size(640, 480), CV_8U);
		StreamImage stream1("streamImage", image1, *mp_fakeInput, "A stream of image");
		testSerialization(stream1, "StreamImage");
	}

	void testStreamObject()
	{
		vector<Object> objects;
		StreamObject stream2("streamObject", objects, *mp_fakeInput, "A stream of objects");
		stream2.AddObject(m_obj2);
		testSerialization(stream2, "StreamObject");
	}

	void testStreamState()
	{
		bool state;
		StreamState stream3("streamState", state, *mp_fakeInput, "A stream of state");
		testSerialization(stream3, "StreamState");
	}

	void testStreamEvent()
	{
		StreamEvent stream4("streamEvent", m_evt2, *mp_fakeInput, "A stream of event");
		testSerialization(stream4, "StreamEvent");
	}

	void testStreamDebug()
	{
#ifdef MARKUS_DEBUG_STREAMS
		cv::Mat image2(cv::Size(640, 480), CV_8U);
		StreamDebug stream5("streamDebug", image2, *mp_fakeInput, "A stream of debug");
		testSerialization(stream5, "StreamDebug");
#endif
	}

	void testMkException()
	{
		MkException excep("test", LOC);
		testSerialization(excep, "MkException");
	}

	void testCalibrationByHeight()
	{
		CalibrationByHeight calib1;
		testSerialization(calib1, "CalibrationByHeight1");

		CalibrationByHeight calib2(0.33, 0.6, 0.25);
		testSerialization(calib2, "CalibrationByHeight2");
	}
		
	void testCalibrationByModel()
	{
		CalibrationByModel calibModel1;
		testSerialization(calibModel1,"CalibrationByModel1");
		
		CalibrationByModel calibModel2(2404.2260764154521, -1.2035892526534258, 137.47118203741616,240.0, 480 , 640);
		testSerialization(calibModel2,"CalibrationByModel2");
	}

	void testPolygon()
	{
		vector<cv::Point2f> pts;
		pts.push_back(cv::Point2f(34.4, 34));
		pts.push_back(cv::Point2f(3.4, -455.5));
		pts.push_back(cv::Point2f(344, 0.34));
		Polygon polygon1(pts);
		testSerialization(polygon1,"Polygon");
	}

	void testModule()
	{
		// TODO: test the serialization of all modules
		testSerialization(*mp_fakeInput, "Module");
	}

	void testFeatureFloat()
	{
		// Serialize all features
		FeatureFloat ff(0.93);
		testSerialization(ff, "FeatureFloat");
	}

	void testFeatureFloatInTime()
	{
		FeatureFloatInTime ffit(0.93);
		testSerialization(ffit, "FeatureFloatInTime");
	}

	void testFeatureString()
	{
		FeatureString fstr("mystring");
		testSerialization(fstr, "FeatureString");
	}

	void testFeatureVectorFloat()
	{
		FeatureVectorFloat fvf(m_vect);
		testSerialization(fvf, "FeatureVectorFloat");
	}

	void testFeatureKeyPoint()
	{
		FeatureKeyPoint fkp(m_kp);
		testSerialization(fkp, "FeatureKeyPoint");
	}

	void testFeaturePoint2f()
	{
		FeaturePoint2f fpt2f(m_pt2f);
		testSerialization(fpt2f, "FeaturePoint2f");
	}

	void testFeaturePoint3f()
	{
		FeaturePoint3f fpt3f(m_pt3f);
		testSerialization(fpt3f, "FeaturePoint3f");
	}

	void testFeatures()
	{
		vector<string> listFeatures;
		m_factoryFeatures.List(listFeatures);
		unsigned int seed = 242343332;
		for(vector<string>::const_iterator it = listFeatures.begin() ; it != listFeatures.end() ; ++it)
		{
			Feature* feat = m_factoryFeatures.Create(*it);
			feat->Randomize(seed, "");
			testSerialization(*feat, *it);
			delete(feat);
		}
	}
};
log4cxx::LoggerPtr SerializableTestSuite::m_logger(log4cxx::Logger::getLogger("SerializableTestSuite"));
#endif