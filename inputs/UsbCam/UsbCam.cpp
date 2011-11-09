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

using namespace std;

UsbCam::UsbCam(const std::string& x_name, ConfigReader& x_configReader): 
	m_param(x_configReader, x_name), 
	Input(x_name, x_configReader),
	m_name(x_name)
{
	m_capture = NULL;
	m_capture = cvCreateCameraCapture( m_param.num );
	m_fps = 0;
	
	if(m_capture == NULL)
	{
		throw("Error : UsbCam not found ! : " + x_name);
	}
	//cout<<"Setting "<<m_param.width<<endl;
	
	// Get capture device information
	//cvSetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_WIDTH, m_param.width); // not working
	//cvSetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_HEIGHT, m_param.height);
	cvQueryFrame(m_capture); // this call is necessary to get correct capture properties
	m_width    = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_WIDTH);
	m_height   = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_HEIGHT);
	//int numFramesc = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_COUNT);
	
//	cout<<"done Setting "<<m_width<<" "<<m_height<<endl;
//	assert(m_width == m_param.width);
//	assert(m_height == m_param.height);
	
	
	m_input = NULL;//cvCreateImage( cvSize(m_width, m_height), IPL_DEPTH_8U, 3);
}

UsbCam::~UsbCam()
{
	//cvReleaseImage(&m_input);
	cvReleaseCapture(&m_capture );
}


void UsbCam::Capture()
{
	//Get frame information:
	//double posMsec   =       cvGetCaptureProperty(m_capture, CV_CAP_PROP_POS_MSEC);
	//int posFrames    = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_POS_FRAMES);
	//double posRatio  =       cvGetCaptureProperty(m_capture, CV_CAP_PROP_POS_AVI_RATIO);

	m_lock.lockForWrite();
	cvGrabFrame(m_capture);
	m_input = cvRetrieveFrame(m_capture);           // retrieve the captured frame

	m_lock.unlock();
}
