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

#ifndef EVENT_CONTROL_H
#define EVENT_CONTROL_H

#include "Controller.h"

namespace mk {
class ModuleClassifyEvents;

/**
* @brief Control class for an event classifier
*/
class ControllerEvent : public Controller
{
public:
	explicit ControllerEvent(ModuleClassifyEvents& rx_module);
	~ControllerEvent() override {}
	MKCLASS("ControllerEvent")

	QWidget* CreateWidget() override;

	// Controls
	void Validate(mkjson& rx_value);
	void Invalidate(mkjson& rx_value);

	typedef void (ControllerEvent::*action)(mkjson&);
	DECLARE_CALL_ACTION(action);
	DECLARE_LIST_ACTION(action);

protected:
	std::map<std::string, const action> m_actions;
	ModuleClassifyEvents  & m_module;
};
} // namespace mk
#endif
