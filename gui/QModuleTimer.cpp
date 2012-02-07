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

#include "QModuleTimer.h"
#include "Module.h"

#include <QTimer>

using namespace std;

QModuleTimer::QModuleTimer(Module& x_module)
: m_module(x_module)
{
	
	double delay = 10;
	if(m_module.GetFps() > 0)
	{
		// Start a timer for module process
		delay = 1000.0 / m_module.GetFps();
		m_timeInterval = 1 / m_module.GetFps();
		start(delay);
	}
	else
	{
		// We do not start the timer :
		//  the module will be called from another module
		delay = m_timeInterval = 0;
	}
}


void QModuleTimer::timerEvent(QTimerEvent* )
{
	m_module.Process(m_timeInterval);
}

