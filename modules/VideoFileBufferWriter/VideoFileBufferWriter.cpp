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

#include "VideoFileBufferWriter.h"
#include <thread>
#include "StreamState.h"
#include "StreamEvent.h"
#include "util.h"
#include "Manager.h"

namespace mk {

using namespace std;
using namespace cv;

log4cxx::LoggerPtr VideoFileBufferWriter::m_logger(log4cxx::Logger::getLogger("VideoFileBufferWriter"));

VideoFileBufferWriter::VideoFileBufferWriter(ParameterStructure& xr_params):
	VideoFileWriter(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params)),
	m_buffer1(m_param.bufferFramesBefore),
	m_buffer2(m_param.bufferFramesBefore + 100),
	m_threadIsWorking(false)
{
	// AddInputStream(0, new StreamImage("image", m_input, *this,   "Video input"));
	AddInputStream(1, new StreamState("trigger", m_trigger, *this,  "Trigger to start/stop of the recording (e.g. motion)"));
	AddInputStream(2, new StreamEvent("event", m_event,     *this,  "Event to which the record will be linked"));

	AddOutputStream(0, new StreamEvent("event", m_event,    *this,  "Event to which the record will be linked"));

	m_recording = false;
	m_eraseFile  = false;
	m_endOfRecord = 0;
}

VideoFileBufferWriter::~VideoFileBufferWriter()
{
	CloseFile();
}

void VideoFileBufferWriter::WaitForThread() const
{
	while(m_threadIsWorking)
	{
		LOG_WARN(m_logger, "Thread is still working. Waiting 0.1 sec.");
		usleep(100000);
	}
}

void VideoFileBufferWriter::Reset()
{
	// note: we do not want to call the Reset of VideoFileWriter since the video file is opened later
	Module::Reset();
	CloseFile();
	m_buffer1.clear();
	m_buffer2.clear();
}

void VideoFileBufferWriter::CloseFile()
{
	// finalize
	WaitForThread();
	if(!m_writer.isOpened())
		m_writer.release();
	if (m_eraseFile && !m_fileName.empty())
	{
		LOG_DEBUG(m_logger, "Delete file "<<m_fileName);
		if (remove(m_fileName.c_str()) != 0)
			LOG_WARN(m_logger, "Error deleting temporary video file named " << m_fileName);
	}
	m_recording  = false;
	m_eraseFile  = !m_param.keepAllRecordings;
	m_fileName   = "";
}

void VideoFileBufferWriter::OpenNewFile()
{
	if(m_param.fourcc.size() != 4)
		throw MkException("Error in parameter: fourcc must have 4 characters in VideoFileBufferWriter::Reset", LOC);
	const char * s = m_param.fourcc.c_str();
	// The color flag seem to be supported on Windows only
	// http://docs.opencv.org/modules/highgui/doc/reading_and_writing_images_and_video.html#videowriter-videowriter
	bool isColor = true;
	assert(m_param.type == CV_8UC3);

	stringstream ss;
	ss << m_param.file  << "." << m_currentTimeStamp << "." << ExtensionFromFourcc(m_param.fourcc);
	m_fileName = RefContext().RefOutputDir().ReserveFile(ss.str());
	double fps = 12;

	try
	{
		fps = GetRecordingFps();
		if(fps == 0)
			fps = 8; // default 8 fps
	}
	catch(exception& e)
	{
		// This may happen if the module is not connected
		LOG_WARN(m_logger, "Impossible to acquire the fps value for recording in VideoFileBufferWriter::Reset. Set to default value " << fps << ". Reason: " << e.what());
	}

	LOG_DEBUG(m_logger, "Start recording file "<<m_fileName<<" with fps="<<fps<<" and size "<<m_param.width<<"x"<<m_param.height);
	WaitForThread();
	m_writer.open(m_fileName, CV_FOURCC(s[0], s[1], s[2], s[3]), fps, Size(m_param.width, m_param.height), isColor);
	if(!m_writer.isOpened())
	{
		throw MkException("Failed to open output video file in VideoFileBufferWriter::Reset", LOC);
	}

}

void VideoFileBufferWriter::ProcessFrame()
{
	LOG_DEBUG(m_logger, "Recording=" << m_recording << " until " << m_endOfRecord << " (" << (m_endOfRecord - m_currentTimeStamp) / 1000.0
			  << "s) trigger=" << m_trigger << " event=" << m_event.IsRaised() << " thread=" << m_threadIsWorking);

	// We are always buffering
	m_buffer1.push_back(Mat());
	m_input.copyTo(m_buffer1.back());

	if(m_recording)
	{
		m_mutex.lock();
		if(!m_threadIsWorking)
		{
			// Thread is complete: write directly
			m_writer.write(m_input);
		}
		else
		{
			// Thread is working: add to buffer
			if(m_buffer2.size() == m_buffer2.max_size())
				LOG_ERROR(m_logger, "Buffer is not being written fast enough to disk, loosing frames");
			m_buffer2.push_back();
			m_input.copyTo(m_buffer2.back());
		}
		m_mutex.unlock();

		if(m_trigger || m_event.IsRaised())
			m_endOfRecord = m_currentTimeStamp + m_param.bufferDurationAfter * 1000;

		// We are recording (motion) to disk
		if(m_currentTimeStamp >= m_endOfRecord)
		{
			CloseFile();
			m_recording = false;
		}
	}
	else
	{
		// If there is motion or if an event occurs, start recording
		if(m_trigger || m_event.IsRaised())
		{
			// Write the buffer to disk
			OpenNewFile();
			LOG_DEBUG(m_logger, "Open " + m_fileName + " for " + to_string(m_buffer1.size()) + " frames");
			// m_buffer2 = boost::circular_buffer<Mat>(m_buffer1);
			for(const auto elem : m_buffer1)
			{
				m_buffer2.push_back();
				elem.copyTo(m_buffer2.back());
			}
			assert(!m_buffer1.empty());
			assert(!m_buffer2.empty());
			LOG_DEBUG(m_logger, "Starting thread");
			m_threadIsWorking = true; // note: must be set here to handle case where destructor is called

			std::thread t([this]()
			{
				// start new thread
				while(true)
				{
					m_mutex.lock();
					if(m_buffer2.empty())
					{
						m_mutex.unlock();
						break;
					}
					m_writer.write(m_buffer2.front());
					m_buffer2.pop_front();

					m_mutex.unlock();
				}
				m_threadIsWorking = false;
			});
			t.detach();
			m_endOfRecord = m_currentTimeStamp + m_param.bufferDurationAfter * 1000;
			m_recording = true;
		}
	}

	// Add the file to the event
	if(m_event.IsRaised())
	{
		m_event.AddExternalFile("record", m_fileName);
		LOG_DEBUG(m_logger, "Add external record to event: " << m_fileName);
		m_eraseFile = false;
	}
}

} // namespace mk
