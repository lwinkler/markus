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

#include "Control.h"

class ClassifyEvents;

/// Control class for an event classifier
class ControllerEvent : public Controller
{
public:
	ControllerEvent(ClassifyEvents& rx_module);
	~ControllerEvent(){};
	inline virtual const std::string& GetName() const {return m_name;} // TODO: Is this necessary ?
	void InvalidateEvent();
	virtual void SetControlledValue(){};
	virtual void GetCurrent(){};
	virtual void GetDefault(){};

protected:
	// QParameterSlider * m_parameterSlider;
	ClassifyEvents  & m_module;
	static const std::string m_name;
};


/// The control board for the module
class EventControl : public ControlBoard
{
public:
	EventControl(const std::string& x_name, const std::string& x_description);
	~EventControl();
	void SetModule(ClassifyEvents& rx_module);
private:
	ClassifyEvents  * m_module;
};

#endif
