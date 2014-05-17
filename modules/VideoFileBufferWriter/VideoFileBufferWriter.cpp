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
#include "StreamState.h"
#include "util.h"
#include "Manager.h"

using namespace std;
using namespace cv;

log4cxx::LoggerPtr VideoFileBufferWriter::m_logger(log4cxx::Logger::getLogger("VideoFileBufferWriter"));

VideoFileBufferWriter::VideoFileBufferWriter(const ConfigReader& x_configReader): 
	VideoFileWriter(x_configReader),
	m_param(x_configReader)
{
	// AddInputStream(0, new StreamImage("input", m_input, *this,   "Video input"));
	AddInputStream(1, new StreamState("trigger", m_trigger, *this,  "Trigger to start/stop of the recording (e.g. motion)"));

	m_buffering = false;
	m_bufferFull = false;
	m_timeBufferFull = 0;
}

void VideoFileBufferWriter::Reset()
{
	Module::Reset();
	m_buffer.clear();
	m_timeBufferFull = 0;
	m_buffering  = false;
	m_bufferFull = false;
	m_currentFrame = m_buffer.end(); // Set to an invalid value
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
	ss << Manager::OutputDir() << "/" << m_param.file  << "." << m_index++ << "." << ExtensionFromFourcc(m_param.fourcc);
	const string filename = ss.str();
	double fps = 12;
	
	try
	{
		fps = GetRecordingFps();
	}
	catch(exception& e)
	{
		// This may happen if the module is not connected
		LOG_WARN(m_logger, "Impossible to acquire the fps value for recording in VideoFileBufferWriter::Reset. Set to default value " << fps << ". Reason: " << e.what());	
	}

	LOG_DEBUG(m_logger, "Start recording file "<<filename<<" with fps="<<fps<<" and size "<<m_param.width<<"x"<<m_param.height);
	m_writer.open(filename, CV_FOURCC(s[0], s[1], s[2], s[3]), fps, Size(m_param.width, m_param.height), isColor);
	if(!m_writer.isOpened())
	{
		throw MkException("Failed to open output video file in VideoFileBufferWriter::Reset", LOC);
	}

}

void VideoFileBufferWriter::ProcessFrame()
{
	if(!m_trigger)
	{
		if(!m_buffering)
		{
			// Start buffering
			m_writer.release();
			m_buffering = true;
			// Save the time stamp
			m_timeBufferFull = m_currentTimeStamp + m_param.bufferDuration * 1000;
		}
		AddImageToBuffer();
	}
	else
	{
		if(m_buffering)
		{
			OpenNewFile();

			// write buffer to file

			std::list<cv::Mat>::iterator bufferBegin = m_currentFrame;
			std::list<cv::Mat>::iterator bufferEnd   = m_currentFrame;
			if(!m_bufferFull)
			{
				bufferBegin = m_buffer.begin();
				bufferEnd   = m_buffer.end();
				
			}
			list<Mat>::iterator it = bufferBegin;
			while(true)
			{
				m_writer.write(*it);
				it->release();
				it++;
				if(it == bufferEnd)
					break;
				if(it == m_buffer.end())
					it = m_buffer.begin();
			}
			m_buffer.clear();
			m_buffering = false;
			m_bufferFull = false;
		}
		m_writer.write(m_input);
	}
}


void VideoFileBufferWriter::AddImageToBuffer()
{
	if(m_currentTimeStamp > m_timeBufferFull)
	{
		m_bufferFull = true;
		LOG4CXX_DEBUG(m_logger, "Buffer contains "<<m_buffer.size()<<" frames.");
	}

	if(m_bufferFull)
	{
		// Buffer is full. Use a circular buffer
		assert(m_buffer.size() > 0);
		m_input.copyTo(*m_currentFrame);

		m_currentFrame++;
		if(m_currentFrame == m_buffer.end())
			m_currentFrame = m_buffer.begin();
	}
	else
	{
		Mat im;
		m_buffer.push_back(im);
		m_input.copyTo(m_buffer.back());
		m_currentFrame = m_buffer.begin();
	}
}
