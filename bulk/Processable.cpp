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

#include "Module.h"
#include "ModuleTimer.h"
#include "InterruptionManager.h"
#include "Event.h"

using namespace std;

log4cxx::LoggerPtr Processable::m_logger(log4cxx::Logger::getLogger("Processable"));

Processable::Processable(ParameterStructure& xr_params) :
	Configurable(xr_params),
	m_lastException(MK_EXCEPTION_NORMAL, "normal", "No exception were thrown", "", ""),
	m_param(dynamic_cast<const Parameters&>(xr_params)),
	m_interruptionManager(InterruptionManager::GetInst())
{
	m_allowAutoProcess = true;
	m_realTime         = true;
	m_moduleTimer      = nullptr;
	mp_context         = nullptr;
}

Processable::~Processable()
{
	CLEAN_DELETE(m_moduleTimer);
}

/**
* @brief Reset the module. Parameters value are set to default
*/
void Processable::Reset()
{
	// Add the module timer (only works with QT)
	if(m_param.autoProcess && m_allowAutoProcess)
	{
		if(m_moduleTimer != nullptr)
		{
			m_moduleTimer->Stop(); // TODO How to handle this: assert ?
			CLEAN_DELETE(m_moduleTimer);
		}
		cout << "MT " << GetName() << endl;
		m_moduleTimer = new ModuleTimer(*this);

		LOG_DEBUG(m_logger, "Reseting auto-processed module with real-time="<<m_realTime<<" and fps="<<m_param.fps);
	}
	else m_moduleTimer = nullptr;

	m_timerProcessing.Reset();
	m_hasRecovered = true;
	m_continueFlag = true;
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
	Module* mod = dynamic_cast<Module*>(this);
	double recfps = mod == nullptr ? 0 : mod->GetRecordingFps();
	if(m_moduleTimer != nullptr)
		m_moduleTimer->Start(m_realTime ? recfps : 0);
};


/**
* @brief Stop the processing thread. To be used before destructur
*
*/
void Processable::Stop()
{
	if(m_moduleTimer != nullptr)
		m_moduleTimer->Stop();
};


bool Processable::ProcessAndCatch()
{
	bool recover = true;
	bool ret = true;
	m_timerProcessing.Start();
	try{
		ret = Process();
	}
	catch(FatalException& e)
	{
		LOG_ERROR(m_logger, GetName() << ": Exception raised (FatalException), aborting : " << e.what());
		m_continueFlag = recover = m_hasRecovered = false;
		exit(MK_EXCEPTION_FATAL - MK_EXCEPTION_FIRST);
	}
	catch(EndOfStreamException& e)
	{
		// elem->Stop(); // TODO: Do in caller
		if(m_hasRecovered)
		{
			// This exception happens after a recovery, keep it!
			m_lastException = e;
			NotifyException(m_lastException);
		}
		recover = m_hasRecovered = false;

		LOG_INFO(m_logger, GetName() << ": Exception raised (EndOfStream) : " << e.what());

		// test if all inputs are over
		if(true) // TODO EndOfAllStreams())
		{
			InterruptionManager::GetInst().AddEvent("exception." + e.GetName());
			// TODO: test what happens if the stream of a camera is cut
			LOG_INFO(m_logger, "End of all video streams : Manager::Process");
			return false;
		}
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
		LOG_ERROR(m_logger, "(Markus exception " << e.GetCode() << "): " << e.what());
	}
	catch(exception& e)
	{
		if(m_hasRecovered)
		{
			// This exception happens after a recovery, keep it!
			m_lastException = MkException(GetName() + " :" + string(e.what()), LOC);
			NotifyException(m_lastException);
		}
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

		// If a full processing cycle has been made without exception,
		// we consider that the manager has recovered from exceptions
	if(recover)
		m_hasRecovered = true;

	// Send command generated by interruptions
	m_continueFlag = true; // TODO m_continueFlag && (m_param.nbFrames == 0 || m_param.nbFrames != m_frameCount);

	// note: we need send an event to stay in the loop
	if(!m_continueFlag && m_param.autoProcess)
		m_interruptionManager.AddEvent("event.stopped");

	return m_continueFlag;
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
	ev.Raise("exception");
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
	evt.Raise("status");
	ss.clear();
	ss << root;
	evt.AddExternalInfo("exception", ss);
	evt.Notify(GetContext(), true);
}
