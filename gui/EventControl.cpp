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

#include "EventControl.h"
#include "ClassifyEvents/ClassifyEvents.h" // TODO: include base module in markus

using namespace std;

const std::string ControllerEvent::m_name = "EventControl";

EventControl::EventControl(const std::string& x_name, const std::string& x_description):
	ControlBoard(x_name, x_description)
{

}

EventControl::~EventControl()
{
}


void EventControl::SetModule(ClassifyEvents & rx_module)
{
	m_module = &rx_module;

	// Delete all controllers
	for(vector<Controller*>::iterator it = m_controllers.begin() ; it != m_controllers.end() ; it++)
	{
		delete(*it);
	}
	m_controllers.clear();

	Controller * ctrr = new ControllerEvent(rx_module);
	if(ctrr == NULL)
		throw MkException("Controller creation failed", LOC);
	else AddController(ctrr);
}

/*--------------------------------------------------------------------------------*/


ControllerEvent::ControllerEvent(ClassifyEvents& rx_module) :
	Controller(),
	m_module(rx_module)
{
	// m_widget = m_parameterSlider = new QParameterSlider(0, 0, m_module.GetMaxMsec(), 0);
}

void ControllerEvent::InvalidateEvent()
{
	//m_module.SetMsec(m_parameterSlider->GetValue());
}
