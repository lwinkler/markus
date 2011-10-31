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

#include "Input.h"

using namespace std;

Input::Input(const std::string& x_name, ConfigReader& x_configReader): 
	m_param(x_configReader, x_name), 
	Configurable(x_configReader),
	m_name(x_name)
{
	cout<<"*** Create object Input : "<<x_name<<" ***"<<endl;

	m_capture = NULL;
	cout<<"Input "<< m_param.source<<endl;

	if(m_param.source == "cam")
	{
		cout<<"Capture from Cam"<<endl;
		m_capture = cvCaptureFromCAM( CV_CAP_ANY );
		m_fps = 0;
	}
	else
	{
		m_capture = cvCaptureFromFile(m_param.source.c_str());
		m_fps     = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_FPS);
	}

	if(m_capture == NULL)
	{
		throw("Error : Input or input file not found ! : " + m_param.source);
	}
	cout<<"Setting "<<m_param.width<<endl;
	
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
	
	
	m_input = cvCreateImage( cvSize(m_width, m_height), IPL_DEPTH_8U, 3);
}

Input::~Input()
{
	cvReleaseCapture(&m_capture );
	cvReleaseImage(&m_input);
}


void Input::Capture()
{
	//Get frame information:
	//double posMsec   =       cvGetCaptureProperty(m_capture, CV_CAP_PROP_POS_MSEC);
	//int posFrames    = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_POS_FRAMES);
	//double posRatio  =       cvGetCaptureProperty(m_capture, CV_CAP_PROP_POS_AVI_RATIO);

	m_input = cvRetrieveFrame(m_capture);           // retrieve the captured frame

}
