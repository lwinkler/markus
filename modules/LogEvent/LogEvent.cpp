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

LogEvent::LogEvent(ParameterStructure& xr_params)
	: Module(xr_params), m_param(dynamic_cast<Parameters&>(xr_params)),
	  m_inputIm1(Size(m_param.width, m_param.height), m_param.type),
	  m_inputIm2(Size(m_param.width, m_param.height), m_param.type)
{
	// Init input images
	AddInputStream(0, new StreamEvent("event", m_event, *this, "Input event to be logged"));
	AddInputStream(1, new StreamImage("image", m_inputIm1, *this, "Video input for image extraction (optional)"));
	AddInputStream(2, new StreamImage("mask" , m_inputIm2, *this, "Binary mask for image extraction (optional)"));

	m_saveImage1 = false;
	m_saveImage2 = false;
	mp_annotationWriter = nullptr;
}

LogEvent::~LogEvent(void)
{
	CLEAN_DELETE(mp_annotationWriter);
	CompareWithGroundTruth();
}

void LogEvent::Reset()
{
	Module::Reset();
	m_event.Clean();

	string srtFileName = GetContext().GetOutputDir() + "/" + m_param.file;
	CLEAN_DELETE(mp_annotationWriter);
	mp_annotationWriter = new AnnotationFileWriter();
	mp_annotationWriter->Open(srtFileName);
	m_saveImage1 = m_inputStreams.at(1)->IsConnected();
	m_saveImage2 = m_inputStreams.at(2)->IsConnected();

	m_folder  = GetContext().GetOutputDir() + "/" + m_param.folder + "/";
	SYSTEM("mkdir -p " + m_folder);
}

void LogEvent::ProcessFrame()
{
	if(m_event.IsRaised())
	{
		// Log the change in event
		SaveImage(m_event);
		WriteEvent();
		// LOG_EVENT(m_logger, m_event.GetEventName());
		m_event.Notify(GetContext());
	}
}

/// Write the subtitle in log file
void LogEvent::WriteEvent()
{
	LOG_DEBUG(m_logger, "Write event to log file");
	stringstream ss;

	// Log event label
	/* The old way
	ss<<m_event.GetEventName()<<endl;

	// Log event features with values
	if(m_event.GetFeatures().size() != 0)
		ss<<"time"<<SEP<<"object"<<SEP<<"feature"<<SEP<<"value"<<SEP<<"mean"<<SEP<<"sqVariance"<<SEP<<"initial"<<SEP<<"min"<<SEP<<"max"<<SEP<<"nbSamples"<<endl;
	for(map<string, Feature>::const_iterator it2 = m_event.GetFeatures().begin() ; it2 != m_event.GetFeatures().end() ; it2++)
	{
		const Feature & feat = it2->second;
		ss<<m_currentTimeStamp<<SEP
			<<m_event.GetObject().GetName()<<m_event.GetObject().GetId()<<SEP
			<<it2->first<<SEP
			<<feat.value<<SEP
			<<feat.mean<<SEP
			<<feat.sqVariance<<SEP
			<<feat.initial<<SEP
			<<feat.min<<SEP
			<<feat.max<<SEP
			<<feat.nbSamples<<endl;
	}*/
	// ss<<"\"event\":";
	ss<<m_event.SerializeToString(m_folder);
	mp_annotationWriter->WriteAnnotation(m_currentTimeStamp, m_currentTimeStamp + 1000 * m_param.duration, ss);
}

/// Save related images
void LogEvent::SaveImage(Event& x_event)
{
	const Object& obj(m_event.GetObject());

	if(m_saveImage1)
	{
		std::stringstream ss1;
		ss1 << m_folder << m_currentTimeStamp << "_" << m_event.GetEventName() << "_global_1." << m_param.extension;
		AddExternalImage(m_inputIm1, "globalImage", ss1.str(), x_event);

		if(obj.width > 0 && obj.height > 0)
		{
			std::stringstream ss2;
			ss2 << m_folder << m_currentTimeStamp << "_" << m_event.GetEventName() << "_" << obj.GetName()<< obj.GetId() << "_1" << "." << m_param.extension;
			// cout<<"Save image "<<obj.m_posX<<" "<<obj.m_posY<<endl;
			Mat img = (m_inputIm1)(obj.GetRect());
			AddExternalImage((m_inputIm1)(obj.GetRect()), "objectImage", ss2.str(), x_event);
		}
	}

	if(m_saveImage2)
	{
		std::stringstream ss1;
		ss1 << m_folder << m_currentTimeStamp << "_" << m_event.GetEventName() << "_global_2." << m_param.extension;
		AddExternalImage(m_inputIm2, "globalMask", ss1.str(), x_event);

		if(obj.width > 0 && obj.height > 0)
		{
			std::stringstream ss2;
			ss2 << m_folder << m_currentTimeStamp << "_" << m_event.GetEventName() << "_" << obj.GetName()<< obj.GetId() << "_2" << "." << m_param.extension;
			// cout<<"Save image "<<obj.m_posX<<" "<<obj.m_posY<<endl;
			Mat img = (m_inputIm2)(obj.GetRect());
			AddExternalImage(img, "objectMask", ss2.str(), x_event);
		}
	}
}

/// Store the extra information necessary along with the event
void LogEvent::AddExternalImage(const Mat& x_image, const std::string& x_name, const std::string& x_file, Event& x_event)
{
	imwrite(x_file, x_image);
	LOG_DEBUG(m_logger, "Add external file to event " << x_name << ": " << x_file);
	x_event.AddExternalFile(x_name, x_file);
}

/// Compare the events previously detected with the ground truth file
void LogEvent::CompareWithGroundTruth()
{
	if(m_param.gtCommand.empty())
		return;
	try
	{
		string outDir = GetContext().GetOutputDir() + "/analysis";
		SYSTEM("mkdir -p " + outDir);
		if(!m_param.gtFile.empty())
			SYSTEM("cp " + m_param.gtFile + " " + outDir);
		stringstream cmd;
		cmd<< m_param.gtCommand << " " << GetContext().GetOutputDir() << "/" << m_param.file;
		// if(m_param.gtFile != "")
		cmd<< " " << outDir << "/" << basename(m_param.gtFile);
		cmd<< " --html --no-browser -o " << outDir;
		if(m_param.gtVideo != "")
			cmd<<" -i -V "<<m_param.gtVideo;

		// Save command for later use
		string fileName = GetContext().GetOutputDir() + "/eval.sh";
		ofstream ofs(fileName.c_str(), ios_base::app);
		ofs << cmd.str() << endl;
		ofs.close();

		LOG_DEBUG(m_logger, "Execute cmd: " + cmd.str());
		SYSTEM(cmd.str());
	}
	catch(MkException& e)
	{
		stringstream ss;
		ss<<"Error while comparing to ground truth: "<<e.what();
		LOG_ERROR(m_logger, ss.str());
	}
}


/// Overwrite this function to process only the input for frames with an event
///	this is a trick to speed up the time spent processing the inputs
/// 	there are two reason why we want to process: either the event is raised or the previous frame had a raised event
bool LogEvent::IsInputProcessed() const
{
	const StreamEvent* pStream =  dynamic_cast<const StreamEvent*>(&m_inputStreams.at(0)->GetConnected());
	assert(pStream != nullptr);
	return m_event.IsRaised() || pStream->GetObject().IsRaised();
}
