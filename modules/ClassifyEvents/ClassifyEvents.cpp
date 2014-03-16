/*----------------------------------------------------------------------------------
*
*    author : Laurent Winkler <lwinkler888@gmail.com>
* 
* 
*    This file is not part of Markus.
-------------------------------------------------------------------------------------*/

#include "ClassifyEvents.h"
#include "StreamEvent.h"
#include "ControllerEvent.h"


using namespace std;
using namespace cv;

log4cxx::LoggerPtr ClassifyEvents::m_logger(log4cxx::Logger::getLogger("ClassifyEvents"));

ClassifyEvents::ClassifyEvents(const ConfigReader& x_configReader) :
	Module(x_configReader),
	m_param(x_configReader)
{
	m_description = "Filter events based on user feedback";

	AddInputStream(0, new StreamEvent( "events",   m_eventIn,  *this, "Incoming events"));

	AddOutputStream(0, new StreamEvent("filtered", m_eventOut, *this, "Filtered events"));
	AddOutputStream(1, new StreamEvent("last_event", m_eventToValidate, *this, "Event awaiting user feedback"));
}

ClassifyEvents::~ClassifyEvents(void )
{
}

void ClassifyEvents::Reset()
{
	Module::Reset();
	// Add a new control to play forward and rewind
	// if(HasNoControllers()) // TODO: find a way to create this once only !
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
	if(m_eventToValidate.IsRaised())
		m_events.push_back(m_eventIn);	
	else
		m_eventToValidate = m_eventIn;
}

void ClassifyEvents::PopEvent()
{
	if(m_events.size() != 0)
	{
		m_eventToValidate = m_events.front();
		m_events.pop_front();
	}

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
