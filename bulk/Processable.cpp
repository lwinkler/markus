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

using namespace std;

log4cxx::LoggerPtr Processable::m_logger(log4cxx::Logger::getLogger("Processable"));

Processable::Processable(const ConfigReader& x_configReader) :
	Configurable(x_configReader)
{
	m_pause            = false;
	m_allowAutoProcess = true;
	m_realTime         = true;
	m_moduleTimer      = nullptr;
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
	if(RefParameters().autoProcess && m_allowAutoProcess)
	{
		CLEAN_DELETE(m_moduleTimer);
		m_moduleTimer = new QModuleTimer(*this);

		// Set a timer for all modules in auto-process (= called at a regular frame rate)
		// all other are called as "slaves" of other modules
		// If not real-time, a module will try to acquire frames as fast as possible
		// this is usefull for a VideoFileReader input when we work offline
		m_moduleTimer->Reset(m_realTime ? RefParameters().fps : 0);
		LOG_DEBUG(m_logger, "Reseting auto-processed module with real-time="<<m_realTime<<" and fps="<<RefParameters().fps);
	}
	else m_moduleTimer = nullptr;
}

/**
* @brief Pause the module and stop the processing
*
* @param x_pause Pause on/off
*/
void Processable::Pause(bool x_pause)
{
	m_pause = x_pause;
	if(m_moduleTimer == nullptr)
		return;
	if(x_pause)
		m_moduleTimer->stop();
	else
		m_moduleTimer->start();
}


