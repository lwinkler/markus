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

#include "LogEvent.h"
#include "StreamEvent.h"
#include "StreamImage.h"
#include "util.h"
#include "Manager.h"

#define SEP "\t"

using namespace std;
using namespace cv;

const char * LogEvent::m_type = "LogEvent";

LogEvent::LogEvent(const ConfigReader& x_configReader) 
	 : Module(x_configReader), m_param(x_configReader)
{
	m_description = "This module takes an event as input and logs it to .srt file";
	m_input       = new Mat(Size(m_param.width, m_param.height), m_param.type);

	// Init input images
	m_inputStreams.push_back(new StreamEvent(0, "event", m_event, *this, "Input event to be logged"));
	m_inputStreams.push_back(new StreamImage(1, "image", m_input, *this, "Video input for image extraction"));

	m_saveImage = false;
}

LogEvent::~LogEvent(void)
{
	delete(m_input);
}

void LogEvent::Reset()
{
	Module::Reset();
	m_event.Empty();
	m_subId = 0;
	
	m_srtFileName = Manager::OutputDir() + "/" + m_param.file + ".srt";
	m_file.close();
	m_file.open (m_srtFileName.c_str(), std::ios_base::app);
	if(! m_file.is_open())
		throw FileNotFoundException("Impossible to open file in LogEvent::Reset", LOC);
	m_saveImage = m_inputStreams.at(1)->IsConnected();
}

void LogEvent::ProcessFrame()
{
	if(m_event.IsRaised())
	//if(m_state != m_oldState)
	{
		LOG_DEBUG(m_logger, "event change");
		// Log the change in event
		WriteEvent();
		if(m_saveImage)
			SaveImage();
		m_subId++;
	}
}

/// Write the subtitle in log file
void LogEvent::WriteEvent()
{
	string startTime = msToTimeStamp(m_currentTimeStamp);
	string endTime   = msToTimeStamp(m_currentTimeStamp + 1000 * m_param.duration);

	m_file<<m_subId<<endl;
	m_file<<startTime<<" --> "<<endTime<<endl;

	// Log event label
	m_file<<m_event.GetLabel()<<endl;

	// Log event features with values
	if(m_event.GetFeatures().size() != 0)
		m_file<<"time"<<SEP<<"object"<<SEP<<"feature"<<SEP<<"value"<<SEP<<"mean"<<SEP<<"sqVariance"<<SEP<<"initial"<<SEP<<"min"<<SEP<<"max"<<SEP<<"nbSamples"<<endl;
	for(map<string, Feature>::const_iterator it2 = m_event.GetFeatures().begin() ; it2 != m_event.GetFeatures().end() ; it2++)
	{
		const Feature & feat = it2->second;
		m_file<<m_currentTimeStamp<<SEP
			<<m_event.GetObject().GetName()<<m_event.GetObject().GetId()<<SEP
			<<it2->first<<SEP
			<<feat.value<<SEP
			<<feat.mean<<SEP
			<<feat.sqVariance<<SEP
			<<feat.initial<<SEP
			<<feat.min<<SEP
			<<feat.max<<SEP
			<<feat.nbSamples<<endl;
	}
	m_file<<endl;
}

/// Save related images
void LogEvent::SaveImage()
{
	const Object& obj(m_event.GetObject());
	std::stringstream ss;
	if(obj.m_width > 0 && obj.m_height > 0)
	{
		ss << m_subId << "_" << m_currentTimeStamp << "_" << obj.GetName()<< obj.GetId() << "." << m_param.extension;
		imwrite(ss.str(), (*m_input)(obj.Rect()));
		ss.flush();
	}

	ss << m_subId << "_" << m_currentTimeStamp << "_" << obj.GetName()<< obj.GetId() << "." << m_param.extension;
	imwrite(ss.str(), *m_input);
}
