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

#include "EventToObjects.h"
#include "StreamEvent.h"
#include "StreamObject.h"
#include "util.h"

using namespace std;
using namespace cv;

log4cxx::LoggerPtr EventToObjects::m_logger(log4cxx::Logger::getLogger("EventToObjects"));

EventToObjects::EventToObjects(const ConfigReader& x_configReader) 
	 : Module(x_configReader), m_param(x_configReader)
{
	AddInputStream(0, new StreamEvent("event", m_event, *this, "Input event"));

	AddOutputStream(0, new StreamObject("objects", m_objects, *this, "Output objects"));
}

EventToObjects::~EventToObjects()
{
}

void EventToObjects::Reset()
{
	Module::Reset();
}

void EventToObjects::ProcessFrame()
{
	m_objects.clear();
	if(m_event.IsRaised())
	{
		m_objects.push_back(m_event.GetObject());
	}
}


/// Overwrite this function to process only the input for frames with an event
///	this is a trick to speed up the time spent processing the inputs
/// 	there are two reason why we want to process: either the event is raised or the previous frame had a raised event
bool EventToObjects::IsInputProcessed() const
{
	const StreamEvent* pStream =  dynamic_cast<const StreamEvent*>(&m_inputStreams.at(0)->GetConnected());
	assert(pStream != NULL);
	return m_event.IsRaised() || pStream->GetEvent().IsRaised();
}
