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

#ifndef INPUT_H
#define INPUT_H

#include "cv.h"
#include <highgui.h>

#include "ConfigReader.h"
#include "Parameter.h"

class InputParameterStructure : public ParameterStructure
{
public:
	InputParameterStructure(ConfigReader& x_confReader, const std::string& x_moduleName) : ParameterStructure(x_confReader, "Input", x_moduleName)
	{
		m_list.push_back(new ParameterT<int>(0, "width", 	640, 	PARAM_INT, 	0, 	4000,	&width));
		m_list.push_back(new ParameterT<int>(0, "height", 	480, 	PARAM_INT, 	0, 	3000,	&height));
		m_list.push_back(new ParameterT<int>(0, "depth", 	IPL_DEPTH_8U, PARAM_INT, 	0, 	32,	&depth));
		m_list.push_back(new ParameterT<int>(0, "channels", 	3, 	PARAM_INT, 	1, 	3,	&channels));
		m_list.push_back(new ParameterT<std::string>(0, "source", 	"cam", 	PARAM_STR, 	&source));
		
		ParameterStructure::Init();
	};

public:
	std::string source;
	int width;
	int height;
	int depth;
	int channels;
};

class Input : Configurable
{
public:
	Input(const std::string& x_name, ConfigReader& x_confReader);
	~Input();
	
	void Capture();
	const std::string& GetName(){return m_name;};
	const IplImage * GetImage() const {return m_input;}

protected:
	IplImage * m_input;
	CvCapture * m_capture;
	int m_width;
	int m_height;
	int m_fps;

	const std::string m_name;
	InputParameterStructure m_param;
	
	virtual const ParameterStructure& GetRefParameter() const {return m_param;};
};

#endif