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

#ifndef INPUT_USBCAM_H
#define INPUT_USBCAM_H

#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>

#include "Input.h"


class UsbCamParameterStructure : public InputParameterStructure
{
public:
	UsbCamParameterStructure(const ConfigReader& x_confReader) : 
	InputParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterInt("num", 	-1, 	PARAM_INT, 	-1, 	2,	&num,	"Number of the camera, as defined by OpenCV")); // TODO: should work with a default parameter even if no camera is present
		ParameterStructure::Init();
	};

public:
	int num;
};

class UsbCam : public Input
{
public:
	UsbCam(const ConfigReader& x_confReader);
	~UsbCam();
	
	void Capture();
	virtual void Reset();
	const std::string& GetName(){return m_name;};
	// virtual const cv::Mat * GetImage() const {return m_output;}

protected:
	cv::VideoCapture m_capture;
	cv::Mat * m_output;
	TIME_STAMP m_timeStamp;

private:
	void GetProperties();
	UsbCamParameterStructure m_param;
	inline virtual UsbCamParameterStructure& RefParameter() {return m_param;};
};

#endif
