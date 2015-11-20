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

#ifndef QMODULE_TIMER_H
#define QMODULE_TIMER_H

#include <thread>
#include <chrono>
#include <atomic>
#include <assert.h>

class Processable;

/// A timer associated with a module (for auto processing, mostly used in input modules)
class QModuleTimer
{
public:
	QModuleTimer(Processable & x_module);
	virtual ~QModuleTimer(){assert(!m_running);}
	void Reset(double x_fps);

	// typedef std::function<void(void)> Timeout;

	inline void Stop(){m_running = false;m_thread.join();}
	void Start();

protected:
	Processable & m_processable;
	std::thread m_thread;
	std::atomic<bool> m_running;
	double m_delay = 0;
};


#endif
