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

#include <semaphore.h>
#include <opencv2/highgui/highgui.hpp>
#include "Input.h"
#include "Timer.h"



namespace mk {
/**
* @brief Read video stream from a network camera

	the url parameter may be an address of an http or rtsp stream
	e.g. http://user:pass@cam_address:8081/cgi/mjpg/mjpg.cgi?.mjpg
	     rtsp://cam_address:554/live.sdp
	     rtsp://<servername>/axis-media/media.amp
*/
class NetworkCam : public Input
{
public:
	class Parameters : public Input::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) :
			Input::Parameters(x_name)
		{
			AddParameter(new ParameterString("url", "", &url, "Network address of the camera (e.g. http://root:admin@192.168.3.62/mjpg/1/video.mjpg"));
			AddParameter(new ParameterBool("checkAspectRatio", false,   &checkAspectRatio, "If true checks the aspect ratio of the incoming image and rebuilds all modules if wrong"));
		}

	public:
		std::string url;
		bool checkAspectRatio;
	};

	explicit NetworkCam(ParameterStructure& xr_params);
	virtual ~NetworkCam();
	MKCLASS("NetworkCam")
	MKCATEG("Input")
	MKDESCR("Read video stream from a network camera")

	double GetRecordingFps() const override;

private:
	bool Grab();
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	void Capture() override;
	void Reset() override;
	void GetProperties();

	// output
	cv::Mat m_output;

	// temporary
	cv::VideoCapture m_capture;
	sem_t m_semTimeout;
	double m_recordingFps;
};

} // namespace mk
#endif
