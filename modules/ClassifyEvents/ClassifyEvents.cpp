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


const char * ClassifyEvents::m_type = "ClassifyEvents";

using namespace std;
using namespace cv;


ClassifyEvents::ClassifyEvents(const ConfigReader& x_configReader) :
	Module(x_configReader),
	m_param(x_configReader)
{
	m_description = "Filter events based on user feedback";

	m_inputStreams.push_back(new StreamEvent( 0, "events",   m_param.width, m_param.height, m_eventIn,  *this, "Incoming events"));

	m_outputStreams.push_back(new StreamEvent(0, "filtered", m_param.width, m_param.height, m_eventOut, *this, "Filtered events"));

#ifndef MARKUS_NO_GUI
	// Add a new control to play forward and rewind
	m_controls.push_back(new ControllerEvent(*this));
#endif
}

ClassifyEvents::~ClassifyEvents(void )
{
}

void ClassifyEvents::Reset()
{
	Module::Reset();
}

void ClassifyEvents::ProcessFrame()
{
	m_eventOut.Empty();
	if(! m_eventIn.IsRaised())
		return;


	m_eventOut = m_eventIn;
}

