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
#include "util.h"

using namespace std;


/*--------------------------------------------------------------------------------*/

void ControllerManager::Reset(string* xp_value)
{
	manager.Reset();
}

void ControllerManager::ResetExceptInputs(string* xp_value)
{
	manager.Reset(false);
}

void ControllerManager::Pause(string* xp_value)
{
	manager.Pause(true);
}

void ControllerManager::Quit(string* xp_value)
{
	manager.Quit();
}


void ControllerManager::Unpause(string* xp_value)
{
	manager.Pause(false);
}

void ControllerManager::PrintStatistics(string* xp_value)
{
	manager.PrintStatistics();
}

void ControllerManager::Status(string* xp_value)
{
	manager.Status();
}

void ControllerManager::WriteStateToDirectory(std::string* xp_value)
{
	manager.WriteStateToDirectory(Manager::OutputDir() + "/state_" + timeStamp());
}

ControllerManager::ControllerManager(Manager& rx_manager) :
	Controller("manager", "manager"),
	manager(rx_manager)
{
	m_actions.insert(std::make_pair("Reset",              &ControllerManager::Reset));
	m_actions.insert(std::make_pair("ResetExceptInputs",  &ControllerManager::ResetExceptInputs));
	m_actions.insert(std::make_pair("Quit",               &ControllerManager::Quit));
	m_actions.insert(std::make_pair("Pause",              &ControllerManager::Pause));
	m_actions.insert(std::make_pair("Unpause",            &ControllerManager::Unpause));
	m_actions.insert(std::make_pair("PrintStatistics",    &ControllerManager::PrintStatistics));
	m_actions.insert(std::make_pair("Status",             &ControllerManager::Status));
	m_actions.insert(std::make_pair("WriteStateToDir",    &ControllerManager::WriteStateToDirectory));
}

QWidget* ControllerManager::CreateWidget()
{
	return NULL;
}
