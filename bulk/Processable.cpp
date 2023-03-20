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
#include "Manager.h"
#include "ModuleTimer.h"
#include "InterruptionManager.h"
#include "Event.h"

namespace mk {
using namespace std;

log4cxx::LoggerPtr Processable::m_logger(log4cxx::Logger::getLogger("Processable"));

Processable::Processable(ParameterStructure& xr_params) :
	Configurable(xr_params),
	m_lastException(MK_EXCEPTION_NORMAL, "normal", "No exception was thrown", "", ""),
	m_param(dynamic_cast<Parameters&>(xr_params)),
	m_interruptionManager(InterruptionManager::GetInst())
{
}

Processable::~Processable() = default;

/**
* @brief Reset the module. Parameters value are set to default
*/
void Processable::Reset()
{
	m_param.PrintParameters();
	m_param.CheckRangeAndThrow();

	// Lock all parameters if needed
	// note: maybe there is another place to do this: explicitely call after reading parameters
	m_param.LockIfRequired();

	// Add the module timer
	// Autoprocess is on for modules if decentralized and for manager if centralized
	if(m_param.autoProcess && ((GetName() == "manager") ^ (! GetContext().IsCentralized())))
	{
		// note LW: Since Reset can be called while processing, we do not allow to re-create a timer
		//          fixing this would be tricky. See module VideoFileReader
		if(!mp_moduleTimer)
			mp_moduleTimer.reset(new ModuleTimer(*this));

		LOG_DEBUG(m_logger, "Reseting auto-processed " << GetName() << " with real-time="<<GetContext().IsRealTime()<<" and fps="<<m_param.fps);
	}
	else
	{
		mp_moduleTimer.reset();
	}

	m_timerProcessable.Reset();
	m_hasRecovered = true;
	m_sleepTime    = 0;
}

/**
* @brief Start the processing thread. To be used after the reset
*
*/
void Processable::Start()
{
	// Set a timer for all modules in auto-process (= called at a regular frame rate)
	// all other are called as "slaves" of other modules
	// If not real-time, a module will try to acquire frames as fast as possible
	// this is usefull for a VideoFileReader input when we work offline
	if(!mp_moduleTimer)
		return;

	if(GetContext().IsRealTime())
	{
		// use GetRecordingFps to get the closest estimation of which fps to use (to mimic real-time)
		mp_moduleTimer->Start(GetRecordingFps());
	}
	else mp_moduleTimer->Start(0);
};


/**
* @brief Stop the processing thread. To be used before destructor
*
*/
void Processable::Stop()
{
	if(mp_moduleTimer)
		mp_moduleTimer->Stop();
};

/**
* @brief The processable will sleep before processing
*/
void Processable::SetSleep(int x_ms)
{
	// m_retryConnection++;
	m_sleepTime = x_ms; // m_retryConnection < 10 ? 10000 : 5 * 60 * 1000;
	LOG_INFO(m_logger, "Set a waiting of " << m_sleepTime << " ms");
}

/**
* @brief Stop the processing thread. To be used before destructor
*
*/
bool Processable::DoSleep()
{
	if(m_sleepTime > 0)
	{
		// If the manager is in a waiting state, wait 500 ms and return
		TIME_STAMP wait = MIN(m_sleepTime, 500);
		LOG_DEBUG(m_logger, "Sleep " << wait << " ms out of " << m_sleepTime);
		std::this_thread::sleep_for(std::chrono::milliseconds(wait));
		m_sleepTime -= wait;
		return true;
	}
	return false;
}

bool Processable::ProcessAndCatch()
{
	WriteLock lock(RefLock());
	bool recover      = true;
	bool continueFlag = true;
	
	// If the processable is asked to sleep, wait and do not process
	bool doSleep = DoSleep();

	m_timerProcessable.Start();
	try
	{
		if(!doSleep)
			Process();
	}
	catch(MkException& e)
	{
		// InterruptionManager::GetInst().AddEvent("exception." + e.GetName());
		if(m_hasRecovered)
		{
			// This exception happens after a recovery, keep it!
			m_lastException = e;
			NotifyException(m_lastException);
		}
		recover = m_hasRecovered = false;

		// note: handle errors by code as they may have been copied and de-generated
		if(e.GetCode() == MK_EXCEPTION_ENDOFSTREAM)
		{
			LOG_INFO(m_logger, GetName() << ": Exception raised (EndOfStream) : " << e.what());

			// test if all inputs are over
			if(AbortCondition())
			{
				continueFlag = false;
			}
		}
		else if(e.IsFatal())
		{
			LOG_ERROR(m_logger, GetName() << ": Fatal exception raised, aborting : " << e.what());
			// continueFlag = false;
			return false;
		}
		else
		{
			LOG_ERROR(m_logger, "(Markus exception " << e.GetCode() << "): " << e.what());
			// continueFlag = false;
		}
		if(!GetContext().GetParameters().robust)
			return false;
	}
	catch(exception& e)
	{
		if(m_hasRecovered)
		{
			// This exception happens after a recovery, keep it!
			m_lastException = MkException(GetName() + " :" + string(e.what()), LOC);
			NotifyException(m_lastException);
		}
		recover = m_hasRecovered = false;
		LOG_ERROR(m_logger, GetName() << ": Exception raised (std::exception): " << e.what());
	}
	catch(string& str)
	{
		if(m_hasRecovered)
		{
			// This exception happens after a recovery, keep it!
			m_lastException = MkException(GetName() + " :" + string(str), LOC);
			NotifyException(m_lastException);
		}
		recover = m_hasRecovered = false;
		LOG_ERROR(m_logger, GetName() << ": Exception raised (string): " << str);
	}
	catch(const char* str)
	{
		if(m_hasRecovered)
		{
			// This exception happens after a recovery, keep it!
			m_lastException = MkException(GetName() + " :" + string(str), LOC);
			NotifyException(m_lastException);
		}
		recover = m_hasRecovered = false;
		LOG_ERROR(m_logger, GetName() << ": Exception raised (const char*): " << str);
	}
	catch(...)
	{
		if(m_hasRecovered)
		{
			// This exception happens after a recovery, keep it!
			m_lastException = MkException(GetName() + ": Unknown", LOC);
			NotifyException(m_lastException);
		}
		recover = m_hasRecovered = false;
		LOG_ERROR(m_logger, GetName() << ": Unknown exception raised");
	}

	m_timerProcessable.Stop();

	// If a full processing cycle has been made without exception,
	// we consider that the manager has recovered from exceptions
	if(recover && !doSleep)
	{
		m_hasRecovered = true;
	}

	return ManageInterruptions(continueFlag);
}

/**
* @brief Notify the parent process of an exception
*
* @param x_exception Exception to be notified
*/
void Processable::NotifyException(const MkException& x_exception)
{
	InterruptionManager::GetInst().AddEvent("exception." + x_exception.GetName());
	Event ev;
	ev.AddExternalInfo("exception", oneLine(x_exception));
	// note: it is difficult to associate a time stamp with events
	ev.Raise("exception", 0, 0);
	ev.Notify(GetContext(), true);
}

/**
* @brief Log the status of the application (last exception)
*/
void Processable::Status() const
{
	mkjson json(m_lastException);
	json["recovered"] = m_hasRecovered;
	Event evt;
	// note: it is difficult to associate a time stamp with events
	evt.Raise("status", 0, 0);
	evt.AddExternalInfo("exception", oneLine(json));
	evt.Notify(GetContext(), true);
}

} // namespace mk
