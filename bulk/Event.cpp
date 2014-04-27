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
#include "util.h"


using namespace cv;
using namespace std;

log4cxx::LoggerPtr Event::m_logger(log4cxx::Logger::getLogger("Event"));

Event::Event() :
	m_object("empty")
{
	m_absTimeEvent = 0;
	m_absTimeNotif = 0;
	m_externalInfo.clear();
}

Event::~Event(){}


/// @brief Serialize event from JSON
/// @param x_out Output stream
/// @param x_dir Output directory (for images)
void Event::Serialize(std::ostream& x_out, const string& x_dir) const
{
	Json::Value root;
	root["raised"] = IsRaised();

	if(IsRaised())
	{
		root["label"]  = m_label; // TODO rename label ?
		root["dateEvent"] = m_absTimeEvent;
		root["dateNotif"] = m_absTimeNotif;
		if(m_object.GetName() != "empty")
		{
			stringstream ss;
			m_object.Serialize(ss, x_dir);
			ss >> root["object"];
		}
	}
	x_out << root;
}

/// @brief Deserialize the event from JSON
/// @param x_in Input stream
/// @param x_dir Input directory (for images)
void Event::Deserialize(std::istream& x_in, const string& x_dir)
{
	// TODO
	// Note that a null JSON means that the event was not raised
}

/// Empty the event: must be called on each frame process to avoid raising multiple events
void Event::Empty()
{
	m_label = "";
	m_object = Object("empty");
	m_externalInfo.clear();
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
	m_absTimeEvent = x_absTimeEvent == 0 ? getAbsTimeMs() : x_absTimeEvent;
	if(IsRaised())
		LOG_WARN(m_logger, "The same event is raised several times. Older events are overriden");
	m_label = x_label;
	m_object = Object("empty");
}



// Log an event and notify the parent process
void Event::Notify(bool x_isProcessEvent)
{
	m_absTimeNotif = getAbsTimeMs();
	Json::Value root;

	stringstream ss;
	Serialize(ss, "");
	ss >> root;

	root["external"] = m_externalInfo;

	Json::FastWriter writer;

	if(x_isProcessEvent)
	{
		// This is only a process event. Only used to notify the parent process
		// root["label"]     = GetLabel();
		// root["dateNotif"] = getAbsTimeMs();
		// if(x_extraInfo != "")
			// root["external"] = x_extraInfo;
		LOG_WARN(m_logger, "@notif@ PROCESS " << writer.write(root));
	}
	else
	{
		LOG_WARN(m_logger, "@notif@ EVENT " << writer.write(root));
	}

}
