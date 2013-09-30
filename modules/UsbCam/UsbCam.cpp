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
	Module::Reset();

	m_capture.release();
	m_capture.open(m_param.num);
	GetProperties();
	
	if(! m_capture.isOpened())
	{
		throw("Error : UsbCam cannot open cam : " + m_param.num);
	}

	// Apparently you cannot set width and height. We try anyway
	m_capture.set(CV_CAP_PROP_FRAME_WIDTH,  m_param.width);
	m_capture.set(CV_CAP_PROP_FRAME_HEIGHT, m_param.height);
	
	// note on the next line: the image will be overloaded but the properties are used to set the input ratio, the type is probably ignored
	delete m_output;
	m_output = new Mat(Size(m_capture.get(CV_CAP_PROP_FRAME_WIDTH), m_capture.get(CV_CAP_PROP_FRAME_HEIGHT)), m_param.type);
}

void UsbCam::Capture()
{
	if(m_capture.grab() == 0)
	{
		m_endOfStream = true;
		std::exception e;
		Pause(true);
		throw e; // TODO: Impl custom exceptions ("Capture failed in UsbCam::Capture.");
	}

	m_capture.retrieve(*m_output);
	
	// cout<<"UsbCam capture image "<<m_output->cols<<"x"<<m_output->rows<<" time stamp "<<m_capture.get(CV_CAP_PROP_POS_MSEC) / 1000.0<< endl;

	m_timeStamp += 1000.0 / m_param.fps; // TODO: improve this
	SetTimeStampToOutputs(m_timeStamp);
}

void UsbCam::GetProperties()
{
	// cout<<"CV_CAP_PROP_POS_MSEC "<<m_capture.get(CV_CAP_PROP_POS_MSEC)<<endl;
	// cout<<"CV_CAP_PROP_POS_FRAMES "<<m_capture.get(CV_CAP_PROP_POS_FRAMES)<<endl;
	// cout<<"CV_CAP_PROP_POS_AVI_RATIO "<<m_capture.get(CV_CAP_PROP_POS_AVI_RATIO)<<endl;
	cout<<"CV_CAP_PROP_FRAME_WIDTH "<<m_capture.get(CV_CAP_PROP_FRAME_WIDTH)<<endl;
	cout<<"CV_CAP_PROP_FRAME_HEIGHT "<<m_capture.get(CV_CAP_PROP_FRAME_HEIGHT)<<endl;
	// cout<<"CV_CAP_PROP_FPS "<<m_capture.get(CV_CAP_PROP_FPS)<<endl;
	// int cc = static_cast<int>(m_capture.get(CV_CAP_PROP_FOURCC));
	// cout<<"CV_CAP_PROP_FOURCC "<< (char)(cc & 0XFF) << (char)((cc & 0XFF00) >> 8) << (char)((cc & 0XFF0000) >> 16) << (char)((cc & 0XFF000000) >> 24) <<endl;
	// cout<<"CV_CAP_PROP_FRAME_COUNT "<<m_capture.get(CV_CAP_PROP_FRAME_COUNT)<<endl;
	// cout<<"CV_CAP_PROP_FORMAT "<<m_capture.get(CV_CAP_PROP_FORMAT)<<endl;
	cout<<"CV_CAP_PROP_MODE "<<m_capture.get(CV_CAP_PROP_MODE)<<endl;
	cout<<"CV_CAP_PROP_BRIGHTNESS "<<m_capture.get(CV_CAP_PROP_BRIGHTNESS)<<endl;
	cout<<"CV_CAP_PROP_CONTRAST "<<m_capture.get(CV_CAP_PROP_CONTRAST)<<endl;
	cout<<"CV_CAP_PROP_SATURATION "<<m_capture.get(CV_CAP_PROP_SATURATION)<<endl;
	cout<<"CV_CAP_PROP_HUE "<<m_capture.get(CV_CAP_PROP_HUE)<<endl;
	cout<<"CV_CAP_PROP_GAIN "<<m_capture.get(CV_CAP_PROP_GAIN)<<endl;
	cout<<"CV_CAP_PROP_EXPOSURE "<<m_capture.get(CV_CAP_PROP_EXPOSURE)<<endl;
	cout<<"CV_CAP_PROP_CONVERT_RGB "<<m_capture.get(CV_CAP_PROP_CONVERT_RGB)<<endl;
	// cout<<"CV_CAP_PROP_WHITE_BALANCE"<<m_capture.get(CV_CAP_PROP_WHITE_BALANCE)<<endl;
	cout<<"CV_CAP_PROP_RECTIFICATION"<<m_capture.get(CV_CAP_PROP_RECTIFICATION)<<endl;
}
