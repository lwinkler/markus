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

#ifndef MANAGER_H
#define MANAGER_H

#include <list>
#include <cstring>
#include <QReadWriteLock>

#include <opencv/cv.h>

#include "ConfigReader.h"
#include "Input.h"
#include "Module.h"
#include "Timer.h"

class CvVideoWriter;

/// Parameter class

class ManagerParameter : public ParameterStructure
{
public:
	ManagerParameter(const ConfigReader& x_confReader, const std::string& x_moduleName) : ParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterString(0, "mode",	"", 	&mode, 	"Mode"));
		//ParameterStructure::Init(); // TODO : restore this
	}
	std::string mode;
};

/// Class for managing all modules

class Manager : public Configurable
{
public:
	Manager(ConfigReader & x_configReader);
	~Manager();
	void Process();
	// const std::vector<Input*> & GetInputList()  const {return m_inputs; };
	const std::vector<Module*>& GetModuleList() const {return m_modules; };

	//std::vector<Input*> & GetInputListVar()  {return m_inputs; };
	std::vector<Module*>& GetModuleListVar() {return m_modules; };
	
	Module * GetModuleById(int x_id) const;
	
	void Export();
	void PrintTimers();
private:
	ManagerParameter m_param;
	CvVideoWriter * m_writer;

	long long m_timerConvertion;
	long long m_timerProcessing;

	//clock_t m_timeLastProcess;
	
	std::vector<Module *> 		m_modules;
	// std::vector<Input  *> 		m_inputs;

	Timer m_timer;
	long long m_frameCount;
protected:
	inline virtual const ParameterStructure& RefParameter() {return m_param;};
	QReadWriteLock m_lock;
};
#endif
