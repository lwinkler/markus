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
#include "FeatureStd.h"
#include "util.h"
#include "AnnotationFileReader.h"

namespace mk {
using namespace std;
using namespace cv;

log4cxx::LoggerPtr GroundTruthReader::m_logger(log4cxx::Logger::getLogger("GroundTruthReader"));


GroundTruthReader::GroundTruthReader(ParameterStructure& xr_params):
	Module(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params)),
	m_input(Size(m_param.width, m_param.height), m_param.type)
{
	AddInputStream(0, new StreamImage("image",  m_input, *this, "Video input"));
	AddInputStream(1, new StreamObject("objects",  m_objects, *this, "Incoming objects"));

	// AddOutputStream(0, new StreamImage("input", m_input,  *this, 	"Copy of the input stream"));
	AddOutputStream(0, new StreamState("state", m_state,  *this, 	"State read from the annotation file"));
	AddOutputStream(1, new StreamObject("objects",  m_objects, *this, "Outcoming objects"));
	AddOutputStream(2, new StreamEvent( "event", m_event,     *this,   "State of ground truth changed"));

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
		const double diagonal = sqrt(m_param.width * m_param.width + m_param.height * m_param.height);
		distanceRefObject = diagonal * m_param.distance;
	}
	mp_annotationReader = createAnnotationFileReader(m_param.file, m_param.width, m_param.height);
}

void GroundTruthReader::ProcessFrame()
{
	m_event.Clean();
	m_state = false;
#ifdef MARKUS_DEBUG_STREAMS
	m_debug.setTo(0);
#endif

	// Interpret the current state
	string text = mp_annotationReader->ReadAnnotationForTimeStamp(m_currentTimeStamp);
	LOG_DEBUG(m_logger, "Read annotation \"" + text + "\" for time stamp "<<m_currentTimeStamp);
	if(text != "")
	{
		// looking to match a part of pattern
		m_state = m_param.pattern.find(text) != string::npos;

#ifdef MARKUS_DEBUG_STREAMS
		Scalar color = m_state ? Scalar(0, 255, 0) : Scalar(0, 0, 255);
		putText(m_debug, text, Point(40, 40),  FONT_HERSHEY_COMPLEX_SMALL, 0.8, color);
#endif
	}

	if (m_oldState != m_state)
	{
		// Raise an alarm
		m_event.Raise("Ground Truth changed", m_currentTimeStamp, m_currentTimeStamp);
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
				Rect centerRefObj = Rect(
										refObj.x + (refObj.width  - distanceRefObject) / 2,
										refObj.y + (refObj.height - distanceRefObject) / 2,
										distanceRefObject, distanceRefObject
									);
				for(const auto& obj : m_objects)
				{
					Rect objRect = obj.GetRect();
					if (centerRefObj.contains(Point (objRect.x+objRect.width/2,objRect.y+objRect.height/2)) && refObj.area() >= objRect.area()) // middle of rect is in middle of bounding box and area is smaller than reference bounding box
					{
						trackedObj.push_back(obj.GetId());
						LOG_DEBUG(m_logger, "Object "<< obj.GetId()<<" is tracked until "<<mp_annotationReader->GetEndTimeStamp());
					}
				}
			}
		}
		else
			trackedObj.clear();
	}

	// display the ROI in srt
	Rect refObj = mp_annotationReader->GetBox();

#ifdef MARKUS_DEBUG_STREAMS
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

	for(auto& elem : m_objects)
	{
		// ass file with rect in subtitle
		if (m_assFile && refObj.width > 0)
		{
			// gt = 1 only if object has been detected in roi
			if (find (trackedObj.begin(), trackedObj.end(), elem.GetId()) != trackedObj.end())
			{
				elem.AddFeature("gt", new FeatureFloat(1.0));
				elem.AddFeature("label", new FeatureString(text));
			}
			else
			{
				elem.AddFeature("gt", new FeatureFloat(0.0));
				elem.AddFeature("label", new FeatureString(""));
			}

		}
		else
		{
			elem.AddFeature("gt", new FeatureFloat(static_cast<float>(m_state)));
			elem.AddFeature("label", new FeatureString(text));
		}

#ifdef MARKUS_DEBUG_STREAMS
		if (dynamic_cast<const FeatureFloat&> (elem.GetFeature("gt")).value > 0)
		{
			rectangle(m_debug,elem.GetRect(),Scalar( 0, 255, 0));
			circle(m_debug,Point(elem.posX,elem.posY),2,Scalar(0,255,0));
		}
		else
		{
			rectangle(m_debug,elem.GetRect(),Scalar(100, 100, 100));
			circle(m_debug,Point(elem.posX,elem.posY),2,Scalar(100, 100, 100));
		}
#endif
	}

}
} // namespace mk
