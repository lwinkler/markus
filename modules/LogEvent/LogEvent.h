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
#include "AnnotationFileWriter.h"



/**
* @brief Read an event and log it to .srt file
*/
class LogEvent : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) : 
			Module::Parameters(x_confReader)
		{
			m_list.push_back(new ParameterString("file"        , "event.srt"  , &file      , "Name of the .srt file without extension"));
			m_list.push_back(new ParameterDouble("duration"    , 5, 0, 600    , &duration  , "Duration of the event for logging in .srt file"));
			m_list.push_back(new ParameterString("folder_name" , "events_img" , &folder    , "Name of the folder to create for images"));
			m_list.push_back(new ParameterString("extension"   , "jpg"        , &extension , "Extension of the thumbnails. Determines the output format."));

			RefParameterByName("type").SetDefault("CV_8UC3");
			Init();
		}
		std::string file;
		double duration;
		std::string extension;
		std::string folder;
	};

	LogEvent(const ConfigReader& x_configReader);
	~LogEvent(void);
	MKCLASS("LogEvent")
	MKDESCR("Read an event and log it to .srt file")

	inline virtual const Parameters& GetParameters() const { return m_param;}
	virtual void ProcessFrame();
	void Reset();

private:
	inline virtual Parameters & RefParameters() {return m_param;}
	Parameters m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	void SaveImage(Event& x_event);
	bool IsInputProcessed() const;
	void AddExternalImage(const cv::Mat& x_image, const std::string& x_name, const std::string& x_file, Event& x_event);
	void WriteEvent();

	// input
	Event m_event;
	cv::Mat m_inputIm1;
	cv::Mat m_inputIm2;

	// temporary
	bool m_saveImage1;
	bool m_saveImage2;
	std::string m_folder;
	AnnotationFileWriter* mp_annotationWriter;
};

#endif

