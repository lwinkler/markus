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




/**
* @brief Write output to a video file
*/
class VideoFileWriter : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) :
			Module::Parameters(x_confReader)
		{
			m_list.push_back(new ParameterString("file", 	  "output", 	     &file,      "Name of the video file to write, with path"));
			m_list.push_back(new ParameterString("fourcc", 	  "MJPG", 	     &fourcc,    "Four character code, determines the format. PIM1, MJPG, MP42, DIV3, DIVX, H263, I263, FLV1"));

			RefParameterByName("width").SetRange("[32:6400]");
			RefParameterByName("height").SetRange("[24:4800]");
			RefParameterByName("type").SetDefault("CV_8UC3");
			RefParameterByName("type").SetRange("[CV_8UC3]");

			RefParameterByName("fourcc").SetRange("[MJPG,MP42,DIV3,DIVX,FLV1]"); // PIM1 does not support 12 fps: disabled for tests / I263 also deactivated for tests

			Init();
		};

		std::string file;
		std::string fourcc;
	};

	VideoFileWriter(ParameterStructure& xr_params);
	~VideoFileWriter();
	MKCLASS("VideoFileWriter")
	MKCATEG("Output")
	MKDESCR("Write output to a video file")

	virtual void ProcessFrame();
	void Reset();
	static const std::string ExtensionFromFourcc(const std::string& x_fourcc);

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	// input
	cv::Mat m_input;

	// state
	int m_index; // Nb of the file to avoid erasing after a reset

	// temporary
	cv::VideoWriter m_writer;
};

#endif
