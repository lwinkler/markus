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

#include "ModuleTimer.h"
#include "Processable.h"


using namespace std;

log4cxx::LoggerPtr ModuleTimer::m_logger(log4cxx::Logger::getLogger("ModuleTimer"));

ModuleTimer::ModuleTimer(Processable& x_module)
	: m_processable(x_module),
	m_running(false)
{}


void ModuleTimer::Start(double x_fps)
{
	if(m_running)
		return;

	m_delay = 0.01;
	if(x_fps > 0)
	{
		// Start a timer for module process
		m_delay = 1.0 / x_fps;
	}
	
	m_running = true;
	// auto ms = chrono::milliseconds((long) m_delay * 1000);
	int us = m_delay * 1000000;
	// std::chrono::seconds<1, double> ms(m_delay);

	m_thread = thread([=]()
	{
		while (m_running == true)
		{
			// this_thread::sleep_for(1s); // TODO In parallel
			usleep(us);
			if(!m_processable.ProcessAndCatch())
			{
				LOG_INFO(m_logger, "Exiting main loop");
				break; // Stop(); // TODO: Handle break with exceptions
			}
		}
		Stop();
	});
}

