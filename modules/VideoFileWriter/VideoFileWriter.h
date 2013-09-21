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

#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <Module.h>



class VideoFileWriterParameterStructure : public ModuleParameterStructure
{
public:
	VideoFileWriterParameterStructure(const ConfigReader& x_confReader) : 
	ModuleParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterString("file", 	"output.avi", 	&file,   "Name of the video file to write, with path"));
		m_list.push_back(new ParameterString("fourcc", 	"MJPG", 	&fourcc, "Four character code, determinses the format"));
		ParameterStructure::Init();
	};

	std::string file;
	std::string fourcc;
};

class VideoFileWriter : public Module
{
public:
	VideoFileWriter(const ConfigReader& x_confReader);
	~VideoFileWriter();
	
	virtual void ProcessFrame();
        void Reset();

protected:
	cv::VideoWriter m_writer;
	cv::Mat * m_input;
	
	static const char * m_type;

private:
	VideoFileWriterParameterStructure m_param;
	inline virtual VideoFileWriterParameterStructure& RefParameter() {return m_param;};
};

#endif
