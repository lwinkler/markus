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
#include "util.h"
#include "StreamImage.h"

using namespace std;
using namespace cv;

UsbCam::UsbCam(const ConfigReader& x_configReader): 
	Input(x_configReader),
	m_param(x_configReader)
{
	
	m_capture = NULL;
	m_capture = cvCreateCameraCapture( m_param.num );
	m_fps = 0;
	
	if(m_capture == NULL)
	{
		throw("Error : UsbCam not found ! : " + m_name);
	}
	//cout<<"Setting "<<m_param.width<<endl;
	
	// Get capture device information
	//cvSetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_WIDTH, m_param.width); // not working
	//cvSetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_HEIGHT, m_param.height);
	//cvQueryFrame(m_capture); // this call is necessary to get correct capture properties
	//IplImage * tmp = cvRetrieveFrame(m_capture); // Test capture
	//m_inputWidth    = tmp->width;//(int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_WIDTH);
	//m_inputHeight   = tmp->height;//(int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_HEIGHT);
	cout<<"Capture from UsbCam with resolution "<<GetWidth()<<"x"<<GetHeight()<<endl;
	//int numFramesc = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_COUNT);
	
	m_output = new Mat( cvSize(m_param.width, m_param.height), m_param.type);
	m_outputStreams.push_back(new StreamImage(0, "input", m_output, *this, 		"Video stream of the camera"));

	//Reset();
}

UsbCam::~UsbCam()
{
	cvReleaseCapture(&m_capture );
	delete(m_output);
}

void UsbCam::Reset()
{
	Module::Reset();
	resize(*m_output, *m_output, cvSize(m_param.width, m_param.height));
}

void UsbCam::Capture()
{
	//Get frame information:
	//double posMsec   =       cvGetCaptureProperty(m_capture, CV_CAP_PROP_POS_MSEC);
	//int posFrames    = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_POS_FRAMES);
	//double posRatio  =       cvGetCaptureProperty(m_capture, CV_CAP_PROP_POS_AVI_RATIO);

	cvGrabFrame(m_capture);
	//m_input = cvRetrieveFrame(m_capture);           // retrieve the captured frame

	int frameH    = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_HEIGHT);
	int frameW    = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_WIDTH);	
	
	Mat * tmp = new Mat(cvRetrieveFrame(m_capture));
	//cout<<tmp->width<<" == "<<m_input->width<<endl;
	//assert(tmp->width == m_input->width);
	//assert(tmp->height == m_input->height);
	//assert(tmp->depth == m_input->depth);
	
	//tmp->copyTo(*m_output);
	adjustSize(tmp, m_output);
	
	delete(tmp);
}
