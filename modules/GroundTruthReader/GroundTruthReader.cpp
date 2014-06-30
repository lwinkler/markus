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
	trackedObj.clear();

	CLEAN_DELETE(mp_annotationReader);
	m_assFile = (m_param.file.substr(m_param.file.find_last_of(".") + 1) == "ass");
	if(m_assFile)
	{
		mp_annotationReader = new AnnotationAssFileReader(m_param.width,m_param.height);
		const double diagonal = sqrt(m_param.width * m_param.width + m_param.height * m_param.height);
		distanceRefObject = diagonal * m_param.distance;
	}
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

	if (m_oldState != m_state)
	{
		// Raise an alarm
		m_event.Raise("Ground Truth changed");
		m_oldState = m_state;

		// Update tracked object
		if (m_state && m_assFile)
		{
			Rect refObj = mp_annotationReader->GetBox();
			if (refObj.width > 0) // rectangle can be absent from ass
			{
				// enlarge the bounding box
				refObj.x -= distanceRefObject / 2;
				refObj.y -= distanceRefObject / 2;
				refObj.width += distanceRefObject;
				refObj.height += distanceRefObject;

				// middle of bounding box
				Rect centerRefObj = Rect(refObj.x+((refObj.width - distanceRefObject) / 2) , refObj.y+ ((refObj.height - distanceRefObject) / 2), distanceRefObject, distanceRefObject);
				for(vector<Object>::iterator it = m_objects.begin() ; it != m_objects.end() ; it++)
				{
					Rect objRect = it->Rect();
					if (centerRefObj.contains(Point (objRect.x+objRect.width/2,objRect.y+objRect.height/2)) && refObj.area() >= objRect.area()) // middle of rect is in middle of bounding box and area is smaller than reference bounding box
					{
						trackedObj.push_back(it->GetId());
						LOG_DEBUG(m_logger, "Object "<< it->GetId()<<" is tracked until "<<mp_annotationReader->GetEndTimeStamp());
					}
				}
			}
		}
		else
			trackedObj.clear();
	}

// display the ROI in srt
#ifdef MARKUS_DEBUG_STREAMS
	Rect refObj = mp_annotationReader->GetBox();
	if (m_assFile && refObj.width > 0 && text != "")
	{
		// enlarge the bounding box
		refObj.x -= distanceRefObject / 2;
		refObj.y -= distanceRefObject / 2;
		refObj.width += distanceRefObject;
		refObj.height += distanceRefObject;

		Rect centerRefObj = Rect(refObj.x+((refObj.width - distanceRefObject) / 2) , refObj.y+ ((refObj.height - distanceRefObject) / 2), distanceRefObject, distanceRefObject);

		rectangle(m_debug,refObj,Scalar( 255, 0, 0));
		rectangle(m_debug,centerRefObj,Scalar( 200, 100, 100));
	}
#endif

	for(vector<Object>::iterator it = m_objects.begin() ; it != m_objects.end() ; it++)
	{
		// ass file with rect in subtitle
		if (m_assFile && refObj.width > 0)
		{
			// gt = 1 only if object has been detected in roi
			if (find (trackedObj.begin(), trackedObj.end(), it->GetId()) != trackedObj.end())
				it->AddFeature("gt",(float)true);
			else
				it->AddFeature("gt",(float)false);

		}
		else
			it->AddFeature("gt",m_state);

#ifdef MARKUS_DEBUG_STREAMS
		if (dynamic_cast<const FeatureFloat&> (it->GetFeature("gt")).value > 0)
		{
			rectangle(m_debug,it->Rect(),Scalar( 0, 255, 0));
			circle(m_debug,Point(it->posX,it->posY),2,Scalar(0,255,0));
		}
		else
		{
			rectangle(m_debug,it->Rect(),Scalar(100, 100, 100));
			circle(m_debug,Point(it->posX,it->posY),2,Scalar(100, 100, 100));
		}
#endif
	}

}
