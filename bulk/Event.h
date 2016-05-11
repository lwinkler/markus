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
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>
#include <log4cxx/logger.h>


class Event;

/// Class representing a event (e.g. Ponctual moment in time where an intrusion occurs)
class Event : public Serializable
{
public:
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
	inline void ScaleObject(double x_ratioX, double x_ratioY)
	{
		m_object.posX   *= x_ratioX;
		m_object.posY   *= x_ratioY;
		m_object.width  *= x_ratioX;
		m_object.height *= x_ratioY;
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
	virtual void Randomize(unsigned int& xr_seed, const std::string& x_requirement, const cv::Size& x_size);
	virtual void Serialize(std::ostream& xr_out, const std::string& x_dir) const;
	virtual void Deserialize(std::istream& x_in, const std::string& x_dir);

	inline void AddExternalInfo(const std::string& x_label, const std::string& x_value) {m_externalInfo[x_label] = x_value;}
	inline void AddExternalInfo(const std::string& x_label, double x_value) {m_externalInfo[x_label] = x_value;}
	inline void AddExternalInfo(const std::string& x_label, int x_value) {m_externalInfo[x_label] = x_value;}
	inline void AddExternalInfo(const std::string& x_label, uint64_t x_value) {Json::Value::UInt64 val(x_value); m_externalInfo[x_label] = val;}
	inline void AddExternalInfo(const std::string& x_label, std::istream& x_in) {x_in >> m_externalInfo[x_label];}
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
	Json::Value m_externalInfo;

private:
	static log4cxx::LoggerPtr m_logger;
};
#endif
