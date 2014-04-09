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
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>


using namespace cv;
using namespace std;

log4cxx::LoggerPtr Event::m_logger(log4cxx::Logger::getLogger("Event"));

Event::Event() :
	m_object("empty")
{
	m_absTimeEvent = 0;
	m_absTimeNotif = 0;
}

Event::~Event(){}


// Serialize event from JSON
void Event::Serialize(std::ostream& x_out) const
{
	Json::Value root;
	if(IsRaised())
	{
		root["label"]  = m_label; // TODO rename label ?
		if(m_object.GetName() != "empty")
		{
			stringstream ss;
			m_object.Serialize(ss);
			ss >> root["object"];
		}
	}
	x_out << root;
}

/// Deserialize the event from JSON
void Event::Deserialize(std::istream& x_in)
{
	// TODO
	// Note that a null JSON means that the event was not raised
}

/// Empty the event: must be called on each frame process to avoid raising multiple events
void Event::Empty()
{
	m_label = "";
	m_object = Object("empty");
}

/// Raise an event with a set of features
void Event::Raise(const string x_label, const Object& x_object, TIME_STAMP x_absTimeEvent)
{
	m_absTimeEvent = x_absTimeEvent;
	if(IsRaised())
		LOG_WARN(m_logger, "The same event is raised several times. Older events are overriden");
	m_label       = x_label;
	m_object      = x_object;
}

/// Raise an event without features
void Event::Raise(const string x_label, TIME_STAMP x_absTimeEvent)
{
	m_absTimeEvent = x_absTimeEvent;
	if(IsRaised())
		LOG_WARN(m_logger, "The same event is raised several times. Older events are overriden");
	m_label = x_label;
	m_object = Object("empty");
}



// Log an event and notify the parent process
void Event::Notify(const string& x_extraInfo, bool x_isProcessEvent)
{
	stringstream extra;
	if(x_extraInfo != "")
		extra<<", \"attrs\": {"<<x_extraInfo<<"}";

	if(x_isProcessEvent)
	{
		LOG_WARN(m_logger, "@notif@ PROCESS {"
				<< jsonify("label", GetLabel()) <<", "
				<< jsonify("dateNotif", getAbsTimeMs())
				<< extra.str()
				<< "}");
	}
	else
	{
		assert(x_extraInfo == "");
		m_absTimeNotif = getAbsTimeMs();
		stringstream ss;
		Serialize(ss);
		LOG_WARN(m_logger, "@notif EVENT " << ss.str());
		/*
		LOG_WARN(m_logger, "@notif@ EVENT {"
				<< jsonify("label", GetLabel()) <<", "
				<< jsonify("dateEvent",  m_absTimeEvent == 0 ? getAbsTimeMs() : m_absTimeEvent) <<", "
				<< jsonify("dateNotif", getAbsTimeMs()) <<", "
				<< jsonify("validity", 1.0) // TODO: add a formatter for floats
				<< extra.str()
				<< "}");
		*/
	}

}
