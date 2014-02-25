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

#include <log4cxx/logger.h>
#include <log4cxx/xml/domconfigurator.h>

#define MAX_WIDTH  6400
#define MAX_HEIGHT 4800



class Stream;
class Controller;
class QModuleTimer;

class ModuleParameterStructure : public ParameterStructure
{
public:
	ModuleParameterStructure(const ConfigReader& x_confReader) : ParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterString("class", 		"", 					&objClass,	"Class of the module (define the module's function)"));
		m_list.push_back(new ParameterInt("width", 	640, 	1, MAX_WIDTH,		&width,		"Width of the input"));
		m_list.push_back(new ParameterInt("height", 	480, 	1, MAX_HEIGHT,		&height,	"Height of the input"));
		m_list.push_back(new ParameterImageType("type", 	CV_8UC1, 				&type,		"Format of the input image"));
		m_list.push_back(new ParameterBool("auto_process",  0, 	0, 	1,		&autoProcess,	"If yes the module processes with a timer at fixed fps, if no the module processes based on the time stamp of the input stream"));
		m_list.push_back(new ParameterBool("allow_unsync_input",  0, 0, 	1,		&allowUnsyncInput, "If yes the module accepts that its input can be on a different time stamp. Only relevant if the module has many inputs. Use at your own risks."));
		m_list.push_back(new ParameterDouble("fps", 	 0, 	0, 	1000,		&fps,		"Frames per seconds (processing speed)"));

		Init();
		RefParameterByName("class").Lock();
		RefParameterByName("width").Lock();
		RefParameterByName("height").Lock();
		// RefParameterByName("type").Lock();
		// RefParameterByName("auto_process").Lock();
		// RefParameterByName("allow_unsync_input").Lock();
	}

public:
	int width;
	int height;
	int type;
	bool autoProcess;
	bool allowUnsyncInput;
	double fps;
	std::string objClass;
};

class Module : public Configurable
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

	const std::map<int, Stream*>& GetInputStreamList() const {return m_inputStreams;}
	const std::map<int, Stream*>& GetOutputStreamList() const {return m_outputStreams;}
	const std::map<int, Stream*>& GetDebugStreamList() const {return m_debugStreams;}
	void AddInputStream(int x_id, Stream* xp_stream);
	void AddOutputStream(int x_id, Stream* xp_stream);
	void AddDebugStream(int x_id, Stream* xp_stream);


	const std::map<std::string, Controller*>& GetControllersList() const {return m_controls;}
	Controller* FindController(const std::string& x_name) const;
	
	inline int GetWidth() const          {return GetParameters().width;}
	inline int GetHeight() const         {return GetParameters().height;}
	inline int GetType() const           {return GetParameters().type;}
	inline double GetFps() const         {return GetParameters().fps;}
	inline bool IsAutoProcessed() const  {return GetParameters().autoProcess;}
	virtual double GetRecordingFps();
	
	inline void AddDependingModule (Module & x_module){m_modulesDepending.push_back(&x_module);}
	virtual void PrintStatistics() const;
	
	virtual inline bool IsInput() {return false;}
	void Export(std::ostream& rx_os, int x_indentation);
	Stream& RefInputStreamById(int x_id);
	Stream& RefOutputStreamById(int x_id);
	inline void LockForRead(){m_lock.lockForRead();}
	inline void LockForWrite(){m_lock.lockForWrite();}
	inline void Unlock(){m_lock.unlock();}
	inline bool IsReady(){return IsAutoProcessed() || m_isReady;}
	void SetAsReady();
	void SetProcessByTimer(bool x_proc) {m_processByTimer = x_proc;}
	bool AllInputsAreReady() const;
	const Module& GetMasterModule();
	
protected:

	// for benchmarking
	long long m_timerConvertion;
	long long m_timerProcessing;
	long long m_timerWaiting;
	long long m_countProcessedFrames;

	TIME_STAMP m_lastTimeStamp;     // time stamp of the lastly processed input
	TIME_STAMP m_currentTimeStamp;  // time stamp of the current input
	bool m_pause;
	bool m_isReady;
	bool m_processByTimer;
	bool m_unsyncWarning;
	
	virtual void ProcessFrame() = 0;
	inline virtual bool IsInputProcessed() const {return true;}

	// Streams
	std::map<int, Stream *> m_inputStreams;
	std::map<int, Stream *> m_outputStreams;
	std::map<int, Stream *> m_debugStreams;	

	std::map<std::string, Controller*> m_controls;
	std::string m_name;
	std::string m_description; 
	int m_id;
	std::vector<Module *> m_modulesDepending;
	QModuleTimer * m_moduleTimer;
	QReadWriteLock m_lock;

private:
	virtual const ModuleParameterStructure & GetParameters() const = 0;
	static log4cxx::LoggerPtr m_logger;
};

#endif
