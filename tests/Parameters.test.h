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

#include <cxxtest/TestSuite.h>
#include <fstream>

#include "Parameter.h"
#include "ParameterNum.h"
#include "ParameterString.h"
#include "ParameterImageType.h"
#include "ParameterSerializable.h"
#include "util.h"
#include "MkException.h"
#include "CalibrationByHeight.h"
#include "CalibrationByModel.h"
#include "Polygon.h"

using namespace std;

/// Unit testing class for ConfigReader class

class ParametersTestSuite : public CxxTest::TestSuite
{
protected:
	void testParameter(Parameter& xr_param, const string& x_legalValue, const string& x_illegalValue, const string& x_testRange)
	{
		xr_param.SetValue(x_legalValue, PARAMCONF_UNKNOWN);
		TS_ASSERT(xr_param.CheckRange() == true);
		xr_param.SetValue(x_illegalValue, PARAMCONF_UNKNOWN);
		TS_ASSERT(x_illegalValue == "" || xr_param.CheckRange() == false);
		TS_ASSERT(xr_param.GetConfigurationSource() == PARAMCONF_UNKNOWN);
		xr_param.SetDefault(x_legalValue);
		xr_param.SetValueToDefault();
		// cout << xr_param.GetValueString() << " == \n" <<  x_legalValue << endl;
		TS_ASSERT(xr_param.GetValueString() == x_legalValue);
		TS_ASSERT(xr_param.GetConfigurationSource() == PARAMCONF_DEF);
		xr_param.Lock();
		xr_param.LockIfRequired();
		TS_ASSERT(xr_param.IsLocked());

		string fileName = "tests/tmp/" + xr_param.GetName() + ".xml";
		ofstream of(fileName.c_str());
		xr_param.Export(of, 0);
		of.close();
		TS_ASSERT(compareFiles(fileName, "tests/parameters/" + xr_param.GetName() + ".xml"));

		fileName = "tests/tmp/" + xr_param.GetName() + ".txt";
		of.open(fileName.c_str());
		xr_param.Print(of);
		of.close();
		TS_ASSERT(compareFiles(fileName, "tests/parameters/" + xr_param.GetName() + ".txt"));

		// test that the current range stays identical
		string range = xr_param.GetRange();
		xr_param.SetRange(range);
		TS_ASSERT(xr_param.GetRange() == range);

		// test that the test range stays identical
		xr_param.SetRange(x_testRange);
		// cout<<xr_param.GetRange()<<endl;
		TS_ASSERT(xr_param.GetRange() == x_testRange);
	}

public:
	void setUp()
	{
	}
	void tearDown()
	{
	}
	/**
	* @brief Test all classes that inherit from Parameter
	*/
	void testParameterBool()
	{
		TS_TRACE("Test ParameterBool");
		bool myBool = true;
		ParameterBool paramBool("param_bool", false, 0, 1, &myBool, "Parameter of type bool");
		testParameter(paramBool, "1", "", "[0:0]");
		// testParameter(paramBool, "0", "", "[1:1]");
	}

	void testParameterInt()
	{
		TS_TRACE("Test ParameterInt");
		int myInt = 3456;
		ParameterInt paramInt("param_int", 12345, 32, 66000, &myInt, "Parameter of type int");
		testParameter(paramInt, "433", "-1234", "[555:789]");
	}

	void testParameterUInt()
	{
		TS_TRACE("Test ParameterUInt");
		unsigned int myInt = 3456;
		ParameterUInt paramUInt("param_uint", 12345, 32, 66000, &myInt, "Parameter of type unsigned int");
		testParameter(paramUInt, "433", "3", "[555:789]");
	}

	void testParameterFloat()
	{
		TS_TRACE("Test ParameterFloat");
		float myFloat = 2.133e44;
		ParameterFloat paramFloat("param_float", 234, - 10, 4.45e24, &myFloat, "Parameter of type float");
		testParameter(paramFloat, "-0.5", "-400", "[666:60000]");
	}

	void testParameterDouble()
	{
		TS_TRACE("Test ParameterDouble");
		double myDouble = 2.133e54;
		ParameterDouble paramDouble("param_double", 120e33, 1e33, 1e99, &myDouble, "Parameter of type double");
		testParameter(paramDouble, "1e+44", "-0", "[1e+44:1e+90]");
	}

	void testParameterString()
	{
		TS_TRACE("Test ParameterString");
		string myString = "value_current";
		ParameterString paramString("param_string", "default_value", &myString, "Parameter of type string");
		testParameter(paramString, "legal", "", "[]");
	}

	void testParameterImageType()
	{
		TS_TRACE("Test ParameterImageType");
		int myImageType = CV_8UC3;
		ParameterImageType paramImageType("param_imageType", CV_8UC1, &myImageType, "Parameter of type imageType");
		paramImageType.SetRange("[CV_8UC1,CV_8UC2,CV_8UC3]");
		testParameter(paramImageType, "CV_8UC2", "CV_32SC1", "[CV_32FC4]");
	}

	void testParameterCalibrationByHeight()
	{
		TS_TRACE("Test ParameterSerializable - CalibrationByHeight");
		CalibrationByHeight myCalibrationByHeight;
		ParameterSerializable paramCalibrationByHeight("param_calibrationByHeight",  "{\"height\":0.6,\"x\":0.3,\"y\":0.6}", &myCalibrationByHeight, "Parameter of type CalibrationByHeight");
		testParameter(paramCalibrationByHeight, "{\"height\":0.0,\"x\":1.0,\"y\":0.0}", "", "") ; // note: For now there is no range check ,"{\"x\":1, \"y\":0, \n \"height\":1.5}");
	}

	void testParameterCalibrationByModel()
	{
		TS_TRACE("Test ParameterSerializable - CalibrationByModel");
		string json_data = "{\"focal\":240.0,\"height\":2404.226076415452,\"ncols\":640,\"nrows\":480,\"roll\":137.4711820374162,\"yaw\":-1.203589252653426}";
		string json_data2 = "{\"focal\":910.0,\"height\":7000.52453240,\"ncols\":384,\"nrows\":287,\"roll\":-10.650,\"yaw\":-35.30}";

		CalibrationByModel myCalibrationByModel;
		ParameterSerializable paramCalibrationByModel("param_calibrationByModel",  json_data2, &myCalibrationByModel, "Parameter of type CalibrationByModel");
		testParameter(paramCalibrationByModel, json_data, "", "") ;
	}

	void testParameterPolygon()
	{
		TS_TRACE("Test ParameterSerializable - Polygon");
		Polygon myPolygon;
		ParameterSerializable paramPolygon("param_polygon",  "{\"height\":0.6,\"points\":[{{\"x\":5.0,\"y\":0.50},{\"x\":6.0,\"y\":5.50}}],\"width\":0.8}", &myPolygon, "Parameter of type Polygon");
		testParameter(paramPolygon, "{\"height\":0.660,\"points\":[{\"x\":54.0,\"y\":53.50},{\"x\":3454.0,\"y\":53.50},{\"x\":54.0,\"y\":53.50},{\"x\":5.0,\"y\":0.50}],\"width\":0.860}", "", "") ;
	}
};
#endif
