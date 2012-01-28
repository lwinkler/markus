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

#ifndef VIDEOOUTPUT_H
#define VIDEOOUTPUT_H

#include "Module.h"

class ConfigReader;
class Object;

class VideoOutputParameterStructure : public ModuleParameterStructure
{
public:
	VideoOutputParameterStructure(const ConfigReader& x_confReader) : ModuleParameterStructure(x_confReader)
	{
		ParameterStructure::Init();
		//RefParameterByName("inputType").SetDefault(static_cast<const void*>(&CV_8UC3));
	};
};

class VideoOutput : public Module
{
public:
	VideoOutput(const ConfigReader& x_configReader);
	~VideoOutput();
	
	virtual void ProcessFrame();
	inline virtual int GetInputWidth() const {return m_param.width;};
	inline virtual int GetInputHeight() const {return m_param.height;};
	
protected:
	// for streams
	cv::Mat * m_imageInput;
	cv::Mat * m_imageOutput;

	static const char * m_type;
	std::vector<Object> m_objectInput1;
	std::vector<Object> m_objectInput2;
	std::vector<Object> m_objectInput3;
	std::vector<Object> m_objectInput4;
	std::vector<Object> m_objectInput5;

private:
	VideoOutputParameterStructure m_param;
	inline virtual const ModuleParameterStructure& GetRefParameter() const { return m_param;};
};


#endif