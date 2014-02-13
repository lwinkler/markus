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
#include "Event.h"
#include "Object.h"
#include "Manager.h"

using namespace cv;
using namespace std;

Event::Event() :
	m_object("empty")
{
}

Event::~Event(){}


/// Empty the event: must be called on each frame process to avoid raising multiple events
void Event::Empty()
{
	m_label = "";
	m_object = Object("empty");
}

/// Raise an event with a set of features
void Event::Raise(const string x_label, const Object& x_object)
{
	if(IsRaised())
		LOG_WARN(Manager::Logger(), "The same event is raised several times. Older events are overrided");
	m_label       = x_label;
	m_object      = x_object;
}

/// Raise an event without features
void Event::Raise(const string x_label)
{
	if(IsRaised())
		LOG_WARN(Manager::Logger(), "The same event is raised several times. Older events are overrided");
	m_label = x_label;
	m_object = Object("empty");
}
