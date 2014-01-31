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

#include "UsbCam.h"
// #include "util.h"
#include "StreamImage.h"

using namespace std;
using namespace cv;

UsbCam::UsbCam(const ConfigReader& x_configReader): 
	Input(x_configReader),
    m_param(x_configReader)
{
	m_timeStamp = TIME_STAMP_INITIAL;
	
	m_output = new Mat(Size(m_param.width, m_param.height), m_param.type);  // Note: sizes will be overridden !
	m_outputStreams.push_back(new StreamImage(0, "input", m_output, *this, 		"Video stream of the camera"));
}

UsbCam::~UsbCam()
{
	delete(m_output);
}

void UsbCam::Reset()
{
	m_lock.lockForRead(); //TODO remove ?
	Module::Reset();

	m_capture.release();
	m_capture.open(m_param.num);
	// TODO if(Global::logger.HasDebugMode())
		GetProperties();
	
	if(! m_capture.isOpened())
	{
		throw MkException("Cannot open USB or local camera number " + m_param.num, LOC);
	}

	// Apparently you cannot set width and height. We try anyway
	m_capture.set(CV_CAP_PROP_FRAME_WIDTH,  m_param.width);
	m_capture.set(CV_CAP_PROP_FRAME_HEIGHT, m_param.height);
	
	// note on the next line: the image will be overloaded but the properties are used to set the input ratio, the type is probably ignored
	delete m_output;
	m_output = new Mat(Size(m_capture.get(CV_CAP_PROP_FRAME_WIDTH), m_capture.get(CV_CAP_PROP_FRAME_HEIGHT)), m_param.type);


	m_frameTimer.Restart();
	m_lock.unlock(); // TODO remove ?
}

void UsbCam::Capture()
{
	if(m_capture.grab() == 0)
	{
		m_endOfStream = true;
		std::exception e;
		Pause(true);
		throw MkException("Capture failed on USB camera", LOC);
	}

	m_capture.retrieve(*m_output);
	
	// cout<<"UsbCam capture image "<<m_output->cols<<"x"<<m_output->rows<<" time stamp "<<m_capture.get(CV_CAP_PROP_POS_MSEC) / 1000.0<< endl;

	// time_t rawtime;
	// time(&rawtime);
	LOG_DEBUG(m_logger, "UsbCam: Capture time: "<<m_frameTimer.GetMSecLong());
	SetTimeStampToOutputs(m_frameTimer.GetMSecLong());
}

void UsbCam::GetProperties()
{
	LOG_DEBUG(m_logger, "FRAME_WIDTH "<<m_capture.get(CV_CAP_PROP_FRAME_WIDTH)
		<<" FRAME_HEIGHT "<<m_capture.get(CV_CAP_PROP_FRAME_HEIGHT)
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
