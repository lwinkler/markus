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

#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>

#include "Input.h"


class VideoFileReaderParameterStructure : public InputParameterStructure
{
public:
	VideoFileReaderParameterStructure(const ConfigReader& x_confReader) : 
	InputParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterString(0, "file", 	"input.avi", 	&file,	"Name of the video file to read, with path"));
		ParameterStructure::Init();
	};

public:
	std::string file;
};

class VideoFileReader : public Input
{
public:
	VideoFileReader(const ConfigReader& x_confReader);
	~VideoFileReader();
	
	void Capture();
        void Reset();
	const std::string& GetName(){return m_name;};
        const cv::Mat * GetImage() const {return m_output;}

protected:
	CvCapture * m_capture;
	cv::Mat * m_output;
	int m_fps;

private:
	VideoFileReaderParameterStructure m_param;
	inline virtual VideoFileReaderParameterStructure& RefParameter() {return m_param;};
};

#endif
