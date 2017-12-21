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

#include "ModuleClassifyEvents.h"
#include "StreamEvent.h"
#include "StreamImage.h"
#include "StreamDebug.h"
#include "ControllerEvent.h"


namespace mk {
using namespace std;
using namespace cv;

log4cxx::LoggerPtr ModuleClassifyEvents::m_logger(log4cxx::Logger::getLogger("ModuleClassifyEvents"));

ModuleClassifyEvents::ModuleClassifyEvents(ParameterStructure& xr_params) :
	Module(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params))
{
	m_imageIn         = Mat(Size(m_param.width, m_param.height), m_param.type);
	m_imageToValidate = Mat(Size(m_param.width, m_param.height), m_param.type);
	AddInputStream(0, new StreamEvent( "events",   m_eventIn,  *this, "Incoming events"));
	AddInputStream(1, new StreamImage( "image" ,   m_imageIn,  *this, "Image associated with the event (for display)"));

	AddOutputStream(0, new StreamEvent("filtered", m_eventOut, *this, "Filtered events"));
	AddOutputStream(1, new StreamEvent("lastEvent", m_eventToValidate, *this, "Event awaiting user feedback"));

#ifdef MARKUS_DEBUG_STREAMS
	m_imageToValidate = Mat(Size(m_param.width, m_param.height), CV_8UC3);
	AddDebugStream(0, new StreamDebug("lastEventIm", m_imageToValidate, *this, "Image corresponding to the last event"));
#endif
}

ModuleClassifyEvents::~ModuleClassifyEvents()
{
}

void ModuleClassifyEvents::Reset()
{
	Module::Reset();
	// Add a new control to play forward and rewind
	if(!HasController("event"))
		AddController(new ControllerEvent(*this));
}

void ModuleClassifyEvents::ProcessFrame()
{
	m_eventOut.Clean();
	if(! m_eventIn.IsRaised())
		return;

	float result = PredictEventValidity(m_eventIn);
	m_eventIn.AddFeature("valid", new FeatureFloat(result));

	PushEvent();

	// Output the event if the prob > 0.5
	if(result > m_param.validityThres)
		m_eventOut = m_eventIn;
}

void ModuleClassifyEvents::PushEvent()
{
	if(true)// if(m_eventToValidate.IsRaised())
	{
		m_events.push_back(m_eventIn);
		m_images.push_back(m_imageIn);
		m_eventToValidate = m_eventIn;
		m_imageIn.copyTo(m_imageToValidate);
	}
	else
	{
		// m_eventToValidate = m_eventIn;
		// m_imageToValidate = m_imageIn;
	}
}

void ModuleClassifyEvents::PopEvent()
{
	if(!m_events.empty())
	{
		m_events.pop_front();
		// m_images.pop_front();
		if(!m_events.empty())
		{
			assert(! m_images.empty());
			m_images.front().copyTo(m_imageToValidate);
			m_eventToValidate = m_events.front();
		}
		else
		{
			// There is no more events in the list: empty
			m_imageToValidate.setTo(0);
			m_eventToValidate.Clean();
		}
	}
	else LOG_WARN(m_logger, "Cannot call PopEvent, event list is empty");
}
} // namespace mk
