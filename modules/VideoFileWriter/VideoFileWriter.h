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

#ifndef INPUT_VIDEOFILEWRITER_H
#define INPUT_VIDEOFILEWRITER_H

#include <opencv2/highgui/highgui.hpp>
#include "Module.h"



class VideoFileWriterParameterStructure : public ModuleParameterStructure
{
public:
	VideoFileWriterParameterStructure(const ConfigReader& x_confReader) : 
	ModuleParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterString("file", 	  "output", 	     &file,      "Name of the video file to write, with path"));
		m_list.push_back(new ParameterString("fourcc", 	  "MJPG", 	     &fourcc,    "Four character code, determines the format. PIM1, MJPG, MP42, DIV3, DIVX, U263, I263, FLV1"));
		RefParameterByName("type").SetDefault("CV_8UC3");
		Init();
		RefParameterByName("type").Lock();
		ParameterStructure::Init();
	};

	std::string file;
	std::string fourcc;
};

/**
* @brief Write output to a video file
*/
class VideoFileWriter : public Module
{
public:
	VideoFileWriter(const ConfigReader& x_confReader);
	~VideoFileWriter();
	MKCLASS("VideoFileWriter")
	MKDESCR("Write output to a video file")
	
	virtual void ProcessFrame();
	void Reset();
	static const std::string ExtensionFromFourcc(const std::string& x_fourcc);

private:
	VideoFileWriterParameterStructure m_param;
	inline virtual const VideoFileWriterParameterStructure& GetParameters() const {return m_param;}
	static log4cxx::LoggerPtr m_logger;

protected:
	// input
	cv::Mat m_input;

	// temporary // TODO: Check that the file is not rewritten after a Reset. Maybe start a new file
	cv::VideoWriter m_writer;
};

#endif
