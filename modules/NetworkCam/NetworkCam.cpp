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
#include "StreamImage.h"
#include <errno.h>

#define TIMEOUT 3 // 3 sec timeout

using namespace std;
using namespace cv;

log4cxx::LoggerPtr NetworkCam::m_logger(log4cxx::Logger::getLogger("NetworkCam"));

struct struct_thread
{
	VideoCapture* capture;
	sem_t* sem;
	bool ret;
};


/// Specific thread dedicated to the reading of commands via stdin
void *grab_thread_nc(void *x_void_ptr)
{
	struct_thread* pst = reinterpret_cast<struct_thread*>(x_void_ptr);
	pst->ret = pst->capture->grab();
	sem_post(pst->sem);

	return NULL;
}


NetworkCam::NetworkCam(const ConfigReader& x_configReader):
	Input(x_configReader),
	m_param(x_configReader),
	m_output(Size(m_param.width, m_param.height), m_param.type)  // Note: sizes will be overridden !
{
	AddOutputStream(0, new StreamImage("input", m_output, *this, 		"Video stream of the camera"));
	m_isUnitTestingEnabled = false; // disable since it is not always possible to find a network camera
}

NetworkCam::~NetworkCam()
{
}

void NetworkCam::Reset()
{
	Module::Reset();
	m_capture.release();

	//open the video stream and make sure it's opened
	m_capture.open(m_param.url);

	if(! m_capture.isOpened())
	{
		throw MkException("Error : Network error, cannot open url : " + m_param.url, LOC);
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

bool NetworkCam::Grab()
{
	// Create a second thread to check if disconnected
	pthread_t thread;
	struct timespec my_timeout;
	struct timeval now;
	gettimeofday(&now,NULL);
	my_timeout.tv_sec  = now.tv_sec + TIMEOUT;
	my_timeout.tv_nsec = now.tv_usec * 1000;

	// create thread
	struct_thread st;
	st.capture = &m_capture;
	st.sem     = &m_semTimeout;
	pthread_create(&thread, NULL, grab_thread_nc, &st);
	int ret = sem_timedwait(&m_semTimeout, &my_timeout);

	if (ret==-1 && errno==ETIMEDOUT)
	{
		LOG_WARN(m_logger, "Timeout while grabbing stream. Camera may be disconnected.");
		pthread_cancel(thread);
		m_capture.release();
		// pthread_join(thread,NULL);

		return false;
	}
	else
	{
		pthread_join(thread,NULL);
		return st.ret;
	}
}

void NetworkCam::Capture()
{

	while(true)
	{
		if(Grab() != true)
		{
			LOG_WARN(m_logger, "Grab failure while reading stream, try to reopen in NetworkCam::Capture");
			sleep(3);
			Reset();
			if(Grab() != true)
			{
				m_endOfStream = true;
				throw MkException("Capture failed on network camera", LOC);
			}
		}

		m_capture.retrieve(m_output);

		// cout<<"NetworkCam capture image "<<m_output->cols<<"x"<<m_output->rows<<" time stamp "<<m_capture.get(CV_CAP_PROP_POS_MSEC) / 1000.0<< endl;

		// note: we use the time stamp of the modules since the official time stamp of the camera
		//    has a strange behavior
		m_currentTimeStamp = m_frameTimer.GetMSecLong();

		// only break out of the loop once we fulfill the fps criterion
		if(m_param.fps == 0 || (m_currentTimeStamp - m_lastTimeStamp) * m_param.fps > 1000)
			break;
	}

	LOG_DEBUG(m_logger, "NetworkCam: Capture time: "<<m_currentTimeStamp);
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
			  <<" FOURCC "<< static_cast<char>(cc & 0XFF) << static_cast<char>((cc & 0XFF00) >> 8)
			  << static_cast<char>((cc & 0XFF0000) >> 16) << static_cast<char>((cc & 0XFF000000) >> 24)
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
	// Normally this value should be given by m_capture.get(CV_CAP_PROP_FPS);
	// but generally the info is not contained inside the stream and CV_CAP_PROP_FPS
	// always equals 1000
	double fps = m_capture.get(CV_CAP_PROP_FPS);
	return fps == 1000.0 ? 8 : fps;
}
