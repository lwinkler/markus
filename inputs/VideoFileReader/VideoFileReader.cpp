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

using namespace std;
using namespace cv;

VideoFileReader::VideoFileReader(const std::string& x_name, ConfigReader& x_configReader): 
	m_param(x_configReader, x_name), 
	Input(x_name, x_configReader),
	m_name(x_name)
{
	m_capture = cvCaptureFromFile(m_param.file.c_str());
	m_fps     = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_FPS);
	
	if(m_capture == NULL)
	{
		throw("Error : VideoFileReader not found ! : " + x_name);
	}
	cout<<"Setting "<<m_param.width<<endl;
	
	// Get capture device information
	//cvSetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_WIDTH, m_param.width); // not working
	//cvSetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_HEIGHT, m_param.height);
	cvQueryFrame(m_capture); // this call is necessary to get correct capture properties
	IplImage * tmp = cvRetrieveFrame(m_capture);
	m_inputWidth    = tmp->width;//(int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_WIDTH);
	m_inputHeight   = tmp->height;//(int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_HEIGHT);
	int numFramesc = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_COUNT);
	
	cout<<"VideoFileReader "<<m_inputWidth<<"x"<<m_inputHeight<<endl;
//	assert(m_width == m_param.width);
//	assert(m_height == m_param.height);
	
	
	m_input = new Mat( cvSize(m_inputWidth, m_inputHeight), IPL_DEPTH_8U, 3);
	m_render = new Mat( cvSize(m_inputWidth, m_inputHeight), IPL_DEPTH_8U, 3);
	m_outputStreams.push_back(new StreamImage("input", m_input));
	m_outputStreams.push_back(new StreamImage("render", m_render));
}

VideoFileReader::~VideoFileReader()
{
	delete(m_input);
	delete(m_render);
	cvReleaseCapture(&m_capture );
}


void VideoFileReader::Capture()
{
	//Get frame information:
	//double posMsec   =       cvGetCaptureProperty(m_capture, CV_CAP_PROP_POS_MSEC);
	//int posFrames    = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_POS_FRAMES);
	//double posRatio  =       cvGetCaptureProperty(m_capture, CV_CAP_PROP_POS_AVI_RATIO);
	m_lock.lockForRead();
	cvGrabFrame(m_capture);
	//m_input = cvRetrieveFrame(m_capture);           // retrieve the captured frame
	//cvCopy(cvRetrieveFrame(m_capture), m_input);
	assert(false);//cvRetrieveFrame(m_capture).copyTo(m_input);
	//m_input = Mat(*cvRetrieveFrame(m_capture), true); // TODO : Fix copied twice
	
	m_lock.unlock();
}
