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

#include <QTimer>

using namespace std;

QModuleTimer::QModuleTimer(Processable& x_module)
	: m_processable(x_module)
{
	// Reset(x_fps);
}

/**
* @brief Reset the timer
*
* @param x_fps The frame per second to be set
*/
void QModuleTimer::Reset(double x_fps)
{
	double delay = 1000.0 / 1000;
	if(x_fps > 0)
	{
		// Start a timer for module process
		delay = 1000.0 / x_fps;
	}
	start(delay);
}

void QModuleTimer::timerEvent(QTimerEvent* px_event)
{
	if(!m_processable.Process())
		stop();
}

