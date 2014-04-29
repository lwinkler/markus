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

void Event::Deserialize(std::istream& x_in, const string& x_dir)
{
	// TODO
	// Note that a null JSON means that the event was not raised
}

/**
* @brief Empty the event: must be called on each frame process to avoid raising multiple events
*/
void Event::Empty()
{
	m_label = "";
	m_object = Object("empty");
	m_externalInfo.clear();
}

/**
* @brief Raise an event with a set of features
*
* @param x_label        Label of the event (e.g. "motion", "intrusion")
* @param x_object       An object linked with the event. The features of the object will be used as the features of the event.
* @param x_absTimeEvent Optional: The time at which the event started. Use this field if events are raised with a delay.
*/
void Event::Raise(const string x_label, const Object& x_object, TIME_STAMP x_absTimeEvent)
{
	m_absTimeEvent = x_absTimeEvent == 0 ? getAbsTimeMs() : x_absTimeEvent;
	if(IsRaised())
		LOG_WARN(m_logger, "The same event is raised several times. Older events are overriden");
	m_label       = x_label;
	m_object      = x_object;
}

/**
* @brief Raise an event without features
*
* @param x_label        Label of the event (e.g. "motion", "intrusion")
* @param x_absTimeEvent Optional: The time at which the event started. Use this field if events are raised with a delay.
*/
void Event::Raise(const string x_label, TIME_STAMP x_absTimeEvent)
{
	m_absTimeEvent = x_absTimeEvent == 0 ? getAbsTimeMs() : x_absTimeEvent;
	if(IsRaised())
		LOG_WARN(m_logger, "The same event is raised several times. Older events are overriden");
	m_label = x_label;
	m_object = Object("empty");
}



/**
* @brief Log an event and notify the parent process
*
* @param x_isProcessEvent If true, this indicates that the event is linked with processing and not analytics (e.g. for process events: "started", "stopped", "exception", "status")
*/
void Event::Notify(bool x_isProcessEvent)
{
	m_absTimeNotif = getAbsTimeMs();
	Json::Value root;

	stringstream ss;
	Serialize(ss, "");
	ss >> root;

	root["external"] = m_externalInfo;

	Json::FastWriter writer;
	string tmp = writer.write(root);
	tmp.erase(std::remove(tmp.begin(), tmp.end(), '\n'), tmp.end());

	if(x_isProcessEvent)
	{
		// This is only a process event. Only used to notify the parent process

		LOG_WARN(m_logger, "@notif@ PROCESS " << tmp);
	}
	else
	{
		LOG_WARN(m_logger, "@notif@ EVENT " << tmp);
	}

}
