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
#include "util.h"
#include "StreamImage.h"

using namespace std;
using namespace cv;

NetworkCam::NetworkCam(const ConfigReader& x_configReader): 
	Input(x_configReader),
	m_param(x_configReader)
{

	//open the video stream and make sure it's opened
	/* it may be an address of an mjpeg stream, 
	e.g. "http://user:pass@cam_address:8081/cgi/mjpg/mjpg.cgi?.mjpg" 
	"rtsp://cam_address:554/live.sdp" */
	if(m_param.url.size() == 0)
		m_capture.open("input.avi");
	else if(!m_capture.open(m_param.url)) {
		throw( "Error opening video stream or file" + m_param.url);
	}
	
	cout<<"Capture from NetworkCam with resolution "<<GetWidth()<<"x"<<GetHeight()<<endl;
	
	m_output = new Mat( cvSize(m_param.width, m_param.height), m_param.type);
	m_outputStreams.push_back(new StreamImage(0, "input", m_output, *this, 		"Video stream of the camera"));

	//Reset();
}

NetworkCam::~NetworkCam()
{
	// cvReleaseCapture(&m_capture );
	delete(m_output);
}

void NetworkCam::Reset()
{
	Module::Reset();
	resize(*m_output, *m_output, cvSize(m_param.width, m_param.height));
}

void NetworkCam::Capture()
{
	//Get frame information:
	//double posMsec   =       cvGetCaptureProperty(m_capture, CV_CAP_PROP_POS_MSEC);
	//int posFrames    = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_POS_FRAMES);
	//double posRatio  =       cvGetCaptureProperty(m_capture, CV_CAP_PROP_POS_AVI_RATIO);
	Mat tmp;// = new Mat(cvRetrieveFrame(m_capture));
	m_capture.read(tmp);

	//int frameH    = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_HEIGHT);
	//int frameW    = (int) cvGetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_WIDTH);	
	
	//cout<<tmp->width<<" == "<<m_input->width<<endl;
	//assert(tmp->width == m_input->width);
	//assert(tmp->height == m_input->height);
	//assert(tmp->depth == m_input->depth);
	
	//tmp->copyTo(*m_output);
	adjustSize(&tmp, m_output);
	
	//delete(tmp);
}
