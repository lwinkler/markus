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
#include "ClassifyEvents/ClassifyEvents.h"

using namespace std;


/*--------------------------------------------------------------------------------*/

void validate(Controller* x_ctr)
{
	ControllerEvent* ctr = dynamic_cast<ControllerEvent*>(x_ctr);
	assert(ctr != NULL);
	ctr->module.ValidateLastEvent();
}

void invalidate(Controller* x_ctr)
{
	ControllerEvent* ctr = dynamic_cast<ControllerEvent*>(x_ctr);
	assert(ctr != NULL);
	ctr->module.InvalidateLastEvent();
}

ControllerEvent::ControllerEvent(ClassifyEvents& rx_module) :
	Controller("Event"),
	module(rx_module)
{
	m_actions.insert(std::make_pair("Validate",   &validate));
	m_actions.insert(std::make_pair("Invalidate", &invalidate));
}

QWidget* ControllerEvent::CreateWidget()
{
	return NULL;
}
