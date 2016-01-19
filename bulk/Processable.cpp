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

using namespace std;

log4cxx::LoggerPtr Processable::m_logger(log4cxx::Logger::getLogger("Processable"));

Processable::Processable(ParameterStructure& xr_params) :
	Configurable(xr_params),
	m_lastException(MK_EXCEPTION_NORMAL, "normal", "No exception was thrown", "", ""),
	m_param(dynamic_cast<const Parameters&>(xr_params)),
	m_interruptionManager(InterruptionManager::GetInst())
{
}

Processable::~Processable()
{
	CLEAN_DELETE(mp_moduleTimer);
}

/**
* @brief Reset the module. Parameters value are set to default
*/
void Processable::Reset()
{
	// Add the module timer
	// Autoprocess is on for modules if decentralized and for manager if centralized
	if(m_param.autoProcess && (GetName() == "manager" ^ ! GetContext().IsCentralized()))
	{
		// note LW: Since Reset can be called while processing, we do not allow to re-create a timer
		//          fixing this would be tricky. See module VideoFileReader
		if(mp_moduleTimer == nullptr)
			mp_moduleTimer = new ModuleTimer(*this);

		LOG_DEBUG(m_logger, "Reseting auto-processed " << GetName() << " with real-time="<<GetContext().IsRealTime()<<" and fps="<<m_param.fps);
	}
	else
	{
		CLEAN_DELETE(mp_moduleTimer);
		mp_moduleTimer = nullptr;
	}

	m_timerProcessable.Reset();
	m_hasRecovered = true;
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
	if(mp_moduleTimer == nullptr)
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
	if(mp_moduleTimer != nullptr)
		mp_moduleTimer->Stop();
};


bool Processable::ProcessAndCatch()
{
	bool recover      = true;
	bool continueFlag = true;
	m_timerProcessable.Start();
	try
	{
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
				InterruptionManager::GetInst().AddEvent("event.stopped");
				try
				{
					// This is a trick to call event.stopped and manage interruptions
					dynamic_cast<Manager&>(*this).ManageInterruptions();
					continueFlag = !AbortCondition();
				}
				catch(...)
				{
					continueFlag = false;
				}
				
				// TODO: test what happens if the stream of a camera is cut
				LOG_INFO(m_logger, "Abort condition has been fulfilled (end of all streams)");
			}
		}
		else if(e.GetCode() == MK_EXCEPTION_FATAL)
		{
			LOG_ERROR(m_logger, GetName() << ": Exception raised (FatalException), aborting : " << e.what());
			continueFlag = false;
		}
		else LOG_ERROR(m_logger, "(Markus exception " << e.GetCode() << "): " << e.what());
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
	if(recover)
		m_hasRecovered = true;

	// Send command generated by interruptions
	// TODO remove
	// if(AbortCondition())
		// continueFlag = false;

	// note: we need send an event to stay in the loop
	if(!continueFlag && m_param.autoProcess)
		m_interruptionManager.AddEvent("event.stopped");

	return continueFlag;
}

/**
* @brief Notify the parent process of an exception
*
* @param x_exception Exception to be notified
*/
void Processable::NotifyException(const MkException& x_exception)
{
	InterruptionManager::GetInst().AddEvent("exception." + x_exception.GetName());// TODO keep this here ?
	stringstream ss;
	x_exception.Serialize(ss, "");
	Event ev;
	ev.AddExternalInfo("exception", ss);
	// note: it is difficult to associate a time stamp with events
	ev.Raise("exception", 0, 0);
	ev.Notify(GetContext(), true);
}

/**
* @brief Log the status of the application (last exception)
*/
void Processable::Status() const
{
	stringstream ss;
	m_lastException.Serialize(ss, "");
	Json::Value root;
	ss >> root;
	root["recovered"] = m_hasRecovered;
	Event evt;
	// note: it is difficult to associate a time stamp with events
	evt.Raise("status", 0, 0);
	ss.clear();
	ss << root;
	evt.AddExternalInfo("exception", ss);
	evt.Notify(GetContext(), true);
}
