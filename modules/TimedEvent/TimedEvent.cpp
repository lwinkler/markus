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

#include "TimedEvent.h"
#include "StreamImage.h"
#include "StreamEvent.h"

using namespace std;
using namespace cv;

log4cxx::LoggerPtr TimedEvent::m_logger(log4cxx::Logger::getLogger("TimedEvent"));

TimedEvent::TimedEvent(ParameterStructure& xr_params)
	: Module(xr_params), m_param(dynamic_cast<Parameters&>(xr_params)),
	  m_input(Size(m_param.width, m_param.height), m_param.type)
{
	// Init output images
	AddInputStream(0, new StreamImage("input", m_input, *this, 	"Video input"));

	AddOutputStream(0, new StreamEvent("event", m_event,  *this, 	"Event occuring at a fixed frequency"));
}

TimedEvent::~TimedEvent(void)
{
}

void TimedEvent::Reset()
{
	Module::Reset();
	m_nextEvent = TIME_STAMP_INITIAL;
}

void TimedEvent::ProcessFrame()
{
	m_event.Empty();

	LOG_DEBUG(m_logger, "Current and next timestamps " << m_currentTimeStamp << ">=" << m_nextEvent);

	if(m_currentTimeStamp >= m_nextEvent)
	{
		if(m_nextEvent == TIME_STAMP_INITIAL)
		{
			m_nextEvent = m_currentTimeStamp + m_param.timeInterval * 1000;
		}

		m_event.Raise("timed_event");
		while(m_nextEvent <= m_currentTimeStamp)
			m_nextEvent += m_param.timeInterval * 1000;
	}
}

/// Overwrite this function since we never need to convert the input (only used for timestamps)
///	this is a trick to speed up the time spent processing the inputs
bool TimedEvent::IsInputProcessed() const
{
	return false;
}
