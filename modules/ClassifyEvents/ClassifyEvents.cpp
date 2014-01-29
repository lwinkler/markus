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


ClassifyEvents::ClassifyEvents(const ConfigReader& x_configReader) :
	Module(x_configReader),
	m_param(x_configReader)
{
	m_description = "Filter events based on user feedback";

	m_inputStreams.push_back(new StreamEvent( 0, "events",   m_eventIn,  *this, "Incoming events"));

	m_outputStreams.push_back(new StreamEvent(0, "filtered", m_eventOut, *this, "Filtered events"));
	m_outputStreams.push_back(new StreamEvent(1, "last_event", m_eventIn, *this, "Event awaiting user feedback"));
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
	throw MkException("This method must be redefined in a children", LOC);
}

/// Consider the last event as valid and train the classifier
void ClassifyEvents::ValidateLastEvent()
{
	throw MkException("This method must be redefined in a children", LOC);
}

/// Return a probability [0..1] that the event is valid
float ClassifyEvents::PredictEventValidity(const Event& x_event)
{
	return 1;
}
