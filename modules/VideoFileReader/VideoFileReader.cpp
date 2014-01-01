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

#include "VideoFileReader.h"
#include "StreamImage.h"

#ifndef MARKUS_NO_GUI
#include "InputStreamControl.h"
#endif

using namespace std;
using namespace cv;

VideoFileReader::VideoFileReader(const ConfigReader& x_configReader): 
	Input(x_configReader),
	m_param(x_configReader)
{
	m_description = "Input from a video file.";
	m_output = new Mat(Size(m_param.width, m_param.height), CV_8UC3); // Note: sizes will be overridden !
	m_outputStreams.push_back(new StreamImage(0, "input", m_output, *this,	"Video stream"));


#ifndef MARKUS_NO_GUI
	// Add a new control to play forward and rewind
	// TODO m_controls.push_back(new InputStreamControl("Video file reader", "Control the reading of the video file"));
	// TODO m_controls.back()->SetModule(*module);
#endif
}

VideoFileReader::~VideoFileReader()
{
	delete(m_output);
}

void VideoFileReader::Reset()
{
	Module::Reset();

	m_capture.release();
	m_capture.open(m_param.file);
	// m_fps     = m_capture.get(CV_CAP_PROP_FPS);
	// TODO if(Global::logger.HasDebugMode())
		GetProperties();
	
	if(! m_capture.isOpened())
	{
		throw MkException("Error : VideoFileReader cannot open file : " + m_param.file, LOC);
	}

	// Apparently you cannot set width and height. We try anyway
	m_capture.set(CV_CAP_PROP_FRAME_WIDTH,  m_param.width);
	m_capture.set(CV_CAP_PROP_FRAME_HEIGHT, m_param.height);
	
	// note on the next line: the image will be overloaded but the properties are used to set the input ratio, the type is probably ignored
	delete m_output;
	m_output = new Mat(Size(m_capture.get(CV_CAP_PROP_FRAME_WIDTH), m_capture.get(CV_CAP_PROP_FRAME_HEIGHT)), m_param.type);
}

void VideoFileReader::Capture()
{
	if(m_capture.grab() == 0)
	{
		// Note: there seems to be a 3 seconds lag when grabbing after the last frame. This is linked to format h264: MJPG is ok
		m_endOfStream = true;
		std::exception e;
		Pause(true);
		throw EndOfStreamException("Capture failed in VideoFileReader::Capture.", LOC);
	}

	m_capture.retrieve(*m_output);
	
	// cout<<"VideoFileReader capture image "<<m_output->cols<<"x"<<m_output->rows<<" time stamp "<<m_capture.get(CV_CAP_PROP_POS_MSEC) / 1000.0<< endl;

	SetTimeStampToOutputs(m_capture.get(CV_CAP_PROP_POS_MSEC));
}

void VideoFileReader::GetProperties()
{
	int cc = static_cast<int>(m_capture.get(CV_CAP_PROP_FOURCC));

	LOG_DEBUG(m_logger, "POS_MSEC "<<m_capture.get(CV_CAP_PROP_POS_MSEC)
		<<" POS_FRAMES "<<m_capture.get(CV_CAP_PROP_POS_FRAMES)
		<<" POS_AVI_RATIO "<<m_capture.get(CV_CAP_PROP_POS_AVI_RATIO)
		<<" FRAME_WIDTH "<<m_capture.get(CV_CAP_PROP_FRAME_WIDTH)
		<<" FRAME_HEIGHT "<<m_capture.get(CV_CAP_PROP_FRAME_HEIGHT)
		<<" FPS "<<m_capture.get(CV_CAP_PROP_FPS)
		<<" FOURCC "<< (char)(cc & 0XFF) << (char)((cc & 0XFF00) >> 8) << (char)((cc & 0XFF0000) >> 16) << (char)((cc & 0XFF000000) >> 24) 
		<<" FRAME_COUNT "<<m_capture.get(CV_CAP_PROP_FRAME_COUNT)
		<<" FORMAT "<<m_capture.get(CV_CAP_PROP_FORMAT)
		<<" MODE "<<m_capture.get(CV_CAP_PROP_MODE)
		<<" BRIGHTNESS "<<m_capture.get(CV_CAP_PROP_BRIGHTNESS)
		<<" CONTRAST "<<m_capture.get(CV_CAP_PROP_CONTRAST)
		<<" SATURATION "<<m_capture.get(CV_CAP_PROP_SATURATION)
		<<" HUE "<<m_capture.get(CV_CAP_PROP_HUE)
		<<" GAIN "<<m_capture.get(CV_CAP_PROP_GAIN)
		<<" EXPOSURE "<<m_capture.get(CV_CAP_PROP_EXPOSURE)
		<<" CONVERT_RGB "<<m_capture.get(CV_CAP_PROP_CONVERT_RGB)
	// 	<<" WHITE_BALANCE"<<m_capture.get(CV_CAP_PROP_WHITE_BALANCE)<<endl;
		<<" RECTIFICATION"<<m_capture.get(CV_CAP_PROP_RECTIFICATION));
}

// Set reading time in msec
void VideoFileReader::SetMsec(int x_msec)
{
	m_capture.set(CV_CAP_PROP_POS_MSEC, x_msec);	
}

// Set reading time in frames
void VideoFileReader::SetFrame(int x_frame)
{
	m_capture.set(CV_CAP_PROP_POS_FRAMES, x_frame);	
}

// Get reading time in msec
int VideoFileReader::GetMsec()
{
	return m_capture.get(CV_CAP_PROP_POS_MSEC);	
}

// Get reading time in frames
int VideoFileReader::GetFrame()
{
	return m_capture.get(CV_CAP_PROP_POS_FRAMES);	
}

// Get reading time in msec
int VideoFileReader::GetMaxMsec()
{
	return 1000 * m_capture.get(CV_CAP_PROP_FRAME_COUNT) / m_capture.get(CV_CAP_PROP_FPS);	
}

// Get reading time in frames
int VideoFileReader::GetMaxFrame()
{
	return m_capture.get(CV_CAP_PROP_FRAME_COUNT);	
}


double VideoFileReader::GetRecordingFps()
{
	return m_capture.get(CV_CAP_PROP_FPS);	
}
