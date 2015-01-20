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

#include <sys/time.h>
#include <unistd.h>

/// Timer class used for benchmarking
class Timer
{
public:
	Timer() {Restart();}
	long GetMSecLong();
	float GetSecFloat();
	double GetSecDouble();
	inline void Restart() {gettimeofday(&m_start, NULL);}

protected:
	timeval m_start;
};

#endif
