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
#include "util.h"

#include <iostream>

namespace mk {
using namespace std;

std::map<std::string, Timer> QuickTimer::ms_timers;

int64_t Timer::GetMsLong() const
{
	int64_t l = m_timer.value * 1000;
	GetSecDouble();
	return l;

}

double Timer::GetSecDouble() const
{
	return m_timer.value;
}

void Timer::Print() const
{
	cout<<"Timer has measured " << GetSecDouble() << "s in " << m_increments << " increments" << std::endl;
}

QuickTimer::QuickTimer(const std::string& x_description, const std::string& x_position, const std::string& x_function)
	: m_name(x_description + " at " + basename(x_position) + ":" + x_function)
{
	ms_timers[m_name].Start();
}

void QuickTimer::PrintTimers()
{
#ifndef MARKUS_DEBUG_STREAMS
	if(!ms_timers.empty())
		cout << "WARNING: Please remove all QuickTimers for a release version" << endl;
#endif

	for(const auto& elem : ms_timers)
	{
		cout<<"QuickTimer "<<elem.first;
		elem.second.Print();
	}
}
} // namespace mk
