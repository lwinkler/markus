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

#ifndef LOG_EVENT_H
#define LOG_EVENT_H

#include "Module.h"
#include "Event.h"
#include "Timer.h"


/*! \class LogEvent
 *  \brief Class containing methods/attributes of a slit camera
 *
 */


class LogEventParameterStructure : public ModuleParameterStructure
{
	
public:
	LogEventParameterStructure(const ConfigReader& x_confReader) : 
		ModuleParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterString("file",  "event",  &file,                           "Name of the .srt file without extension"));
		m_list.push_back(new ParameterDouble("time",  0,        PARAM_DOUBLE, 0, 600, &duration, "Duration of the event for logging in .srt file"));
		ParameterStructure::Init();
	}
	std::string file;
	double duration;
};

class LogEvent : public Module
{
protected:
	virtual void ProcessFrame();
	LogEventParameterStructure m_param;
	static const char * m_type;


	Event m_event;
	// bool m_state;
	// bool m_oldState;
	long int m_subId;
	// std::string m_startTime;
	// std::string m_endTime;
	std::string m_srtFileName;

public:
	LogEvent(const ConfigReader& x_configReader);
	~LogEvent(void);
	void Reset();


protected:
	void WriteEvent();
	inline virtual LogEventParameterStructure& RefParameter() { return m_param;}
	
	std::ofstream m_file;
};

#endif

