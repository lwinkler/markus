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
#ifndef TEST_STREAMS_H
#define TEST_STREAMS_H

#include <cxxtest/TestSuite.h>
#include <sstream>
#include <fstream>

#include "util.h"

#include "StreamImage.h"
#include "StreamObject.h"
#include "StreamDebug.h"
#include "StreamState.h"
#include "StreamEvent.h"
#include "StreamT.h"
#include "MultiStreamT.h"


using namespace std;


/// Unit testing class for serializable classes


class StreamsTestSuite : public CxxTest::TestSuite
{
	class FakeModule : public Module
	{
	public:
		FakeModule(ParameterStructure& xr_params) :
		Module(xr_params),
		m_param(dynamic_cast<Parameters&>(xr_params))
		{
			AddInputStream( 0, new StreamObject("object", m_obj, *this, "Test stream"));
			AddInputStream( 1, new StreamEvent("event", m_evt, *this, "Test stream"));
			AddInputStream( 2, new StreamImage("image", m_image, *this, "Test stream"));
			AddInputStream( 3, new StreamNum<bool>("bool", m_bool, *this, "Test stream"));
			AddInputStream( 4, new StreamNum<int>("int", m_int, *this, "Test stream"));
			AddInputStream( 5, new StreamNum<uint>("uint", m_uint, *this, "Test stream"));
			AddInputStream( 6, new StreamNum<float>("float", m_float, *this, "Test stream"));
			AddInputStream( 7, new StreamNum<double>("double", m_double, *this, "Test stream"));

			AddInputStream( 0, new MultiStreamT<vector<Object>>("object", m_objs, *this, "Test stream"));
			AddInputStream( 1, new MultiStreamT<Event>("event", m_evts, *this, "Test stream"));
			AddInputStream( 2, new MultiStreamT<cv::Mat>("image", m_images, *this, "Test stream"));
			// AddInputStream( 3, new MultiStreamT<bool>("bool", m_bools, *this, "Test stream"));
			// AddInputStream( 4, new MultiStreamT<int>("int", m_ints, *this, "Test stream"));
			// AddInputStream( 5, new MultiStreamT<uint>("uint", m_uints, *this, "Test stream"));
			// AddInputStream( 6, new MultiStreamT<float>("float", m_floats, *this, "Test stream"));
			// AddInputStream( 7, new MultiStreamT<double>("double", m_doubles, *this, "Test stream"));

			AddOutputStream( 0, new StreamObject("object", m_obj, *this, "Test stream"));
			AddOutputStream( 1, new StreamEvent("event", m_evt, *this, "Test stream"));
			AddOutputStream( 2, new StreamImage("image", m_image, *this, "Test stream"));
			AddOutputStream( 3, new StreamNum<bool>("bool", m_bool, *this, "Test stream"));
			AddOutputStream( 4, new StreamNum<int>("int", m_int, *this, "Test stream"));
			AddOutputStream( 5, new StreamNum<uint>("uint", m_uint, *this, "Test stream"));
			AddOutputStream( 6, new StreamNum<float>("float", m_float, *this, "Test stream"));
			AddOutputStream( 7, new StreamNum<double>("double", m_double, *this, "Test stream"));

			AddOutputStream( 0, new MultiStreamT<vector<Object>>("object", m_objs, *this, "Test stream"));
			AddOutputStream( 1, new MultiStreamT<Event>("event", m_evts, *this, "Test stream"));
			AddOutputStream( 2, new MultiStreamT<cv::Mat>("image", m_images, *this, "Test stream"));
			// AddOutputStream( 3, new MultiStreamT<bool>("bool", m_bools, *this, "Test stream"));
			// AddOutputStream( 4, new MultiStreamT<int>("int", m_ints, *this, "Test stream"));
			// AddOutputStream( 5, new MultiStreamT<uint>("uint", m_uints, *this, "Test stream"));
			// AddOutputStream( 6, new MultiStreamT<float>("float", m_floats, *this, "Test stream"));
			// AddOutputStream( 7, new MultiStreamT<double>("double", m_doubles, *this, "Test stream"));
		}

		~FakeModule(){}
		MKCLASS("FakeModule")
		MKDESCR("Output video stream with additional object streams")
		void Reset(){}
		virtual void ProcessFrame(){}

	private:
		const ParameterStructure& m_param;

		vector<Object>m_obj;
		Event         m_evt;
		cv::Mat       m_image;
		bool          m_bool;
		int           m_int;
		uint          m_uint;
		float         m_float;
		double        m_double;

		vector<vector<Object>> m_objs;
		vector<Event>        m_evts;
		vector<cv::Mat>      m_images;
		vector<bool>         m_bools;
		vector<int>          m_ints;
		vector<uint>         m_uints;
		vector<float>        m_floats;
		vector<double>       m_doubles;
	};

public:
	StreamsTestSuite() :
		m_configFile("tests/streams/config.xml"),
		m_config(m_configFile),
		m_fakeParams(m_config.FindRef("application>module[name=\"FakeModule\"]", true)),
		m_fakeModule(m_fakeParams)
		{}

protected:
	ConfigFile m_configFile;
	ConfigReader        m_config;
	ParameterStructure  m_fakeParams;
	FakeModule          m_fakeModule;

public:
	void setUp()
	{
	}
	void tearDown()
	{
	}

public:
	void testCreation()
	{
	}
};
#endif
