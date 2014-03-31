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

log4cxx::LoggerPtr LogEvent::m_logger(log4cxx::Logger::getLogger("LogEvent"));

LogEvent::LogEvent(const ConfigReader& x_configReader) 
	 : Module(x_configReader), m_param(x_configReader),
	m_input(Size(m_param.width, m_param.height), m_param.type)
{
	m_description = "This module takes an event as input and logs it to .srt file";

	// Init input images
	AddInputStream(0, new StreamEvent("event", m_event, *this, "Input event to be logged"));
	AddInputStream(1, new StreamImage("image", m_input, *this, "Video input for image extraction"));

	m_saveImage = false;
}

LogEvent::~LogEvent(void)
{
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
		throw MkException("Impossible to open file " + m_srtFileName, LOC);
	m_saveImage = m_inputStreams.at(1)->IsConnected();

	m_folderName  = Manager::OutputDir() + "/" + m_param.folder + "/"; 
	SYSTEM("mkdir -p " + m_folderName);
}

void LogEvent::ProcessFrame()
{
	if(m_event.IsRaised())
	{
		// Log the change in event
		WriteEvent();
		// LOG_EVENT(m_logger, m_event.GetLabel()); 
		m_event.Notify("");
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
	if(obj.width > 0 && obj.height > 0)
	{
		std::stringstream ss1;
		ss1 << m_folderName << m_subId << "_" << m_currentTimeStamp << "_" << m_event.GetLabel() << "_" << obj.GetName()<< obj.GetId() << "." << m_param.extension;
		// cout<<"Save image "<<obj.m_posX<<" "<<obj.m_posY<<endl;
		imwrite(ss1.str(), (m_input)(obj.Rect()));
		//ss1.flush();
	}

	std::stringstream ss2;
	ss2 << m_folderName << m_subId << "_" << m_currentTimeStamp << "_" << m_event.GetLabel() << "_global." << m_param.extension;
	imwrite(ss2.str(), m_input);
}

/// Overwrite this function to process only the input for frames with an event
///	this is a small hack to speed up the time spent processing the inputs
/// 	there are two reason why we want to process: either the event is raised or the previous frame had a raised event
bool LogEvent::IsInputProcessed() const
{
	const StreamEvent* pStream =  dynamic_cast<StreamEvent*>(&m_inputStreams.at(0)->RefConnected());
	assert(pStream != NULL);
	return m_event.IsRaised() || pStream->GetEvent().IsRaised();
}
