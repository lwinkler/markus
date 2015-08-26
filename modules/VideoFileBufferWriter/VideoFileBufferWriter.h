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

#ifndef INPUT_VIDEOFILEBUFFERWRITER_H
#define INPUT_VIDEOFILEBUFFERWRITER_H

#include "modules/VideoFileWriter/VideoFileWriter.h"
#include "Event.h"

#include <opencv2/highgui/highgui.hpp>
#include <boost/circular_buffer.hpp>


/**
* @brief Write output to a buffer and exports it if an evenement occurs
*/
class VideoFileBufferWriter : public VideoFileWriter
{
public:
	class Parameters : public VideoFileWriter::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) :
			VideoFileWriter::Parameters(x_confReader)
		{
			m_list.push_back(new ParameterInt   ("buffer_frames_before" , 1200, 0, 10000, &bufferFramesBefore, "Length of video buffer before the recording [frames]"));
			m_list.push_back(new ParameterDouble("buffer_duration_after", 120, 0, 600, &bufferDurationAfter  , "Length of video buffer after the recording [s]"));

			RefParameterByName("type").SetDefault("CV_8UC3");
			RefParameterByName("type").SetRange("[CV_8UC3]");
			Init();
		};
		int    bufferFramesBefore;
		double bufferDurationAfter;
	};

	VideoFileBufferWriter(ParameterStructure& xr_params);
	~VideoFileBufferWriter();
	MKCLASS("VideoFileBufferWriter")
	MKCATEG("Output")
	MKDESCR("Write output to a buffer and export it if an evenement occurs")

	virtual void ProcessFrame();
	virtual void Reset();
	void AddImageToBuffer();
	void OpenNewFile();
	void CloseFile();

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	// input
	bool m_trigger;

	// output and input
	Event m_event;

	// state
	bool m_recording;
	bool m_eraseFile;
	std::string m_fileName;

	// temporary
	boost::circular_buffer<cv::Mat> m_buffer;
	TIME_STAMP m_endOfRecord;;
};

#endif
