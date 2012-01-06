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

#include "cv.h"
#include <highgui.h>

#include "Input.h"


class VideoFileReaderParameterStructure : public InputParameterStructure
{
public:
	VideoFileReaderParameterStructure(const ConfigReader& x_confReader) : 
	InputParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterT<std::string>(0, "file", 	"input.avi", 	PARAM_STR,	&file));
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
	const std::string& GetName(){return m_name;};
	virtual const cv::Mat * GetImage() const {return m_output;}

protected:
	VideoFileReaderParameterStructure m_param;
	//const std::string m_name;
	CvCapture * m_capture;
	cv::Mat * m_output;

	//int m_width;
	//int m_height;
	int m_fps;
	
	inline virtual const VideoFileReaderParameterStructure& GetRefParameter() const {return m_param;};
};

#endif