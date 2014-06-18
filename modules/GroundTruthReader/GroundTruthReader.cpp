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

#include "GroundTruthReader.h"
#include "StreamImage.h"
#include "StreamObject.h"
#include "StreamState.h"
#include "StreamEvent.h"
#include "StreamDebug.h"
#include "util.h"

using namespace std;
using namespace cv;

log4cxx::LoggerPtr GroundTruthReader::m_logger(log4cxx::Logger::getLogger("GroundTruthReader"));


GroundTruthReader::GroundTruthReader(const ConfigReader& x_configReader): 
	Module(x_configReader),
	m_param(x_configReader),
	m_input(Size(m_param.width, m_param.height), m_param.type)
{
	mp_annotationReader = NULL;
	AddInputStream(0, new StreamImage("input",  m_input, *this, "Video input"));
	AddInputStream(1, new StreamObject("input object",  m_objects, *this, "Incoming objects"));

	// AddOutputStream(0, new StreamImage("input", m_input,  *this, 	"Copy of the input stream"));
	AddOutputStream(0, new StreamState("state", m_state,  *this, 	"State read from the annotation file"));
	AddOutputStream(1, new StreamObject("output object",  m_objects, *this, "Outcoming objects"));
	AddOutputStream(2, new StreamEvent( "change_gt", m_event,     *this,   "State of ground truth changed"));

#ifdef MARKUS_DEBUG_STREAMS
	m_debug = Mat(Size(m_param.width, m_param.height), CV_8UC3);
	AddDebugStream(0, new StreamDebug("debug", m_debug,  *this, 	"Debug stream"));
#endif
}

GroundTruthReader::~GroundTruthReader()
{
	CLEAN_DELETE(mp_annotationReader);
}

void GroundTruthReader::Reset()
{
	Module::Reset();
	m_state    = false;
	m_oldState = false;

	CLEAN_DELETE(mp_annotationReader);
	m_assFile = (m_param.file.substr(m_param.file.find_last_of(".") + 1) == "ass");
	if(m_assFile)
		mp_annotationReader = new AnnotationAssFileReader();
	else
		mp_annotationReader = new AnnotationSrtFileReader();

	mp_annotationReader->Open(m_param.file);
}

void GroundTruthReader::ProcessFrame()
{
	m_event.Empty();
	m_state = false;
#ifdef MARKUS_DEBUG_STREAMS
	m_debug.setTo(0);
#endif

	// Interpret the current state
	string text = mp_annotationReader->ReadAnnotationForTimeStamp(m_currentTimeStamp);
	LOG_DEBUG(m_logger, "Read annotation \"" + text + "\" for time stamp "<<m_currentTimeStamp);
	if(text != "")
	{
		m_state = text.find(m_param.pattern) != string::npos; // TODO: Improve this with regexp

#ifdef MARKUS_DEBUG_STREAMS
		Scalar color = m_state ? Scalar(0, 255, 0) : Scalar(0, 0, 255); 
		putText(m_debug, text, Point(40, 40),  FONT_HERSHEY_COMPLEX_SMALL, 0.8, color);
#endif
	}

	// annotate stream objects
	Rect refObj = mp_annotationReader->GetBox();

	if (m_assFile && refObj.width > 0)	// rectangle can be absent from ass
	{

		// enlarge the bounding box
		refObj.x -= m_param.distance / 2;
		refObj.y -= m_param.distance / 2;
		refObj.width += m_param.distance;
		refObj.height += m_param.distance;

		Rect lcRefObj = Rect(refObj.x, refObj.y, m_param.distance, m_param.distance);

#ifdef MARKUS_DEBUG_STREAMS
		if (text != "")
			rectangle(m_debug,refObj,Scalar( 205, 205, 205));
#endif

		for(vector<Object>::iterator it = m_objects.begin() ; it != m_objects.end() ; it++)
		{
			// test bounding rectangle only if necessary
			if (m_state)
			{
				Rect obj = it->Rect();				

				if (lcRefObj.contains(Point (obj.x,obj.y)) && refObj.area() >= obj.area())
				{
					it->AddFeature("gt",m_state);
#ifdef MARKUS_DEBUG_STREAMS
					rectangle(m_debug,obj,Scalar( 0, 255, 0));
#endif
				}
				else
				{
					it->AddFeature("gt",false);
#ifdef MARKUS_DEBUG_STREAMS
					rectangle(m_debug,obj,Scalar( 255, 255, 0));
#endif
				}
			}
			else
				it->AddFeature("gt",m_state);
		}
	}
	else
	{
		for(vector<Object>::iterator it = m_objects.begin() ; it != m_objects.end() ; it++)
		{
			it->AddFeature("gt",m_state);
		}
	}

	if (m_oldState != m_state)
	{
		m_event.Raise("Ground Truth changed");
		m_oldState = m_state;
	}
}
