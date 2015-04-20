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

#ifndef MK_TIMER_H
#define MK_TIMER_H

#include <unistd.h>
#include <opencv2/flann/timer.h>

/// Timer class used for benchmarking
class Timer
{
public:
	Timer() {}
	long GetMsLong() const;
	double GetSecDouble() const;
	inline void Reset(){m_timer.reset();};
	inline void Start(){m_timer.start();}
	inline void Stop(){m_timer.stop();}
	inline void Add(const Timer& x_val){m_timer.value += x_val.GetSecDouble();}

protected:
	cvflann::StartStopTimer m_timer;
};

#endif
