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

#ifndef LOG_OBJECTS_H
#define LOG_OBJECTS_H

#include "Module.h"
#include "Parameter.h"
#include "StreamObject.h"



class LogObjectsParameterStructure : public ModuleParameterStructure
{
	
public:
	LogObjectsParameterStructure(const ConfigReader& x_confReader) : 
		ModuleParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterString("file", 	  "objects", 	     &file,      "Name of the .srt file without extension"));
		ParameterStructure::Init();
	}
	std::string file;
};

class LogObjects : public Module
{
protected:
	virtual void ProcessFrame();
	std::string m_fileName;

	std::vector <Object> m_objectsIn;

public:
	LogObjects(const ConfigReader& x_configReader);
	~LogObjects(void);
	void Reset();


private:
	LogObjectsParameterStructure m_param;
	inline virtual const LogObjectsParameterStructure& GetParameters() const { return m_param;}
};

#endif

