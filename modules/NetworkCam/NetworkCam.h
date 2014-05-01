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
#include <semaphore.h>

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
	}

public:
	std::string url;
};

/**
* @brief Read video stream from a network camera
*/
class NetworkCam : public Input
{
public:
	NetworkCam(const ConfigReader& x_confReader);
	~NetworkCam();
	virtual const std::string& GetClass() const {static std::string cl = "NetworkCam"; return cl;}
	virtual const std::string& GetDescription() const {static std::string descr = "Read video stream from a network camera"; return descr;}
	
	void Capture();
	virtual void Reset();
	const std::string& GetName(){return m_name;}
	virtual double GetRecordingFps();
private:
	bool Grab();
	NetworkCamParameterStructure m_param;
	inline virtual const NetworkCamParameterStructure& GetParameters() const {return m_param;}
	static log4cxx::LoggerPtr m_logger;
protected:
	void GetProperties();

	// output
	cv::Mat m_output;

	// temporary
	cv::VideoCapture m_capture;
	Timer m_frameTimer;
	sem_t m_semTimeout;
};

#endif
