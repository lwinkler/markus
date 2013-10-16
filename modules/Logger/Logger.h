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

#ifndef LOGGER_H
#define LOGGER_H

#include "Module.h"
#include "Parameter.h"
#include "Timer.h"


/*! \class Logger
 *  \brief Class containing methods/attributes of a slit camera
 *
 */


class LoggerParameterStructure : public ModuleParameterStructure
{
	
public:
	LoggerParameterStructure(const ConfigReader& x_confReader) : 
		ModuleParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterString("file", 	  "state", 	     &file,      "Name of the .srt file without extension"));
		m_list.push_back(new ParameterBool("timestamp", 1, PARAM_BOOL, 0, 1, &timeStamp, "Add a time stamp the to file name"));
		ParameterStructure::Init();
	}
	std::string file;
	bool timeStamp;
};

class Logger : public Module
{
protected:
	virtual void ProcessFrame();
	LoggerParameterStructure m_param;
	static const char * m_type;


	bool m_state;
	bool m_oldState;
	long int m_subId;
	std::string m_startTime;
	std::string m_srtFileName;

public:
	Logger(const ConfigReader& x_configReader);
	~Logger(void);
	void Reset();


protected:
	inline virtual LoggerParameterStructure& RefParameter() { return m_param;}
};

#endif

