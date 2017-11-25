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
#include "Serializable.h"
#include "util.h"
#include "InterruptionManager.h"

#include <cstdio>


using namespace cv;
using namespace std;


#define COPY(target, src) target=src
#define COPY_AND_CHECK(target, src) target=src; if((target).isNull() || (target) == "") {throw MkException("Element of exported event is invalid", LOC);}

log4cxx::LoggerPtr Event::m_logger(log4cxx::Logger::getLogger("Event"));

Event::Event() :
	m_object("empty")
{
	m_externalInfo.clear();
}

Event::~Event() {}

/// Randomize the content of the event
void Event::Randomize(unsigned int& xr_seed, const Json::Value& x_requirement, const Size& x_size)
{
	// random event
	Clean();
	if(x_requirement != Json::nullValue || rand_r(&xr_seed) < RAND_MAX /10)
	{
		if(x_requirement == Json::nullValue && rand_r(&xr_seed) < RAND_MAX /10)
		{
			Raise("random", rand_r(&xr_seed), rand_r(&xr_seed));
		}
		else
		{
			Object obj("random");
			obj.Randomize(xr_seed, x_requirement, x_size);
			Raise("random", obj, rand_r(&xr_seed), rand_r(&xr_seed));
		}
	}
}

void to_json(mkjson& rx_json, const Event& x_ser)
{
	if(x_ser.IsRaised())
	{
		rx_json = mkjson{
			{"raised", true},
			{"eventName", x_ser.m_eventName},
			{"dateEvent", x_ser.m_timeStampEvent},
			{"dateNotif", x_ser.m_timeStampNotif},
			{"object", x_ser.m_object}
			// TODO {"external", x_ser.m_externalInfo}
		};
	}
	else rx_json = mkjson{{"raised", false}};
}

void from_json(const mkjson& x_json, Event& _ser)
{
	// Note that a null JSON means that the event was not raised
	bool raised = x_json["raised"].get<bool>();
	if(raised)
	{
		_ser.m_eventName = x_json["eventName"].get<string>();
		_ser.m_timeStampEvent = x_json["dateEvent"].get<TIME_STAMP>();
		_ser.m_timeStampNotif = x_json["dateNotif"].get<TIME_STAMP>();

		if(!x_json["object"].is_null())
		{
			_ser.m_object = x_json["object"].get<Object>();
		}

		// Deserialize files
		_ser.m_externalInfo.clear();
		for(auto it = x_json.at("external").cbegin(); it != x_json.at("external").cend(); ++it)
			; // TODO m_externalInfo = x_json["external"].get<Object>();
	}
	else
	{
		_ser.m_eventName = "";
		_ser.m_timeStampEvent = 0;
		_ser.m_timeStampNotif = 0;
		_ser.m_object = Object("empty");
		_ser.m_externalInfo.clear();
	}
}

/**
* @brief Empty the event: must be called on each frame process to avoid raising multiple events
*/
void Event::Clean()
{
	m_eventName = "";
	m_object = Object("empty");
	m_externalInfo.clear();
}

/**
* @brief Raise an event with a set of features
*
* @param x_eventName    Name of the event (e.g. "motion", "intrusion")
* @param x_object       An object linked with the event. The features of the object will be used as the features of the event.
* @param x_absTimeNotif The time at which the event occured
* @param x_absTimeEvent The time at which the event started. The times will differ if events are raised with a delay.
*/
void Event::Raise(const string& x_eventName, const Object& x_object, TIME_STAMP x_absTimeNotif, TIME_STAMP x_absTimeEvent)
{
	m_timeStampEvent = x_absTimeEvent;
	m_timeStampNotif = x_absTimeNotif;
	if(IsRaised())
		LOG_WARN(m_logger, "The same event is raised several times. Older events are overridden");
	m_eventName       = x_eventName;
	m_object      = x_object;
}

/**
* @brief Raise an event without features
*
* @param x_eventName    Name of the event (e.g. "motion", "intrusion")
* @param x_absTimeNotif The time at which the event occured
* @param x_absTimeEvent The time at which the event started. The times will differ if events are raised with a delay.
*/
void Event::Raise(const string& x_eventName, TIME_STAMP x_absTimeNotif, TIME_STAMP x_absTimeEvent)
{
	InterruptionManager::GetInst().AddEvent("event." + x_eventName);
	m_timeStampEvent = x_absTimeEvent;
	m_timeStampNotif = x_absTimeNotif;
	if(IsRaised())
		LOG_WARN(m_logger, "The same event is raised several times. Older events are overridden");
	m_eventName = x_eventName;
	m_object = Object("empty");
}



/**
* @brief Log an event and notify the parent process
*
* @param x_context        The context object
* @param x_isProcessEvent If true, this indicates that the event is linked with processing and not analytics (e.g. for process events: "started", "stopped", "exception", "status")
*/
void Event::Notify(const Context& x_context, bool x_isProcessEvent)
{
	/*
	Json::Value root;
	string level = "EVENT";

	stringstream ss;
	Serialize(ss);
	ss >> root;

	LOG_DEBUG(m_logger, "Notify event:" << *this);

	// root["external"] = m_externalInfo;

	// export the event to our specific format
	Json::Value out;
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
		if(out["attrs"].isNull())
			out["attrs"] = Json::Value(Json::arrayValue);
		level = "PROCESS";
	}
	else
	{
		COPY_AND_CHECK(out["files"]       , root["external"]["files"]);

		if(m_object.GetName() != "empty")
		{
			Json::Value& outObj = out["objects"][0];
			Json::Value& inObj  = root["object"];
			stringstream ss;
			ss<<inObj["name"].asString()<<inObj["id"].asInt();
			COPY_AND_CHECK(outObj["objectId"] , ss.str());
			COPY_AND_CHECK(outObj["x"]        , inObj["x"]);
			COPY_AND_CHECK(outObj["y"]        , inObj["y"]);
			COPY_AND_CHECK(outObj["width"]    , inObj["width"]);
			COPY_AND_CHECK(outObj["height"]   , inObj["height"]);
			COPY_AND_CHECK(outObj["features"] , inObj["features"]);
		}
	}

	// Notify via stdout
	Json::FastWriter writer;
	string tmp = writer.write(out);
	tmp.erase(remove(tmp.begin(), tmp.end(), '\n'), tmp.end());

	LOG_WARN(m_logger, "@notif@ " << level << " " << tmp);
	*/
	LOG_WARN(m_logger, mkjson(*this).dump());
}

/// Return the external files
void Event::GetExternalFiles(map<std::string, string>& xr_output) const
{
	xr_output.clear();
	for(const auto& elem : m_externalInfo["files"].getMemberNames())
	{
		xr_output[elem] = m_externalInfo["files"][elem].asString();
	}
}
