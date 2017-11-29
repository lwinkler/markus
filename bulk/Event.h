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

#ifndef MKEVENT_H
#define MKEVENT_H

#include "define.h"
#include "Object.h"
#include "Context.h"
#include "json.hpp"
#include <log4cxx/logger.h>


/// Class representing a event (e.g. Ponctual moment in time where an intrusion occurs)
class Event
{
public:
	friend void to_json(mkjson& _json, const Event& _ser);
	friend void from_json(const mkjson& _json, Event& _ser);

	Event();
	virtual ~Event();
	void Clean();
	void Raise(const std::string& x_eventName, TIME_STAMP x_absTimeNotif, TIME_STAMP x_absTimeEvent);
	void Raise(const std::string& x_eventName, const Object& m_object, TIME_STAMP x_absTimeNotif, TIME_STAMP x_absTimeEvent);
	inline bool IsRaised() const {return m_eventName != "";}
	inline const std::string& GetEventName() const {return m_eventName;}
	inline const Object& GetObject() const {return m_object;}
	inline const TIME_STAMP& GetTimeNotif() const {return m_timeStampNotif;}
	inline const TIME_STAMP& GetTimeEvent() const {return m_timeStampEvent;}
	inline const TIME_STAMP& GetTimeStart() const {return m_timeStampStart;}
	inline const TIME_STAMP& GetTimeEnd() const {return m_timeStampEnd;}
	inline void AddFeature(std::string x_name, Feature* x_feat) {m_object.AddFeature(x_name, x_feat);}
	inline void ScaleObject(cv::Point_<double> x_ratio)
	{
		m_object.posX   *= x_ratio.x;
		m_object.posY   *= x_ratio.y;
		m_object.width  *= x_ratio.x;
		m_object.height *= x_ratio.y;
	}
	inline void SetObject(const Object& x_object) {m_object = x_object;}
	// inline const std::string& GetObjectEventName(){return m_objectEventName;};

	inline void AddFeature(std::string x_name, double x_value) {m_object.AddFeature(x_name, x_value);}
	inline const std::map <std::string, FeaturePtr>& GetFeatures() const {return m_object.GetFeatures();}
	inline const Feature& GetFeature(const std::string& x_name) const
	{
		return m_object.GetFeature(x_name);
	}
	void Notify(const Context& x_contextbool, bool x_isProcessEvent=false);
	virtual void Randomize(unsigned int& xr_seed, const mkjson& x_requirement, const cv::Size& x_size);

	template<class T> inline void AddExternalInfo(const std::string& x_label, const T& x_value) {m_externalInfo[x_label] = x_value;}
	// inline void AddExternalInfo(const std::string& x_label, const std::string& x_value) {m_externalInfo[x_label] = x_value;}
	// inline void AddExternalInfo(const std::string& x_label, double x_value) {m_externalInfo[x_label] = x_value;}
	// inline void AddExternalInfo(const std::string& x_label, int x_value) {m_externalInfo[x_label] = x_value;}
	// inline void AddExternalInfo(const std::string& x_label, uint64_t x_value) {Json::Value::UInt64 val(x_value); m_externalInfo[x_label] = val;}
	// inline void AddExternalInfo(const std::string& x_label, std::istream& x_in) {x_in >> m_externalInfo[x_label];}
	inline void AddExternalFile(const std::string& x_label, const std::string& x_file) {m_externalInfo["files"][x_label] = x_file;}
	void GetExternalFiles(std::map<std::string, std::string>& xr_output) const;

protected:
	std::string m_eventName;
	Object m_object;
	/// Time stamp of the event (should correspond with the most important frame of the event)
	TIME_STAMP m_timeStampEvent = TIME_STAMP_MIN;
	/// Time stamp of the notification (Can be different if the event is notified with a delay)
	TIME_STAMP m_timeStampNotif = TIME_STAMP_MIN;
	/// Time stamp of the start of the event
	TIME_STAMP m_timeStampStart = TIME_STAMP_MIN;
	/// Time stamp of the end of the event
	TIME_STAMP m_timeStampEnd = TIME_STAMP_MIN;
	mkjson m_externalInfo;

private:
	static log4cxx::LoggerPtr m_logger;
};
#endif
