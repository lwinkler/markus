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

#include "ControllerEvent.h"
#include "ModuleClassifyEvents.h"

namespace mk {
using namespace std;


/*--------------------------------------------------------------------------------*/

/**
* @brief Command: Consider the last event as valid
*
* @param rx_value
*/
void ControllerEvent::Validate(mkjson& rx_value)
{
	Processable::WriteLock lock(m_module.RefLock());
	m_module.ValidateLastEvent();
	m_module.PopEvent();
}

/**
* @brief Command: Consider the last event as invalid
*
* @param rx_value
*/
void ControllerEvent::Invalidate(mkjson& rx_value)
{
	Processable::WriteLock lock(m_module.RefLock());
	m_module.InvalidateLastEvent();
	m_module.PopEvent();
}

ControllerEvent::ControllerEvent(ModuleClassifyEvents& rx_module) :
	Controller("event"),
	m_module(rx_module)
{
	m_actions.insert(make_pair("Validate",   &ControllerEvent::Validate));
	m_actions.insert(make_pair("Invalidate", &ControllerEvent::Invalidate));
}

QWidget* ControllerEvent::CreateWidget()
{
	return nullptr;
}
} // namespace mk
