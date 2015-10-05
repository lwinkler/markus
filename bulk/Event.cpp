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
#include "util.h"
#include "InterruptionManager.h"

#include <cstdio>


using namespace cv;
using namespace std;


#define COPY(target, src) target=src
#define COPY_AND_CHECK(target, src) target=src; if((target).IsNull() || (target).AsString() == "") {throw MkException("Element of exported event is invalid", LOC);}

log4cxx::LoggerPtr Event::m_logger(log4cxx::Logger::getLogger("Event"));

Event::Event() :
	m_object("empty")
{
	m_absTimeEvent = 0;
	m_absTimeNotif = 0;
	m_externalInfo.Clear();
}

Event::~Event() {}

/// Randomize the content of the event
void Event::Randomize(unsigned int& xr_seed, const string& x_requirement, const Size& x_size)
{
	// random event
	Empty();
	if(x_requirement != "" || rand_r(&xr_seed) < RAND_MAX /10) // TODO: reqs: object: features: ... ?
	{
		if(x_requirement == "" && rand_r(&xr_seed) < RAND_MAX /10)
		{
			Raise("random");
		}
		else
		{
			Object obj("random");
			obj.Randomize(xr_seed, x_requirement, x_size);
			Raise("random", obj);
		}
	}
}

void Event::Serialize(MkJson& xr_out, const string& x_dir) const
{
	xr_out["raised"] = IsRaised();

	if(IsRaised())
	{
		xr_out["eventName"] = m_eventName;
		xr_out["dateEvent"] = m_absTimeEvent;
		xr_out["dateNotif"] = m_absTimeNotif;
		if(m_object.GetName() != "empty")
		{
			m_object.Serialize(xr_out.Create("object"), x_dir);
		}
		else xr_out["object"] = MkJson::nullValue();
		xr_out["external"]    = m_externalInfo;
	}
}

void Event::Deserialize(MkJson& xr_in, const string& x_dir)
{
	// Note that a null JSON means that the event was not raised
	bool raised = xr_in["raised"].AsBool();
	if(raised)
	{
		m_eventName = xr_in["eventName"].AsString();
		m_absTimeEvent = xr_in["dateEvent"].AsInt64();
		m_absTimeNotif = xr_in["dateNotif"].AsInt64();

		if(!xr_in["object"].IsNull())
		{
			m_object.Deserialize(xr_in["object"], x_dir);
		}

		// Deserialize files
		m_externalInfo = xr_in["external"];
		/*
		Json::Value::Members members = xr_in["external"].getMemberNames();
		m_externalInfo.Clear();
		for(const auto& elem : members)
			m_externalInfo[elem] = xr_in["external"][elem];
			*/
	}
	else
	{
		m_eventName = "";
		m_absTimeEvent = 0;
		m_absTimeNotif = 0;
		m_object = Object("empty");
		m_externalInfo.Clear();
	}
}

/**
* @brief Empty the event: must be called on each frame process to avoid raising multiple events
*/
void Event::Empty()
{
	m_eventName = "";
	m_object = Object("empty");
	m_externalInfo.Clear();
}

/**
* @brief Raise an event with a set of features
*
* @param x_eventName    Name of the event (e.g. "motion", "intrusion")
* @param x_object       An object linked with the event. The features of the object will be used as the features of the event.
* @param x_absTimeEvent Optional: The time at which the event started. Use this field if events are raised with a delay.
*/
void Event::Raise(const string& x_eventName, const Object& x_object, TIME_STAMP x_absTimeEvent)
{
	m_absTimeEvent = x_absTimeEvent == 0 ? getAbsTimeMs() : x_absTimeEvent;
	if(IsRaised())
		LOG_WARN(m_logger, "The same event is raised several times. Older events are overriden");
	m_eventName       = x_eventName;
	m_object      = x_object;
}

/**
* @brief Raise an event without features
*
* @param x_eventName    Name of the event (e.g. "motion", "intrusion")
* @param x_absTimeEvent Optional: The time at which the event started. Use this field if events are raised with a delay.
*/
void Event::Raise(const string& x_eventName, TIME_STAMP x_absTimeEvent)
{
	InterruptionManager::GetInst().AddEvent("event." + x_eventName);// TODO keep this here ?
	m_absTimeEvent = x_absTimeEvent == 0 ? getAbsTimeMs() : x_absTimeEvent;
	if(IsRaised())
		LOG_WARN(m_logger, "The same event is raised several times. Older events are overriden");
	m_eventName = x_eventName;
	m_object = Object("empty");
}



/**
* @brief Log an event and notify the parent process
*
* @param x_isProcessEvent If true, this indicates that the event is linked with processing and not analytics (e.g. for process events: "started", "stopped", "exception", "status")
*/
void Event::Notify(const Context& x_context, bool x_isProcessEvent)
{
	m_absTimeNotif = getAbsTimeMs();
	string level = "EVENT";

	MkJson root;
	Serialize(root, "");

	LOG_DEBUG(m_logger, "Notify event:" << *this);

	// root["external"] = m_externalInfo;

	// export the event to our specific format
	MkJson out;
	COPY_AND_CHECK(out["dateEvent"]          , root["dateEvent"]);
	COPY_AND_CHECK(out["dateNotif"]          , root["dateNotif"]);
	COPY_AND_CHECK(out["applicationName"]    , x_context.GetApplicationName());
	COPY_AND_CHECK(out["applicationVersion"] , x_context.Version(false));
	COPY_AND_CHECK(out["eventName"]          , root["eventName"]);

	if(x_isProcessEvent)
	{
		// This is only a process event. Only used to notify the parent process
		out["attrs"] = root["external"];
		// note: attrs can be empty, create an empty vector
		if(out["attrs"].IsNull())
			out["attrs"] = MkJson::emptyArray();
		level = "PROCESS";
	}
	else
	{
		COPY_AND_CHECK(out["files"]       , root["external"]["files"]);

		if(m_object.GetName() != "empty")
		{
			MkJson outObj = out["objects"][0];
			MkJson inObj  = root["object"];
			stringstream ss;
			ss<<inObj["name"].AsString()<<inObj["id"].AsInt();
			COPY_AND_CHECK(outObj["objectId"] , ss.str());
			COPY_AND_CHECK(outObj["x"]        , inObj["x"]);
			COPY_AND_CHECK(outObj["y"]        , inObj["y"]);
			COPY_AND_CHECK(outObj["width"]    , inObj["width"]);
			COPY_AND_CHECK(outObj["height"]   , inObj["height"]);
			COPY_AND_CHECK(outObj["features"] , inObj["features"]);
		}
	}

	// Notify via stdout
	/* TODO
	Json::FastWriter writer;
	string tmp = writer.write(out);
	tmp.erase(remove(tmp.begin(), tmp.end(), '\n'), tmp.end());

	LOG_WARN(m_logger, "@notif@ " << level << " " << tmp);
	*/
}
