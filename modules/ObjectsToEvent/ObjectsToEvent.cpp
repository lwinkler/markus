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

#include "ObjectsToEvent.h"
#include "StreamObject.h"
#include "StreamState.h"

using namespace std;
using namespace cv;

log4cxx::LoggerPtr ObjectsToEvent::m_logger(log4cxx::Logger::getLogger("ObjectsToEvent"));

ObjectsToEvent::ObjectsToEvent(ParameterStructure& xr_params) :
	Module(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params))
{
	AddInputStream(0, new StreamObject("input", 	m_objectsIn, *this,	"Incoming objects"));

	AddOutputStream(0, new StreamEvent("event",  m_event, *this,	"Raise an event if the condition is filled"));
	AddOutputStream(1, new StreamState("state",  m_state, *this,	"Set state to true if the condition is filled"));
}

void ObjectsToEvent::Reset()
{
	Module::Reset();
}

void ObjectsToEvent::ProcessFrame()
{
	m_event.Empty();
	if(m_objectsIn.size() >= m_param.minObjectsNb && m_objectsIn.size() <= m_param.maxObjectsNb)
	{
		if(!m_state)
			m_event.Raise(m_param.eventName);
		m_state = true;
	}
	else
	{
		m_state = false;
	}
}

