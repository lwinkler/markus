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

class ClassifyEvents;

/**
* @brief Control class for an event classifier
*/
class ControllerEvent : public Controller
{
public:
	ControllerEvent(ClassifyEvents& rx_module);
	~ControllerEvent(){}
	virtual QWidget* CreateWidget();

	// Controls
	void Validate(std::string* xp_value);
	void Invalidate(std::string* xp_value);

	typedef void (ControllerEvent::*action)(std::string*);
	DECLARE_CALL_ACTION(action);
	DECLARE_LIST_ACTION(action);

protected:
	std::map<std::string, const action> m_actions;
	ClassifyEvents  & module;
};
#endif
