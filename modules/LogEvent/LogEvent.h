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

#ifndef LOG_EVENT_H
#define LOG_EVENT_H

#include "Module.h"
#include "Event.h"
#include "Timer.h"
#include "AnnotationFileWriter.h"

#include <memory>

namespace mk {

class MkDirectory;

/**
* @brief Read an event and log it to .srt file
*/
class LogEvent : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) :
			Module::Parameters(x_name)
		{
			AddParameter(new ParameterString("file"        , "event.%d.srt", &file      ,  "Name of the .srt file without extension"));
			AddParameter(new ParameterDouble("duration"    , 5, 0, 600    , &duration  ,  "Duration of the event for logging in .srt file"));
			AddParameter(new ParameterString("folder"      , "events_img" , &folder    ,  "Name of the folder to create for images"));
			AddParameter(new ParameterString("extension"   , "jpg"        , &extension ,  "Extension of the thumbnails. Determines the output format."));

			// The 4 gt_ parameters are only used for evaluation vs ground truth file
			AddParameter(new ParameterString("gtCommand"  , ""           , &gtCommand ,  "The command to use for comparison with ground truthi, e.g. \"tools/evaluation/analyse_events.py -d 0 -t 8 -e intrusion\""));
			AddParameter(new ParameterString("gtFile"     , ""           , &gtFile    ,  "Ground truth file name. If empty, the program will consider that the ground truth is empty."));
			AddParameter(new ParameterString("gtVideo"    , ""           , &gtVideo   ,  "Video file to use to create the ground truth."));

			RefParameterByName("type").SetDefaultAndValue("CV_8UC3");
			RefParameterByName("type").SetRange(R"({"allowed":["CV_8UC1","CV_8UC3"]})"_json);
			RefParameterByName("extension").SetRange(R"({"allowed":["jpg","png"]})"_json);
		}
		std::string file;
		double duration;
		std::string extension;
		std::string folder;
		std::string gtCommand;
		std::string gtFile;
		std::string gtVideo;
	};

	explicit LogEvent(ParameterStructure& xr_params);
	~LogEvent() override;
	MKCLASS("LogEvent")
	MKCATEG("Output")
	MKDESCR("Read an event and log it to .srt file")

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	void ProcessFrame() override;
	void Reset() override;
	void SaveImage(Event& xr_event) const;
	bool IsInputProcessed() const override;
	void WriteEvent();
	void CompareWithGroundTruth();

	// input
	Event m_event;
	cv::Mat m_inputIm1;
	cv::Mat m_inputIm2;

	// temporary
	bool m_saveImage1 = false;
	bool m_saveImage2 = false;
	AnnotationFileWriter* mp_annotationWriter;
	std::unique_ptr<MkDirectory> mp_outputDir;
};

} // namespace mk
#endif

