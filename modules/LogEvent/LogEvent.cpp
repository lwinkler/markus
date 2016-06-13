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

using namespace std;
using namespace cv;

log4cxx::LoggerPtr LogEvent::m_logger(log4cxx::Logger::getLogger("LogEvent"));

LogEvent::LogEvent(ParameterStructure& xr_params)
	: Module(xr_params), m_param(dynamic_cast<Parameters&>(xr_params)),
	  m_inputIm1(Size(m_param.width, m_param.height), m_param.type),
	  m_inputIm2(Size(m_param.width, m_param.height), m_param.type) // TODO: All of these second streams should be black and white normally
{
	// Init input images
	AddInputStream(0, new StreamEvent("event", m_event, *this, "Input event to be logged"));
	AddInputStream(1, new StreamImage("image", m_inputIm1, *this, "Video input for image extraction (optional)"));
	AddInputStream(2, new StreamImage("mask" , m_inputIm2, *this, "Binary mask for image extraction (optional)"));

	mp_annotationWriter = nullptr;
}

LogEvent::~LogEvent()
{
	CLEAN_DELETE(mp_annotationWriter);
	CompareWithGroundTruth();
}

void LogEvent::Reset()
{
	Module::Reset();
	m_event.Clean();

	CLEAN_DELETE(mp_annotationWriter);
	mp_annotationWriter = new AnnotationFileWriter();
	mp_annotationWriter->Open(RefContext().RefOutputDir().ReserveFile(m_param.file, m_nbReset));
	m_saveImage1 = m_inputStreams.at(1)->IsConnected();
	m_saveImage2 = m_inputStreams.at(2)->IsConnected();

	mp_outputDir.reset(new MkDirectory(m_param.folder, RefContext().RefOutputDir(), false));
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

	ss<<m_event.SerializeToString(mp_outputDir.get());
	mp_annotationWriter->WriteAnnotation(m_currentTimeStamp, m_currentTimeStamp + 1000 * m_param.duration, ss);
}

/// Save related images
void LogEvent::SaveImage(Event& xr_event) const
{
	const Object& obj(xr_event.GetObject());

	if(m_saveImage1)
	{
		std::stringstream ss1;
		ss1 << m_currentTimeStamp << "_" << xr_event.GetEventName() << "_global_1." << m_param.extension;
		addExternalImage(m_inputIm1, "globalImage", mp_outputDir->ReserveFile(ss1.str()), xr_event);

		if(obj.width > 0 && obj.height > 0)
		{
			std::stringstream ss2;
			ss2 << m_currentTimeStamp << "_" << xr_event.GetEventName() << "_" << obj.GetName()<< obj.GetId() << "_1" << "." << m_param.extension;
			// cout<<"Save image "<<obj.m_posX<<" "<<obj.m_posY<<endl;
			addExternalImage((m_inputIm1)(obj.GetRect()), "objectImage", mp_outputDir->ReserveFile(ss2.str()), xr_event);
		}
	}

	if(m_saveImage2)
	{
		std::stringstream ss1;
		ss1 << m_currentTimeStamp << "_" << xr_event.GetEventName() << "_global_2." << m_param.extension;
		addExternalImage(m_inputIm2, "globalMask", mp_outputDir->ReserveFile(ss1.str()), xr_event);

		if(obj.width > 0 && obj.height > 0)
		{
			std::stringstream ss2;
			ss2 << m_currentTimeStamp << "_" << xr_event.GetEventName() << "_" << obj.GetName()<< obj.GetId() << "_2" << "." << m_param.extension;
			// cout<<"Save image "<<obj.m_posX<<" "<<obj.m_posY<<endl;
			addExternalImage((m_inputIm2)(obj.GetRect()), "objectMask",  mp_outputDir->ReserveFile(ss2.str()), xr_event);
		}
	}
}

/// Compare the events previously detected with the ground truth file
void LogEvent::CompareWithGroundTruth()
{
	if(m_param.gtCommand.empty())
		return;
	try
	{
		MkDirectory dir("analysis", RefContext().RefOutputDir(), false);
		const string& outDir(dir.GetPath());
		if(!m_param.gtFile.empty())
			RefContext().RefOutputDir().Cp(m_param.gtFile, "analysis/" + basename(m_param.gtFile));
		stringstream cmd;
		cmd<< m_param.gtCommand << " " << RefContext().RefOutputDir().GetPath() << "/" << m_param.file;
		// if(m_param.gtFile != "")
		cmd<< " " << outDir << "/" << basename(m_param.gtFile);
		cmd<< " --html --no-browser -o " << outDir;
		if(m_param.gtVideo != "")
			cmd<<" -i -V "<<m_param.gtVideo;

		// Save command for later use
		ofstream ofs(RefContext().RefOutputDir().ReserveFile("eval.%d.sh", m_nbReset), ios_base::app);
		ofs << cmd.str() << endl;

		LOG_DEBUG(m_logger, "Execute cmd: " + cmd.str());
		SYSTEM(cmd.str());

		// Iterate over all files created by the command
		boost::filesystem::directory_iterator end_iter;
		for(boost::filesystem::directory_iterator dir_iter(outDir) ; dir_iter != end_iter ; ++dir_iter)
		{
			if(boost::filesystem::is_regular_file(dir_iter->status()))
			{
				stringstream ss;
				ss << "analysis/" << dir_iter->path().filename().string();
				RefContext().RefOutputDir().ReserveFile(ss.str());
			}
		}
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
	return m_event.IsRaised() || pStream->GetContent().IsRaised();
}
