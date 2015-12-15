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
#include "util.h"


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

	double delay = 0.0;
	if(x_fps > 0)
	{
		// Start a timer for module process
		delay = 1.0 / x_fps;
	}
	
	m_running = true;
	// auto ms = chrono::milliseconds((long) delay * 1000);
	// std::chrono::seconds<1, double> ms(delay);

	m_thread = thread([this, delay, x_fps]()
	{
		TIME_STAMP waitMs = delay * 1000;
		while (m_running == true)
		{
			TIME_STAMP ts = getAbsTimeMs();
			bool ret = m_processable.ProcessAndCatch();

			// Wait to respect the fps
			ts = getAbsTimeMs() - ts;
			// cout << "runn" << ts << " " << waitMs << " " << x_fps << endl;
			if(ts < waitMs)
			{
				LOG_DEBUG(m_logger, "Sleep ms " << (waitMs - ts));
				usleep((waitMs - ts) * 1000);
			}

			if(!ret)
			{
				LOG_INFO(m_logger, "Exiting main loop");
				break;
			}
		}
		// Stop();
	});
}

