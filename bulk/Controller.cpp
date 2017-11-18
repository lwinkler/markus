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

#include "Controller.h"

using namespace std;

log4cxx::LoggerPtr Controller::m_logger(log4cxx::Logger::getLogger("Controller"));

Controller::Controller(const string& x_name) :
	m_name(x_name)
{
}


Controller::~Controller()
{
}

/**
* @brief Find a controller by name
*
* @param x_name
*
* @return Reference to controller
*/
Controller& Controllable::FindController(const string& x_name) const
{
	auto it = m_controls.find(x_name);
	if(it == m_controls.end())
		throw MkException("Cannot find controller " + x_name + " in controls list", LOC);
	return *(it->second);
}
