/*----------------------------------------------------------------------------------
*
*    author : Laurent Winkler <lwinkler888@gmail.com>
*
*
*    This file is not part of Markus.
-------------------------------------------------------------------------------------*/

#include "ClassifyEvents.h"
#include "StreamEvent.h"
#include "StreamImage.h"
#include "StreamDebug.h"
#include "ControllerEvent.h"


using namespace std;
using namespace cv;

log4cxx::LoggerPtr ClassifyEvents::m_logger(log4cxx::Logger::getLogger("ClassifyEvents"));

ClassifyEvents::ClassifyEvents(const ConfigReader& x_configReader) :
	Module(x_configReader),
	m_param(x_configReader)
{
	m_imageIn         = Mat(Size(m_param.width, m_param.height), m_param.type);
	m_imageToValidate = Mat(Size(m_param.width, m_param.height), m_param.type);
	AddInputStream(0, new StreamEvent( "events",   m_eventIn,  *this, "Incoming events"));
	AddInputStream(1, new StreamImage( "image" ,   m_imageIn,  *this, "Image associated with the event (for display)"));

	AddOutputStream(0, new StreamEvent("filtered", m_eventOut, *this, "Filtered events"));
	AddOutputStream(1, new StreamEvent("last_event", m_eventToValidate, *this, "Event awaiting user feedback"));

#ifdef MARKUS_DEBUG_STREAMS
	m_imageToValidate = Mat(Size(m_param.width, m_param.height), CV_8UC3);
	AddDebugStream(0, new StreamDebug("last_event_im", m_imageToValidate, *this, "Image corresponding to the last event"));
#endif
}

ClassifyEvents::~ClassifyEvents(void )
{
}

void ClassifyEvents::Reset()
{
	Module::Reset();
	// Add a new control to play forward and rewind
	if(FindController("event") == NULL)
		AddController(new ControllerEvent(*this));
}

void ClassifyEvents::ProcessFrame()
{
	m_eventOut.Empty();
	if(! m_eventIn.IsRaised())
		return;

	float result = PredictEventValidity(m_eventIn);
	m_eventIn.AddFeature("valid", result);

	PushEvent();

	// Output the event if the prob > 0.5
	if(result > m_param.validityThres)
		m_eventOut = m_eventIn;
}

void ClassifyEvents::PushEvent()
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

void ClassifyEvents::PopEvent()
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
			m_eventToValidate.Empty();
		}
	}
	else LOG_WARN(m_logger, "Cannot call PopEvent, event list is empty");
}

// ---------------------------------------------------------------------------------
// The following functions must be redefined
// ---------------------------------------------------------------------------------


/// Consider the last event as invalid and train the classifier
void ClassifyEvents::InvalidateLastEvent()
{
	LOG_WARN(m_logger, "This method is empty and must be redefined in a children");
}

/// Consider the last event as valid and train the classifier
void ClassifyEvents::ValidateLastEvent()
{
	LOG_WARN(m_logger, "This method must be redefined in a children");
}

/// Return a probability [0..1] that the event is valid
float ClassifyEvents::PredictEventValidity(const Event& x_event)
{
	return 1;
}
