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
#include <atomic>


namespace mk {
/**
* @brief Write output to a buffer and exports it if an evenement occurs
*/
class VideoFileBufferWriter : public VideoFileWriter
{
public:
	class Parameters : public VideoFileWriter::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) :
			VideoFileWriter::Parameters(x_name)
		{
			AddParameter(new ParameterInt   ("bufferFramesBefore" , 1200, 0, 10000, &bufferFramesBefore,  "Length of video buffer before activity [frames]"));
			AddParameter(new ParameterDouble("bufferDurationAfter", 120, 0, 600,    &bufferDurationAfter, "Length of video buffer after activity [s]. If possible this should be longer than the duration before next event."));
			AddParameter(new ParameterBool("keepAllRecordings"    , false,          &keepAllRecordings  , "Keep all recordings, event if no event is associated with it."));

			RefParameterByName("type").SetDefaultAndValue("CV_8UC3");
			RefParameterByName("type").SetRange(R"({"allowed":["CV_8UC3"]})"_json);
		};
		int    bufferFramesBefore;
		double bufferDurationAfter;
		bool   keepAllRecordings;
	};

	explicit VideoFileBufferWriter(ParameterStructure& xr_params);
	virtual ~VideoFileBufferWriter();
	MKCLASS("VideoFileBufferWriter")
	MKCATEG("Output")
	MKDESCR("Write output to a buffer and export it if an evenement occurs")

	void ProcessFrame() override;
	void Reset() override;
	void AddImageToBuffer();
	void OpenNewFile();
	void CloseFile();
	void WaitForThread() const;

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
	boost::circular_buffer<cv::Mat> m_buffer1;
	boost::circular_buffer<cv::Mat> m_buffer2;
	TIME_STAMP m_endOfRecord;
	std::mutex m_mutex;
	std::atomic<bool> m_threadIsWorking;
};

} // namespace mk
#endif
