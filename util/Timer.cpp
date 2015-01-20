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
#include "Timer.h"
#include <unistd.h>

long int Timer::GetMSecLong()
{
	timeval now;
	//clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &now);
	gettimeofday(&now, NULL); // TODO: use clock as in blog.habets.pp.se: gettimeofday should never be used to measure time

	long seconds  = now.tv_sec  - m_start.tv_sec;
	long useconds = now.tv_usec - m_start.tv_usec;

	return seconds * 1000 + useconds / 1000;
}

double Timer::GetSecDouble()
{
	timeval now;
	gettimeofday(&now, NULL);
	//DIFF(m_start, now, temp);
	long seconds  = now.tv_sec  - m_start.tv_sec;
	long useconds = now.tv_usec - m_start.tv_usec;

	return seconds + useconds / 1000000.0;
}

float Timer::GetSecFloat()
{
	timeval now;
	gettimeofday(&now, NULL);
	//DIFF(m_start, now, temp);
	long seconds  = now.tv_sec  - m_start.tv_sec;
	long useconds = now.tv_usec - m_start.tv_usec;

	return seconds + useconds / 1000000.0f;
}


