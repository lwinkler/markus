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
	AddOutputStream(1, new StreamEvent("last_event", m_eventIn, *this, "Event awaiting user feedback"));
}

ClassifyEvents::~ClassifyEvents(void )
{
}

void ClassifyEvents::Reset()
{
	Module::Reset();
#ifndef MARKUS_NO_GUI
	// Add a new control to play forward and rewind
	Controller* ctr = new ControllerEvent(*this);
	m_controls.insert(std::make_pair(ctr->GetName(), ctr));
	//m_controls.push_back(new ControllerEvent(*this));
#endif
}

void ClassifyEvents::ProcessFrame()
{
	m_eventOut.Empty();
	if(! m_eventIn.IsRaised())
		return;

	float result = PredictEventValidity(m_eventIn);
	m_eventIn.AddFeature("valid", result);

	// Output the event if the prob > 0.5
	if(result > m_param.validityThres)
		m_eventOut = m_eventIn;
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
