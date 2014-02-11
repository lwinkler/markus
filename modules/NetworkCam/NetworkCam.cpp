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

#include "NetworkCam.h"
// #include "util.h"
#include "StreamImage.h"

using namespace std;
using namespace cv;

NetworkCam::NetworkCam(const ConfigReader& x_configReader): 
	Input(x_configReader),
	m_param(x_configReader),
	m_output(Size(m_param.width, m_param.height), m_param.type)  // Note: sizes will be overridden !
{
	m_description = "Module to acquire a video stream from a network camera";
	m_timeStamp = TIME_STAMP_INITIAL;
	
	AddOutputStream(0, new StreamImage("input", m_output, *this, 		"Video stream of the camera"));
}

NetworkCam::~NetworkCam()
{
}

void NetworkCam::Reset()
{
	Module::Reset();
	m_capture.release();

	//open the video stream and make sure it's opened
	/* it may be an address of an mjpeg stream, 
	e.g. "http://user:pass@cam_address:8081/cgi/mjpg/mjpg.cgi?.mjpg" 
	"rtsp://cam_address:554/live.sdp" rtsp://<servername>/axis-media/media.amp */
	if(m_param.url.size() == 0)
		m_capture.open("in/input.mp4");
	else m_capture.open(m_param.url);
	
	if(! m_capture.isOpened())
	{
		throw MkException("Error : Network cannot open url : " + m_param.url, LOC);
	}

	// Apparently you cannot set width and height. We try anyway
	m_capture.set(CV_CAP_PROP_FRAME_WIDTH,  m_param.width);
	m_capture.set(CV_CAP_PROP_FRAME_HEIGHT, m_param.height);
	
	if(m_logger->isDebugEnabled())
		GetProperties();

	// note on the next line: the image will be overloaded but the properties are used to set the input ratio, the type is probably ignored
	m_output = Mat(Size(m_capture.get(CV_CAP_PROP_FRAME_WIDTH), m_capture.get(CV_CAP_PROP_FRAME_HEIGHT)), m_param.type);

	m_frameTimer.Restart();
}

void NetworkCam::Capture()
{
	while(true)
	{
		if(m_capture.grab() == 0)
		{
			LOG_WARN(m_logger, "Grab failure while reading stream, try to reopen in NetworkCam::Capture");
			Reset();
			if(m_capture.grab() == 0)
			{
				m_endOfStream = true;
				Pause(true);
				throw MkException("Capture failed on network camera", LOC);
			}
		}

		m_capture.retrieve(m_output);
		
		// cout<<"NetworkCam capture image "<<m_output->cols<<"x"<<m_output->rows<<" time stamp "<<m_capture.get(CV_CAP_PROP_POS_MSEC) / 1000.0<< endl;

		// note: we use the time stamp of the modules since the official time stamp of the camera
		//    has a strange behavior
		m_currentTimeStamp = m_frameTimer.GetMSecLong();
		// cout<<m_capture.get(CV_CAP_PROP_POS_MSEC)<<endl;

		// only break out of the loop once we fulfill the fps criterion
		if(m_param.fps == 0 || (m_currentTimeStamp - m_lastTimeStamp) * m_param.fps > 1000)
			break;
	}
	// cout<<m_currentTimeStamp<<" - "<<m_lastTimeStamp<<endl;

	LOG_DEBUG(m_logger, "NetworkCam: Capture time: "<<m_currentTimeStamp);
	// SetTimeStampToOutputs(m_currentTimeStamp);
}

void NetworkCam::GetProperties()
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
	// 	<<" WHITE_BALANCE"<<m_capture.get(CV_CAP_PROP_WHITE_BALANCE)
		<<" RECTIFICATION"<<m_capture.get(CV_CAP_PROP_RECTIFICATION));
}

double NetworkCam::GetRecordingFps()
{
	return 8; //  m_capture.get(CV_CAP_PROP_FPS);	// TODO remove hack
}
