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

#include "ControllerBackground.h"
#include "BackgroundSubtraction.h"
#include "util.h"

namespace mk {
using namespace std;


/*--------------------------------------------------------------------------------*/

/**
* @brief Command: Consider the last event as invalid
*
* @param rx_value
*/
void ControllerBackground::StartOnlineLearning(mkjson& rx_value)
{
	Processable::WriteLock lock(m_module.RefLock());
	m_module.StartOnlineLearning(rx_value.is_null() ? false : rx_value.get<bool>());
}

ControllerBackground::ControllerBackground(BackgroundSubtraction& rx_module) :
	Controller("background"),
	m_module(rx_module)
{
	m_actions.insert(make_pair("StartOnlineLearning", &ControllerBackground::StartOnlineLearning));
}

QWidget* ControllerBackground::CreateWidget()
{
	return nullptr;
}
} // namespace mk
