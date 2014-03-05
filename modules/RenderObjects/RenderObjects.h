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

#ifndef RENDER_OBJECTS_H
#define RENDER_OBJECTS_H

#include "Module.h"

class ConfigReader;
class Object;

class RenderObjectsParameterStructure : public ModuleParameterStructure
{
public:
	RenderObjectsParameterStructure(const ConfigReader& x_confReader) : ModuleParameterStructure(x_confReader)
	{
		RefParameterByName("type").SetDefault("CV_8UC3");
		ParameterStructure::Init();
	};
};

class RenderObjects : public Module
{
public:
	RenderObjects(const ConfigReader& x_configReader);
	~RenderObjects();
	
	void Reset();
	virtual void ProcessFrame();

private:
	RenderObjectsParameterStructure m_param;
	inline virtual const ModuleParameterStructure& GetParameters() const { return m_param;}
	static log4cxx::LoggerPtr m_logger;

protected:
	// for streams
	cv::Mat m_imageInput;
	cv::Mat m_imageOutput;
	std::vector<Object> m_objectInput1;
	std::vector<Object> m_objectInput2;
	std::vector<Object> m_objectInput3;
	std::vector<Object> m_objectInput4;
	std::vector<Object> m_objectInput5;
};


#endif
