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
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  <See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public License
*    along with Markus.  If not, see <http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------------------*/

#ifndef PROCESSABLE_H
#define PROCESSABLE_H

#include "ConfigReader.h"
#include "ParameterStructure.h"
#include "ParameterNum.h"
#include "Context.h"
#include <QReadWriteLock>
#include <log4cxx/logger.h>

class QModuleTimer;

/**
* @brief Class representing a module. A module is a node of the application, it processes streams
*/
class Processable : public Configurable
{
public:
	class Parameters : public ParameterStructure
	{
	public:
		Parameters(const ConfigReader& x_confReader) : ParameterStructure(x_confReader)
		{
			m_list.push_back(new ParameterBool("auto_process",  0, 	0, 	1,		&autoProcess,	"If yes the module processes with a timer at fixed fps, if no the module processes based on the time stamp of the input stream"));
			m_list.push_back(new ParameterBool("allow_unsync_input",  0, 0, 	1,		&allowUnsyncInput, "If yes the module accepts that its input can be on a different time stamp. Only relevant if the module has many inputs. Use at your own risks."));
			m_list.push_back(new ParameterDouble("fps", 	 0, 	0, 	1000,		&fps,		"Frames per seconds (processing speed)"));

			Init();
		}

		bool autoProcess;
		bool allowUnsyncInput;
		double fps;
	};
	Processable(const ConfigReader& x_confReader);
	virtual ~Processable();
	
	virtual void Reset();
	void Pause(bool x_pause);
	virtual bool Process() = 0;
	inline void AllowAutoProcess(bool x_proc) {m_allowAutoProcess = x_proc;}
	inline void SetRealTime(bool x_realTime) {m_realTime  = x_realTime;}
	inline virtual void SetContext(const Context& x_context){m_context = x_context;}
	
protected:
	virtual Parameters & RefParameters() = 0;

	bool m_pause;
	bool m_allowAutoProcess;
	bool m_realTime;         /// Process in real-time: if true, the module processes as fast as possible
	QModuleTimer * m_moduleTimer;
	QReadWriteLock m_lock;

	Context m_context; /// context given by Manager (output directory, ...)

private:
	static log4cxx::LoggerPtr m_logger;
};

#endif
