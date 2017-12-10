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
#include "ParameterEnumT.h"
#include "ParameterT.h"
#include "util.h"
#include "enums.h"
#include "MkException.h"
#include "Polygon.h"

using namespace std;
using namespace mk;

/// Unit testing class for mkconf class

class ParametersTestSuite : public CxxTest::TestSuite
{
public:
	static void testParameter(Parameter& xr_param, const mkjson& x_legalValue, const mkjson& x_illegalValue)
	{
		TS_TRACE("Test parameter " + xr_param.GetName());
		xr_param.SetValue(x_legalValue, PARAMCONF_UNKNOWN);
		TS_ASSERT(xr_param.CheckRange() == true);
		if(!x_illegalValue.is_null())
		{
			xr_param.SetValue(x_illegalValue, PARAMCONF_UNKNOWN);
			TS_ASSERT(xr_param.CheckRange() == false);
		}
		xr_param.SetDefaultAndValue(x_legalValue);
		xr_param.SetDefaultAndValue(x_legalValue);
		xr_param.SetDefaultAndValue(x_legalValue);
		xr_param.SetDefaultAndValue(x_legalValue);
		xr_param.SetDefaultAndValue(x_legalValue);
		xr_param.SetValueToDefault();
		// TS_ASSERT(xr_param.GetValue().compare(x_legalValue));
		TS_ASSERT(oneLine(xr_param.GetValue()) == oneLine(x_legalValue));
		// TS_ASSERT(xr_param.GetValue() == x_legalValue); // does not work apparently
		TS_ASSERT(xr_param.GetConfigurationSource() == PARAMCONF_DEF);

		string fileName = "tests/tmp/" + xr_param.GetName() + ".json";
		ofstream of(fileName.c_str());
		mkjson js = xr_param.Export();
		of << multiLine(js);
		of.close();
		TS_ASSERT(compareJsonFiles(fileName, "tests/parameters/" + xr_param.GetName() + ".json"));

		fileName = "tests/tmp/" + xr_param.GetName() + ".txt";
		of.open(fileName.c_str());
		xr_param.Print(of);
		of.close();
		TS_ASSERT(compareFiles(fileName, "tests/parameters/" + xr_param.GetName() + ".txt"));

		// test that the current range stays identical
		mkjson range = xr_param.GetRange();
		xr_param.SetRange(range);
		// cout << oneLine(range).size() << " == " << oneLine(xr_param.GetRange()).compare(oneLine(range)) << endl;
		TS_ASSERT(xr_param.GetRange() == range);
	}

	static void testLock(Parameter& xr_param)
	{
		xr_param.Lock();
		xr_param.LockIfRequired();
		TS_ASSERT(xr_param.IsLocked());
	}
	
	static void testRange(Parameter& xr_param, const mkjson& x_illegalValue, const mkjson& x_testRange)
	{
		// test that the test range stays identical
		xr_param.SetRange(x_testRange);
		const mkjson& json(xr_param.GetRange());
		xr_param.SetRange(json);
		// cout<<xr_param.GetRange()<<endl;
		TS_ASSERT(xr_param.GetRange() == json);

		if(x_illegalValue.is_null())
			return;
		xr_param.SetValue(x_illegalValue, PARAMCONF_JSON);
		TS_ASSERT(!xr_param.CheckRange());
	}

	void setUp()
	{
	}
	void tearDown()
	{
	}

	void testJson()
	{
		TS_ASSERT(mkjson("mystring").get<string>() == "mystring");
		TS_ASSERT(mkjson(34).get<int>() == 34);
		TS_ASSERT(mkjson(34.1).get<float>() - 34.1 < 0.0001);
		TS_ASSERT(mkjson(true).get<bool>() == true);
		TS_ASSERT(mkjson(false).get<bool>() == false);
	}
	/**
	* @brief Test all classes that inherit from Parameter
	*/
	void testParameterBool()
	{
		TS_TRACE("Test ParameterBool");
		bool myBool = true;
		ParameterBool paramBool("param_bool", false, &myBool, "Parameter of type bool");
		testParameter(paramBool, false, nullptr);
		testRange(paramBool, true, R"({"max":false})"_json);
		testLock(paramBool);
	}

	void testParameterInt()
	{
		TS_TRACE("Test ParameterInt");
		int myInt = 3456;
		ParameterInt paramInt("param_int", 12345, 32, 66000, &myInt, "Parameter of type int");
		testParameter(paramInt, 635, -1234);
		testRange(paramInt, 4, R"({"min":555, "max":789})"_json);
		testLock(paramInt);
	}

	void testParameterUInt()
	{
		TS_TRACE("Test ParameterUInt");
		unsigned int myInt = 3456;
		ParameterUInt paramUInt("param_uint", 12345, 32, 66000, &myInt, "Parameter of type unsigned int");
		testParameter(paramUInt, 633, 3);
		testRange(paramUInt, 10000, R"({"min": 555, "max":789})"_json);
		testLock(paramUInt);
	}

	void testParameterFloat()
	{
		TS_TRACE("Test ParameterFloat");
		float myFloat = 2.133e44;
		ParameterFloat paramFloat("param_float", 234, - 10, 4.45e24, &myFloat, "Parameter of type float");
		testParameter(paramFloat, -0.5, -400);
		testRange(paramFloat, -0.5, R"({"min":666, "max":60000})"_json);
		testLock(paramFloat);
	}

	void testParameterDouble()
	{
		TS_TRACE("Test ParameterDouble");
		double myDouble = 2.133e54;
		ParameterDouble paramDouble("param_double", 120e33, 1e33, 1e99, &myDouble, "Parameter of type double");
		testParameter(paramDouble, 1e+44, -0);
		testRange(paramDouble, 1e+43, R"({"min":1e+44, "max":1e+90})"_json);
		testLock(paramDouble);
	}

	void testParameterString()
	{
		TS_TRACE("Test ParameterString");
		string myString = "value_current";
		ParameterString paramString("param_string", "default_value", &myString, "Parameter of type string");
		testParameter(paramString, "legal", nullptr);
		testRange(paramString, nullptr, "{}"_json);
		testLock(paramString);
	}

	void testParameterImageType()
	{
		TS_TRACE("Test ParameterImageType");
		int myImageType = CV_8UC3;
		ParameterEnumT<mk::ImageType> paramImageType("param_imageType", CV_8UC1, &myImageType, "Parameter of type imageType");
		paramImageType.SetRange(R"({"allowed":["CV_8UC1","CV_8UC2","CV_8UC3"]})"_json);
		testParameter(paramImageType, "CV_8UC2", "CV_32SC1");
		testRange(paramImageType, "CV_8UC2", R"({"allowed":["CV_32FC4"]})"_json);
		testLock(paramImageType);
	}

	void testParameterPolygon()
	{
		TS_TRACE("Test Parameter - Polygon");
		Polygon myPolygon;
		ParameterT<Polygon> paramPolygon("param_polygon",  R"({"height":0.6,"points":[{"x":5.0,"y":0.50},{"x":6.0,"y":5.50}],"width":0.8})"_json, &myPolygon, "Parameter of type Polygon");
		testParameter(paramPolygon, R"({"height":0.660,"points":[{"x":54.0,"y":53.50},{"x":3454.0,"y":53.50},{"x":54.0,"y":53.50},{"x":5.0,"y":0.50}],"width":0.860})"_json, nullptr) ;
		testLock(paramPolygon);
	}


	//TODO Test for parameters issue of streams
};
#endif
