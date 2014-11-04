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
#ifndef PARAMETERS_TEST_H
#define PARAMETERS_TEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include "Parameter.h"
#include "ParameterSerializable.h"
#include "MkException.h"


/// Unit testing class for ConfigReader class

class TestParameters : public CppUnit::TestFixture
{
	private:
		static log4cxx::LoggerPtr m_logger;
	protected:
		std::vector<std::string> moduleTypes;
		// FactoryModules m_factory;
		ConfigReader* mp_config;

		// Objects for streams
		cv::Mat m_image; // (module->GetHeight(), module->GetWidth(), module->GetType());
		bool m_state;
		Event m_event;
		std::vector<Object> m_objects;
		int m_cpt;
	public:
	void runTest()
	{
	}
	void setUp()
	{
	}
	void tearDown()
	{
	}

	void testParameter(Parameter& xr_param, const std::string& x_legalValue, const std::string& x_illegalValue, const std::string& x_testRange)
	{
		xr_param.SetValue(x_legalValue, PARAMCONF_UNKNOWN);
		CPPUNIT_ASSERT(xr_param.CheckRange() == true);
		xr_param.SetValue(x_illegalValue, PARAMCONF_UNKNOWN);
		CPPUNIT_ASSERT(x_illegalValue == "" || xr_param.CheckRange() == false);
		CPPUNIT_ASSERT(xr_param.GetConfigurationSource() == PARAMCONF_UNKNOWN);
		xr_param.SetDefault(x_legalValue);
		xr_param.SetValueToDefault();
		CPPUNIT_ASSERT_MESSAGE(xr_param.GetValueString() + " != " + x_legalValue, xr_param.GetValueString() == x_legalValue);
		CPPUNIT_ASSERT(xr_param.GetConfigurationSource() == PARAMCONF_DEF);
		xr_param.Lock();
		CPPUNIT_ASSERT(xr_param.IsLocked());

		std::string fileName = "testing/tmp/" + xr_param.GetName() + ".xml";
		std::ofstream of(fileName.c_str());
		xr_param.Export(of, 0);
		of.close();
		CPPUNIT_ASSERT(compareFiles(fileName, "testing/parameters/" + xr_param.GetName() + ".xml"));

		fileName = "testing/tmp/" + xr_param.GetName() + ".txt";
		of.open(fileName.c_str());
		xr_param.Print(of);
		of.close();
		CPPUNIT_ASSERT(compareFiles(fileName, "testing/parameters/" + xr_param.GetName() + ".txt"));

		// test that the current range stays identical
		std::string range = xr_param.GetRange();
		xr_param.SetRange(range);
		CPPUNIT_ASSERT(xr_param.GetRange() == range);

		// test that the test range stays identical
		xr_param.SetRange(x_testRange);
		// std::cout<<xr_param.GetRange()<<std::endl;
		CPPUNIT_ASSERT(xr_param.GetRange() == x_testRange);
	}

	/**
	* @brief Test all classes that inherit from Parameter
	*/
	void testParametersByType()
	{
		LOG_TEST(m_logger, "Test ParameterBool");
		bool myBool = true;
		ParameterBool paramBool("param_bool", false, 0, 1, &myBool, "Parameter of type bool");
		testParameter(paramBool, "1", "", "[0:0]");
		// testParameter(paramBool, "0", "", "[1:1]");

		LOG_TEST(m_logger, "Test ParameterInt");
		int myInt = 3456;
		ParameterInt paramInt("param_int", 12345, 32, 66000, &myInt, "Parameter of type int");
		testParameter(paramInt, "433", "-1234", "[555:789]");

		LOG_TEST(m_logger, "Test ParameterFloat");
		float myFloat = 2.133e44;
		ParameterFloat paramFloat("param_float", 234, - 10, 4.45e24, &myFloat, "Parameter of type float");
		testParameter(paramFloat, "-0.5", "-400", "[666:60000]");

		LOG_TEST(m_logger, "Test ParameterDouble");
		double myDouble = 2.133e54;
		ParameterDouble paramDouble("param_double", 120e33, 1e33, 1e99, &myDouble, "Parameter of type double");
		testParameter(paramDouble, "1e+44", "-0", "[1e+44:1e+90]");

		LOG_TEST(m_logger, "Test ParameterString");
		std::string myString = "value_current";
		ParameterString paramString("param_string", "default_value", &myString, "Parameter of type string");
		testParameter(paramString, "legal", "", "");

		LOG_TEST(m_logger, "Test ParameterImageType");
		int myImageType = CV_8UC3;
		ParameterImageType paramImageType("param_imageType", CV_8UC1, &myImageType, "Parameter of type imageType");
		paramImageType.SetRange("[CV_8UC1,CV_8UC2,CV_8UC3]");
		testParameter(paramImageType, "CV_8UC2", "CV_32SC1", "[CV_32FC4]");

		LOG_TEST(m_logger, "Test ParameterSerializable - CalibrationByHeight");
		CalibrationByHeight myCalibrationByHeight;
		ParameterSerializable paramCalibrationByHeight("param_calibrationByHeight",  "{\"height\":0.6,\"x\":0.3,\"y\":0.6}", &myCalibrationByHeight, "Parameter of type CalibrationByHeight");
		testParameter(paramCalibrationByHeight, "{\"height\":0.0,\"x\":1.0,\"y\":0.0}", "", "") ; // TODO: For now there is no range check ,"{\"x\":1, \"y\":0, \n \"height\":1.5}");


		LOG_TEST(m_logger, "Test ParameterSerializable - CalibrationByModel");
		std::string json_data = "{\"camera_height\":2404.226076415452,\"focal\":240.0,\"height_model\":480,\"roll\":137.4711820374162,\"width_model\":640,\"yaw\":-1.203589252653426}";
		std::string json_data2 = "{\"camera_height\":7000.52453240,\"focal\":910.0,\"height_model\":287,\"roll\":-10.650,\"width_model\":384,\"yaw\":-35.30}";

		CalibrationByModel myCalibrationByModel;
		ParameterSerializable paramCalibrationByModel("param_calibrationByModel",  json_data2, &myCalibrationByModel, "Parameter of type CalibrationByModel");
		testParameter(paramCalibrationByModel, json_data,"", "") ;
		// TODO: Tests calibration model ?
	}

	static CppUnit::Test *suite()
	{
		CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite("TestParameters");
		suiteOfTests->addTest(new CppUnit::TestCaller<TestParameters>("testParametersByType", &TestParameters::testParametersByType));
		return suiteOfTests;
	}
};
log4cxx::LoggerPtr TestParameters::m_logger(log4cxx::Logger::getLogger("TestParameters"));
#endif
