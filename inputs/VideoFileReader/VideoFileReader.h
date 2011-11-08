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
	VideoFileReaderParameterStructure(ConfigReader& x_confReader, const std::string& x_objectName) : 
	InputParameterStructure(x_confReader, x_objectName)
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
	VideoFileReader(const std::string& x_name, ConfigReader& x_confReader);
	~VideoFileReader();
	
	void Capture();
	const std::string& GetName(){return m_name;};
	virtual const IplImage * GetImage() const {return m_input;}

protected:
	IplImage * m_input;
	CvCapture * m_capture;
	int m_width;
	int m_height;
	int m_fps;

	const std::string m_name;
	VideoFileReaderParameterStructure m_param;
	
	virtual const VideoFileReaderParameterStructure& GetRefParameter() const {return m_param;};
};

#endif