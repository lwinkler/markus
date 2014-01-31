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

#ifndef INPUT_NETWORKCAM_H
#define INPUT_NETWORKCAM_H

#include <opencv2/highgui/highgui.hpp>

#include "Input.h"
#include "Timer.h"


class NetworkCamParameterStructure : public InputParameterStructure
{
public:
	NetworkCamParameterStructure(const ConfigReader& x_confReader) : 
	InputParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterString("url", 	"", 	&url,	"Network address of the camera (e.g. http://root:admin@192.168.3.62/mjpg/1/video.mjpg"));
		ParameterStructure::Init();
	};

public:
	std::string url;
};

class NetworkCam : public Input
{
public:
	NetworkCam(const ConfigReader& x_confReader);
	~NetworkCam();
	
	void Capture();
	virtual void Reset();
	const std::string& GetName(){return m_name;};
	// virtual const cv::Mat * GetImage() const {return m_output;}
	virtual double GetRecordingFps();
private:
	NetworkCamParameterStructure m_param;
	inline virtual NetworkCamParameterStructure& RefParameter() {return m_param;};
protected:
	cv::VideoCapture m_capture;
	cv::Mat m_output;
	TIME_STAMP m_timeStamp;
	void GetProperties();
	Timer m_frameTimer;
};

#endif
