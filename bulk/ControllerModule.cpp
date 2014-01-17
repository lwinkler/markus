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

#include "ControllerModule.h"
#include "Module.h"

using namespace std;


/*--------------------------------------------------------------------------------*/

void reset(Controller* x_ctr)
{
	ControllerModule* ctr = dynamic_cast<ControllerModule*>(x_ctr);
	assert(ctr != NULL);
	ctr->module.Reset();
}

void pauseModule(Controller* x_ctr)
{
	ControllerModule* ctr = dynamic_cast<ControllerModule*>(x_ctr);
	assert(ctr != NULL);
	ctr->module.Pause(true);
}

void unpauseModule(Controller* x_ctr)
{
	ControllerModule* ctr = dynamic_cast<ControllerModule*>(x_ctr);
	assert(ctr != NULL);
	ctr->module.Pause(false);
}

void printStatistics(Controller* x_ctr)
{
	ControllerModule* ctr = dynamic_cast<ControllerModule*>(x_ctr);
	assert(ctr != NULL);
	ctr->module.PrintStatistics();
}


ControllerModule::ControllerModule(Module& rx_module) :
	Controller("Module"),
	module(rx_module)
{
	m_actions.insert(std::make_pair("Reset",   &reset));
	m_actions.insert(std::make_pair("Pause",   &pauseModule));
	m_actions.insert(std::make_pair("Unpause",   &unpauseModule));
	m_actions.insert(std::make_pair("Print statistics",   &printStatistics));
}

QWidget* ControllerModule::CreateWidget()
{
	return NULL;
}
