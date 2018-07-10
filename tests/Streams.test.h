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

#include "Parameters.test.h"
#include "StreamImage.h"
#include "StreamObject.h"
#include "StreamDebug.h"
#include "StreamState.h"
#include "StreamEvent.h"
#include "StreamT.h"
#include "StreamNum.h"
#include "MultiStreamT.h"


using namespace std;


/// Unit testing class for serializable classes


class StreamsTestSuite : public CxxTest::TestSuite
{
	class FakeModule : public Module
	{
	public:
		MKCATEG("Fake")
		explicit FakeModule(ParameterStructure& xr_params) :
			Module(xr_params),
			m_param(dynamic_cast<Parameters&>(xr_params)),
			m_image(cv::Size(m_param.width, m_param.height), m_param.type)
		{
			m_objs.resize(10);
			m_evts.resize(14);
			m_images.resize(9);

			for(auto& im : m_images)
			{
				im = cv::Mat(cv::Size(m_param.width, m_param.height), m_param.type);
			}

			TS_ASSERT(GetWidth() * GetHeight() > 0);
			AddInputStream( 0, new StreamObject("stream_object0", m_obj, *this, "Test stream"));
			AddInputStream( 1, new StreamEvent("stream_event1", m_evt, *this, "Test stream"));
			AddInputStream( 2, new StreamImage("stream_image2", m_image, *this, "Test stream"));
			AddInputStream( 3, new StreamNum<bool>("stream_bool3", m_bool, *this, "Test stream"));
			AddInputStream( 4, new StreamNum<int>("stream_int4", m_int, *this, "Test stream"));
			AddInputStream( 5, new StreamNum<uint>("stream_uint5", m_uint, *this, "Test stream"));
			AddInputStream( 6, new StreamNum<float>("stream_float6", m_float, *this, "Test stream"));
			AddInputStream( 7, new StreamNum<double>("stream_double7", m_double, *this, "Test stream"));

			AddInputStream( 8, new MultiStreamT<vector<Object>>("stream_object8", m_objs, *this, "Test stream"));
			// AddInputStream( 9, new MultiStreamT<Event>("stream_event9", m_evts, *this, "Test stream"));
			// AddInputStream(10, new MultiStreamT<cv::Mat>("stream_image10", m_images, *this, "Test stream"));
			// AddInputStream(11, new MultiStreamT<bool>("stream_bool11", m_bools, *this, "Test stream"));
			// AddInputStream(12, new MultiStreamT<int>("stream_int12", m_ints, *this, "Test stream"));
			// AddInputStream(13, new MultiStreamT<uint>("stream_uint13", m_uints, *this, "Test stream"));
			// AddInputStream(14, new MultiStreamT<float>("stream_float14", m_floats, *this, "Test stream"));
			// AddInputStream(15, new MultiStreamT<double>("stream_double15", m_doubles, *this, "Test stream"));

			AddOutputStream( 0, new StreamObject("stream_object0", m_obj, *this, "Test stream"));
			AddOutputStream( 1, new StreamEvent("stream_event1", m_evt, *this, "Test stream"));
			AddOutputStream( 2, new StreamImage("stream_image2", m_image, *this, "Test stream"));
			AddOutputStream( 3, new StreamNum<bool>("stream_bool3", m_bool, *this, "Test stream"));
			AddOutputStream( 4, new StreamNum<int>("stream_int4", m_int, *this, "Test stream"));
			AddOutputStream( 5, new StreamNum<uint>("stream_uint5", m_uint, *this, "Test stream"));
			AddOutputStream( 6, new StreamNum<float>("stream_float6", m_float, *this, "Test stream"));
			AddOutputStream( 7, new StreamNum<double>("stream_double7", m_double, *this, "Test stream"));

			AddOutputStream( 8, new MultiStreamT<vector<Object>>("stream_object8", m_objs, *this, "Test stream"));
			// AddOutputStream( 9, new MultiStreamT<Event>("stream_event9", m_evts, *this, "Test stream"));
			// AddOutputStream(10, new MultiStreamT<cv::Mat>("stream_image10", m_images, *this, "Test stream"));
			// AddOutputStream(11, new MultiStreamT<bool>("stream_bool11", m_bools, *this, "Test stream"));
			// AddOutputStream(12, new MultiStreamT<int>("stream_int12", m_ints, *this, "Test stream"));
			// AddOutputStream(13, new MultiStreamT<uint>("stream_uint13", m_uints, *this, "Test stream"));
			// AddOutputStream(14, new MultiStreamT<float>("stream_float14", m_floats, *this, "Test stream"));
			// AddOutputStream(15, new MultiStreamT<double>("stream_double15", m_doubles, *this, "Test stream"));
		}

		~FakeModule() override {}
		MKCLASS("FakeModule")
		MKDESCR("Output video stream with additional object streams")
	
		void Reset() override {}
		void ProcessFrame() override
		{
			for(auto& stream : m_inputStreams)
				stream.second->Randomize(seed);
		}

	private:
		const Module::Parameters& m_param;
		uint seed = 6345345;

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
	StreamsTestSuite()
	{
		readFromFile(m_config, "tests/streams/config.json");
	}

protected:
	mkconf        m_config;
	Module::Parameters* mp_fakeParams1 = nullptr;
	Module::Parameters* mp_fakeParams2 = nullptr;
	Module::Parameters* mp_fakeParams3 = nullptr;
	FakeModule*         mp_fakeModule1 = nullptr;
	FakeModule*         mp_fakeModule2 = nullptr;
	FakeModule*         mp_fakeModule3 = nullptr;

public:
	void setUp() override
	{
		mp_fakeParams1 = new Module::Parameters("FakeModule");
		mp_fakeParams2 = new Module::Parameters("FakeModule");
		mp_fakeParams3 = new Module::Parameters("FakeModule");
		mp_fakeParams1->Read(m_config.at("modules").at("FakeModule"));
		mp_fakeParams2->Read(m_config.at("modules").at("FakeModule"));
		mp_fakeParams3->Read(m_config.at("modules").at("FakeModule"));
		mp_fakeParams1->width  = 320;
		mp_fakeParams1->height = 240;
		mp_fakeParams1->type   = CV_8UC3;
		mp_fakeModule1       = new FakeModule(*mp_fakeParams1);
		mp_fakeParams2->width  = 160;
		mp_fakeParams2->height = 120;
		mp_fakeModule2       = new FakeModule(*mp_fakeParams2);
		mp_fakeParams3->width  = 320;
		mp_fakeParams3->height = 240;
		mp_fakeModule3       = new FakeModule(*mp_fakeParams3);

		for(auto input : mp_fakeModule1->GetInputStreamList())
		{
			// connect inputs with outputs
			string name = input.first;
			mp_fakeModule2->RefInputStreamByName(name).Connect(mp_fakeModule1->RefOutputStreamByName(name));
			mp_fakeModule3->RefInputStreamByName(name).Connect(mp_fakeModule2->RefOutputStreamByName(name));
		}

		mp_fakeModule1->Reset();
		mp_fakeModule2->Reset();
		mp_fakeModule3->Reset();
	}
	void tearDown() override
	{
		delete(mp_fakeModule1);
		delete(mp_fakeModule2);
		delete(mp_fakeModule3);
		delete(mp_fakeParams1);
		delete(mp_fakeParams2);
		delete(mp_fakeParams3);
	}

public:
	void testConvertInput()
	{
		uint seed = 556345;

		for(int i = 0 ; i < 100 ; i++)
		{
			mp_fakeModule1->ProcessFrame();
			for(auto input : mp_fakeModule1->GetInputStreamList())
			{
				string name = input.first;
				mp_fakeModule2->RefInputStreamByName(name).ConvertInput();
				mp_fakeModule3->RefInputStreamByName(name).ConvertInput();
			}
		}
	}

	void testStreamAsParameters()
	{
		mp_fakeModule1->Reset();
		for(auto input : mp_fakeModule1->GetInputStreamList())
		{
			ParametersTestSuite::testParameter(*input.second, input.second->GetDefault(), nullptr);
		}
	}
};
#endif
