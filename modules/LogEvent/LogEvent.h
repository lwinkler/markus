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
		m_list.push_back(new ParameterString("file"       , "event" , &file        , "Name of the .srt file without extension"));
		m_list.push_back(new ParameterDouble("time"       , 0       , 0, 600 , &duration , "Duration of the event for logging in .srt file"));
		m_list.push_back(new ParameterString("folder_name", "events_img" , &folder    , "Name of the folder to create for images"));
		m_list.push_back(new ParameterString("extension"  , "jpg"   , &extension   , "Extension of the thumbnails. Determines the output format."));

		RefParameterByName("type").SetDefault("CV_8UC3");
		ParameterStructure::Init();
	}
	std::string file;
	double duration;
	std::string extension;
	std::string folder;
};

class LogEvent : public Module
{
public:
	LogEvent(const ConfigReader& x_configReader);
	~LogEvent(void);
	void Reset();

protected:
	virtual void ProcessFrame();
	void WriteEvent();
	void SaveImage();

	Event m_event;
	long int m_subId;
	std::string m_srtFileName;
	cv::Mat * m_input;
	std::ofstream m_file;
	bool m_saveImage;
	std::string m_folderName;

private:
	inline virtual LogEventParameterStructure& RefParameter() { return m_param;} // TODO: see that this is always private
	static const char * m_type; // TODO: see if we keep this or not
	LogEventParameterStructure m_param;
};

#endif

