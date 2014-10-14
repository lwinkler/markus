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

#include "ParameterImageType.h"
#include "ParameterNum.h"
#include "ParameterString.h"
#include "Controller.h"
#include "Processable.h"
#include <log4cxx/logger.h>
#include <opencv2/core/core.hpp>

#define MAX_WIDTH  6400
#define MAX_HEIGHT 4800



class Stream;
class QModuleTimer;

/**
* @brief Class representing a module. A module is a node of the application, it processes streams
*/
class Module : public Processable, public Controllable, public Serializable
{
public:
	class Parameters : public Processable::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) : Processable::Parameters(x_confReader)
		{
			m_list.push_back(new ParameterString("class", 		"", 					&objClass,	"Class of the module (define the module's function)"));
			m_list.push_back(new ParameterInt("width", 	640, 	1, MAX_WIDTH,		&width,		"Width of the input"));
			m_list.push_back(new ParameterInt("height", 	480, 	1, MAX_HEIGHT,		&height,	"Height of the input"));
			m_list.push_back(new ParameterImageType("type", 	CV_8UC1, 				&type,		"Format of the input image"));

			Init();
		}

		int width;
		int height;
		int type;
		std::string objClass;
	};

	Module(const ConfigReader& x_confReader);
	virtual ~Module();
	
	virtual void Reset();
	void Pause(bool x_pause);
	void Process();

	const std::string& GetName() const{return m_name;}
	virtual const std::string& GetClass() const = 0;
	virtual const std::string& GetDescription() const = 0;
	int GetId() const {return m_id;}
	virtual const Parameters & GetParameters() const = 0;

	const std::map<int, Stream*>& GetInputStreamList() const {return m_inputStreams;}
	const std::map<int, Stream*>& GetOutputStreamList() const {return m_outputStreams;}
	const std::map<int, Stream*>& GetDebugStreamList() const {return m_debugStreams;}
	void AddInputStream(int x_id, Stream* xp_stream);
	void AddOutputStream(int x_id, Stream* xp_stream);
	void AddDebugStream(int x_id, Stream* xp_stream);


	inline int GetWidth() const          {return GetParameters().width;}
	inline int GetHeight() const         {return GetParameters().height;}
	inline int GetImageType() const      {return GetParameters().type;}
	inline double GetFps() const         {return GetParameters().fps;}
	inline bool IsAutoProcessed() const  {return GetParameters().autoProcess;}
	virtual double GetRecordingFps() const;
	
	inline void AddDependingModule (Module & x_module){m_modulesDepending.push_back(&x_module);}
	virtual void PrintStatistics() const;
	virtual void Serialize(std::ostream& stream, const std::string& x_dir) const;
	virtual void Deserialize(std::istream& stream, const std::string& x_dir);
	
	virtual inline bool IsInput() {return false;}
	void Export(std::ostream& rx_os, int x_indentation);
	Stream& RefInputStreamById(int x_id); 
	Stream& RefOutputStreamById(int x_id);
	inline void LockForRead(){m_lock.lockForRead();}
	inline void LockForWrite(){m_lock.lockForWrite();}
	inline void Unlock(){m_lock.unlock();}
	inline bool IsReady(){return IsAutoProcessed() || m_isReady;}
	void SetAsReady();
	bool AllInputsAreReady() const;
	const Module& GetMasterModule() const;
	
protected:
	virtual Parameters & RefParameters() = 0;

	// for benchmarking
	long long m_timerConvertion;
	long long m_timerProcessing;
	long long m_timerWaiting;
	long long m_countProcessedFrames;

	TIME_STAMP m_lastTimeStamp;     // time stamp of the lastly processed input
	TIME_STAMP m_currentTimeStamp;  // time stamp of the current input
	bool m_isReady;
	bool m_unsyncWarning;
	
	virtual void ProcessFrame() = 0;
	inline virtual bool IsInputProcessed() const {return true;}

	// Streams
	std::map<int, Stream *> m_inputStreams;
	std::map<int, Stream *> m_outputStreams;
	std::map<int, Stream *> m_debugStreams;	

	std::string m_name;
	int m_id;
	std::vector<Module *> m_modulesDepending;
	QModuleTimer * m_moduleTimer;

private:
	static log4cxx::LoggerPtr m_logger;
};

#endif
