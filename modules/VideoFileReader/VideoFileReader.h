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

#ifndef INPUT_VIDEOFILEREADER_H
#define INPUT_VIDEOFILEREADER_H

#include <opencv2/highgui/highgui.hpp>

#include "Input.h"



namespace mk {
/**
* @brief Read video stream from a video file
*
*  improvement: implement parameters for begin and end of reading time
*/
class VideoFileReader : public Input
{
public:
	class Parameters : public Input::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) :
			Input::Parameters(x_name)
		{
			AddParameter(new ParameterString("file",  "in/input.mp4", &file, "Name of the video file to read, with path"));
			AddParameter(new ParameterBool("loop",    false,          &loop, "Loop on file"));
		}

	public:
		std::string file;
		bool loop;
	};

	explicit VideoFileReader(ParameterStructure& xr_params);
	~VideoFileReader() override;
	MKCLASS("VideoFileReader")
	MKCATEG("Input")
	MKDESCR("Read video stream from a video file")

	// const cv::Mat * GetImage() const {return m_output;}

	// Specific to file reader
	void SetMsec(int x_msec);
	void SetFrame(int x_frame);
	int GetMsec();
	int GetFrame();
	int GetMaxMsec();
	int GetFrameCount();
	double GetRecordingFps() const override;

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	void Capture() override;
	void Reset() override;
	void GetProperties();

	// state
	TIME_STAMP m_beginTimeStamp = 0;  /// Used to handle time stamps with loop=1

	// output
	cv::Mat m_output;

	// temporary
	cv::VideoCapture m_capture;
	double m_recordingFps = 0;
};

} // namespace mk
#endif
