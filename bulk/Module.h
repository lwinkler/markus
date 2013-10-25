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

#ifndef MODULE_H
#define MODULE_H

#include "ConfigReader.h"
#include "Parameter.h"
#include <QReadWriteLock>

#define MAX_WIDTH  6400
#define MAX_HEIGHT 4800



class Stream;
class ControlBoard;
class QModuleTimer;

class ModuleParameterStructure : public ParameterStructure
{
public:
	ModuleParameterStructure(const ConfigReader& x_confReader) : ParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterString("class", 		"", 					&objClass,	"Class of the module (define the module's function)"));
		m_list.push_back(new ParameterInt("width", 	640, 	PARAM_INT, 	1, MAX_WIDTH,		&width,		"Width of the input"));
		m_list.push_back(new ParameterInt("height", 	480, 	PARAM_INT, 	1, MAX_HEIGHT,		&height,	"Height of the input"));
		m_list.push_back(new ParameterImageType("type", 	CV_8UC1, 				&type,		"Format of the input image"));
		m_list.push_back(new ParameterBool("auto_process",  0, 	PARAM_BOOL, 	0, 	1,		&autoProcess,	"If yes the module processes with a timer at fixed fps, if no the module processes based on the time stamp of the input stream"));
		m_list.push_back(new ParameterDouble("fps", 	 0, 	PARAM_DOUBLE, 	0, 	1000,		&fps,		"Frames per seconds (processing speed)"));

		// Lock class parameter as it can never be changed // TODO: make this work
		// Init();
		// RefParameterByName("class").Lock();
		// RefParameterByName("width").Lock();
		// RefParameterByName("height").Lock();
	}

public:
	int width;
	int height;
	int type;
	bool autoProcess;
	double fps;
	std::string objClass;
};

class Module : Configurable
{
public:
	Module(const ConfigReader& x_confReader);
	virtual ~Module();
	
	virtual void Reset();
	void Pause(bool x_pause);
	void Process();

	const std::string& GetName() const{return m_name;}
	const std::string& GetDescription() const{return m_description;}
	int GetId() const {return m_id;}

	const std::vector<Stream*>& GetInputStreamList() const {return m_inputStreams;}
	const std::vector<Stream*>& GetOutputStreamList() const {return m_outputStreams;}
	const std::vector<Stream*>& GetDebugStreamList() const {return m_debugStreams;}
	const std::vector<ControlBoard*>& GetControlList() const {return m_controls;}
	virtual ModuleParameterStructure & RefParameter() = 0;
	
	// inline int GetWidth() {return RefParameter().width;}
	// inline int GetHeight(){return RefParameter().height;}
	// inline int GetType()  {return RefParameter().type;}
	virtual double GetRecordingFps();
	
	inline void SetPreceedingModule(Module & x_module){m_modulePreceeding = &x_module;}
	inline void AddDependingModule (Module & x_module){m_modulesDepending.push_back(&x_module);}
	virtual void PrintStatistics(std::ostream& os) const;
	
	virtual inline bool IsInput() {return false;}
	void Export(std::ostream& rx_os, int x_indentation);
	Stream * GetInputStreamById(int x_id) const;
	Stream * GetOutputStreamById(int x_id) const;
	inline void LockForRead(){m_lock.lockForRead();};
	inline void LockForWrite(){m_lock.lockForWrite();};
	inline void Unlock(){m_lock.unlock();};
	
protected:

	// for benchmarking
	long long m_timerConvertion;
	long long m_timerProcessing;
	long long m_timerWaiting;
	long long m_countProcessedFrames;

	TIME_STAMP m_lastTimeStamp;     // time stamp of the lastly processed input
	TIME_STAMP m_currentTimeStamp;  // time stamp of the current input
	bool m_pause;
	
	virtual void ProcessFrame() = 0;
	inline virtual bool IsInputProcessed() const {return true;}

	// Streams
	std::vector<Stream *> m_inputStreams;
	std::vector<Stream *> m_outputStreams;
	std::vector<Stream *> m_debugStreams;	

	std::vector<ControlBoard *> m_controls;
	std::string m_name;
	std::string m_description; 
	int m_id;
	Module * m_modulePreceeding;
	std::vector<Module *> m_modulesDepending;
	QModuleTimer * m_moduleTimer;
	QReadWriteLock m_lock;
};

#endif
