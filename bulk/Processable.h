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
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public License
*    along with Markus.  If not, see <http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------------------*/

#ifndef PROCESSABLE_H
#define PROCESSABLE_H

#include "Configurable.h"
#include "ParameterStructure.h"
#include "ParameterNum.h"
#include "Context.h"
#include "Timer.h"
#include <log4cxx/logger.h>
#include <boost/thread/shared_mutex.hpp>

class ModuleTimer;
class InterruptionManager;

/**
* @brief Class representing a module. A module is a node of the application, it processes streams
*/
class Processable : public Configurable
{
public:
	// Create a lock (for outside use)
	typedef boost::shared_mutex Lock;
	typedef boost::unique_lock<Lock> WriteLock;
	typedef boost::shared_lock<Lock> ReadLock;

	class Parameters : public ParameterStructure
	{
	public:
		Parameters(const std::string& x_name) : ParameterStructure(x_name)
		{
			AddParameter(new ParameterBool("auto_process"       , 0 , 0 , 1    , &autoProcess      , "If yes the module processes with a timer at fixed fps, if no the module processes based on the time stamp of the input stream"));
			AddParameter(new ParameterDouble("fps"              , 0 , 0 , 1000 , &fps              , "Frames per seconds (processing speed)"));
		}

		bool autoProcess;
		double fps;
	};
	Processable(ParameterStructure& xr_params);
	virtual ~Processable();

	virtual void Reset();
	virtual void Process() = 0;
	virtual bool AbortCondition() const = 0;
	virtual const std::string& GetName() const = 0;
	virtual double GetRecordingFps() const = 0;
	bool ProcessAndCatch();
	inline virtual bool ManageInterruptions(bool x_continueFlag){return x_continueFlag;}
	virtual void Start();
	virtual void Stop();
	void Status() const;
	void SetSleep(int x_ms);
	bool DoSleep();
	inline virtual void SetContext(Context& x_context) {if(mp_context != nullptr) throw MkException("Context was already set", LOC); mp_context = &x_context;}
	inline virtual const Context& GetContext() const {if(mp_context == nullptr) throw MkException("Context was not set", LOC); return *mp_context;}
	inline virtual Context& RefContext() const {if(mp_context == nullptr) throw MkException("Context was not set", LOC); return *mp_context;}
	inline bool IsContextSet() const {return mp_context != nullptr;}
	inline Lock& RefLock() {return m_lock;}
	inline const MkException& LastException() const {return m_lastException;}
	inline void SetLastException(const MkException& x_excep) {m_lastException = x_excep;}
	inline bool HasRecovered() const {return m_hasRecovered;}

protected:
	void NotifyException(const MkException& x_exeption);

	InterruptionManager& m_interruptionManager;
	Timer m_timerProcessable;

private:
	Lock m_lock;
	bool m_hasRecovered     = true; // Flag to test if all modules have recovered from the last exception, only working if centralized
	MkException m_lastException;    // Field to store the last exception

	Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;
	ModuleTimer * mp_moduleTimer = nullptr;
	Context* mp_context = nullptr; /// context given by Manager (output directory, ...)

	// To handle disconnection
	TIME_STAMP m_sleepTime = 0;        // Time to sleep, used with certain exceptions
};

#endif
