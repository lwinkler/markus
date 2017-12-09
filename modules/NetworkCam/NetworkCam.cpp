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
#include "InterruptionManager.h"
#include <errno.h>
#include <util.h>
#include <future>
#include <thread>
#include <chrono>

#define TIMEOUT 3 // 3 sec timeout

namespace mk {
using namespace std;
using namespace cv;

log4cxx::LoggerPtr NetworkCam::m_logger(log4cxx::Logger::getLogger("NetworkCam"));

NetworkCam::NetworkCam(ParameterStructure& xr_params):
	Input(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params)),
	m_output(Size(m_param.width, m_param.height), m_param.type)  // Note: sizes will be overridden !
{
	AddOutputStream(0, new StreamImage("image", m_output, *this, 		"Video stream of the camera"));
	m_isUnitTestingEnabled = false; // disable since it is not always possible to find a network camera
	m_recordingFps = 0;
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
		throw MkException("Network, cannot open url : " + m_param.url, LOC);
	}

	// Normally this value should be given by m_capture.get(CV_CAP_PROP_FPS);
	// but generally the info is not contained inside the stream and CV_CAP_PROP_FPS
	// always equals 1000
	m_recordingFps = m_capture.get(CV_CAP_PROP_FPS);
	m_recordingFps = (m_recordingFps == 1000.0 || m_recordingFps != m_recordingFps /*Nan*/) ? 8 : m_recordingFps;

	// Apparently you cannot set width and height. We try anyway
	m_capture.set(CV_CAP_PROP_FRAME_WIDTH,  m_param.width);
	m_capture.set(CV_CAP_PROP_FRAME_HEIGHT, m_param.height);

	if(m_logger->isDebugEnabled())
		GetProperties();

	// note on the next line: the image will be overloaded but the properties are used to set the input ratio, the type is probably ignored
	m_output = Mat(Size(m_capture.get(CV_CAP_PROP_FRAME_WIDTH), m_capture.get(CV_CAP_PROP_FRAME_HEIGHT)), m_param.type);
}

bool NetworkCam::Grab()
{
	std::future<bool> ret = std::async(std::launch::async, [this]()
	{
		return m_capture.grab();
	});

	std::future_status status = ret.wait_for(std::chrono::seconds(TIMEOUT));
	if (status != std::future_status::ready)
	{
		throw GrabFreezeException("Timeout while grabbing frame. Camera may be disconnected.", LOC);
	}
	return ret.get();
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
				throw VideoStreamException("Capture failed on network camera", LOC);
			}
		}

		m_capture.retrieve(m_output);

		if(m_param.checkAspectRatio)
		{
			if(m_output.cols * m_param.height != m_param.width * m_output.rows)
			{
				stringstream ss;
				ss << "Aspect ratio of acquired image " << m_output.size() << " is inconsistant with " << GetSize();
				LOG_ERROR(m_logger, ss.str() << ": send rebuild command to the manager");
				InterruptionManager::GetInst().AddCommand(Command("manager.aspect_ratio.Set", convertAspectRatio(m_output.size())));
				InterruptionManager::GetInst().AddCommand(Command("manager.manager.Rebuild", ""));
				throw VideoStreamException(ss.str(), LOC);
			}
		}

		// cout<<"NetworkCam capture image "<<m_output->cols<<"x"<<m_output->rows<<" time stamp "<<m_capture.get(CV_CAP_PROP_POS_MSEC) / 1000.0<< endl;

		// note: we use the time stamp of the modules since the official time stamp of the camera
		//    has a strange behavior
		m_currentTimeStamp = getAbsTimeMs();

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

double NetworkCam::GetRecordingFps() const
{
	if(m_param.fps == 0)
		return m_recordingFps;
	return MIN(m_param.fps, m_recordingFps);
}
} // namespace mk
