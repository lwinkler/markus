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

#include "ReadEvent.h"
#include "StreamEvent.h"
#include "StreamImage.h"
#include "util.h"
#include "Manager.h"

#define SEP "\t"

using namespace std;
using namespace cv;

log4cxx::LoggerPtr ReadEvent::m_logger(log4cxx::Logger::getLogger("ReadEvent"));

ReadEvent::ReadEvent(const ConfigReader& x_configReader) 
	 : Input(x_configReader), m_param(x_configReader),
	m_outputIm1(Size(m_param.width, m_param.height), m_param.type),
	m_outputIm2(Size(m_param.width, m_param.height), m_param.type)
{
	// Init input images
	AddOutputStream(0, new StreamEvent("event", m_event, *this, "Input event to be logged"));
	AddOutputStream(1, new StreamImage("image", m_outputIm1, *this, "Video output for image extraction (optional)"));
	AddOutputStream(2, new StreamImage("mask" , m_outputIm2, *this, "Binary mask for image extraction (optional)"));

	mp_annotationReader = NULL;
}

ReadEvent::~ReadEvent(void)
{
	CLEAN_DELETE(mp_annotationReader);
}

void ReadEvent::Reset()
{
	Input::Reset();
	m_event.Empty();
	
	CLEAN_DELETE(mp_annotationReader);
	mp_annotationReader = new AnnotationFileReader();
	mp_annotationReader->Open(m_param.file);
}

void ReadEvent::ProcessFrame()
{
	string text;
	if(!mp_annotationReader->ReadNextAnnotation(text))
	{
		m_endOfStream = true;
		Pause(true);
		throw EndOfStreamException("Cannot read next annotation", LOC);
	}
	//TODO m_currentTimeStamp = m_capture.get(CV_CAP_PROP_POS_MSEC);
	stringstream ss;
	ss<<text;
	m_event.Deserialize(ss, m_param.folder);
}

/// Save related images
/*
void ReadEvent::SaveImage(Event& x_event)
{
	const Object& obj(m_event.GetObject());

	if(m_saveImage1)
	{
		std::stringstream ss1;
		ss1 << m_folderName << m_subId << "_" << m_currentTimeStamp << "_" << m_event.GetEventName() << "_global_1." << m_param.extension;
		AddExternalImage(m_inputIm1, "globalImage", ss1.str(), x_event);

		if(obj.width > 0 && obj.height > 0)
		{
			std::stringstream ss2;
			ss2 << m_folderName << m_subId << "_" << m_currentTimeStamp << "_" << m_event.GetEventName() << "_" << obj.GetName()<< obj.GetId() << "_1" << "." << m_param.extension;
			// cout<<"Save image "<<obj.m_posX<<" "<<obj.m_posY<<endl;
			Mat img = (m_inputIm1)(obj.Rect());
			AddExternalImage((m_inputIm1)(obj.Rect()), "objectImage", ss2.str(), x_event);
		}
	}

	if(m_saveImage2)
	{
		std::stringstream ss1;
		ss1 << m_folderName << m_subId << "_" << m_currentTimeStamp << "_" << m_event.GetEventName() << "_global_2." << m_param.extension;
		AddExternalImage(m_inputIm2, "globalMask", ss1.str(), x_event);

		if(obj.width > 0 && obj.height > 0)
		{
			std::stringstream ss2;
			ss2 << m_folderName << m_subId << "_" << m_currentTimeStamp << "_" << m_event.GetEventName() << "_" << obj.GetName()<< obj.GetId() << "_2" << "." << m_param.extension;
			// cout<<"Save image "<<obj.m_posX<<" "<<obj.m_posY<<endl;
			Mat img = (m_inputIm2)(obj.Rect());
			AddExternalImage(img, "objectMask", ss2.str(), x_event);
		}
	}
}

/// Store the extra information necessary along with the event
void ReadEvent::AddExternalImage(const Mat& x_image, const std::string& x_name, const std::string& x_file, Event& x_event)
{
	imwrite(x_file, x_image);
	x_event.AddExternalFile(x_name, x_file);
}
*/
