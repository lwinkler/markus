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

#include "ControllerManager.h"
#include "Manager.h"

using namespace std;


/*--------------------------------------------------------------------------------*/

void resetManager(Controller* xp_ctr, string* xp_value)
{
	ControllerManager* ctr = dynamic_cast<ControllerManager*>(xp_ctr);
	assert(ctr != NULL);
	ctr->manager.Reset();
}

void pauseManager(Controller* xp_ctr, string* xp_value)
{
	ControllerManager* ctr = dynamic_cast<ControllerManager*>(xp_ctr);
	assert(ctr != NULL);
	ctr->manager.Pause(true);
}

void quitManager(Controller* xp_ctr, string* xp_value)
{
	ControllerManager* ctr = dynamic_cast<ControllerManager*>(xp_ctr);
	assert(ctr != NULL);
	ctr->manager.Quit();
}


void unpauseManager(Controller* xp_ctr, string* xp_value)
{
	ControllerManager* ctr = dynamic_cast<ControllerManager*>(xp_ctr);
	assert(ctr != NULL);
	ctr->manager.Pause(false);
}

void printStatisticsManager(Controller* xp_ctr, string* xp_value)
{
	ControllerManager* ctr = dynamic_cast<ControllerManager*>(xp_ctr);
	assert(ctr != NULL);
	ctr->manager.PrintStatistics();
}


ControllerManager::ControllerManager(Manager& rx_manager) :
	Controller("manager", "manager"),
	manager(rx_manager)
{
	m_actions.insert(std::make_pair("Reset",           &resetManager));
	m_actions.insert(std::make_pair("Quit",            &quitManager));
	m_actions.insert(std::make_pair("Pause",           &pauseManager));
	m_actions.insert(std::make_pair("Unpause",         &unpauseManager));
	m_actions.insert(std::make_pair("PrintStatistics", &printStatisticsManager));
}

QWidget* ControllerManager::CreateWidget()
{
	return NULL;
}
