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
#include <boost/lexical_cast.hpp>

namespace mk {
using namespace std;


/*--------------------------------------------------------------------------------*/

/**
* @brief Command: Reset the manager
*
* @param rx_value unused
*/
void ControllerManager::Reset(mkjson& rx_value)
{
	Processable::WriteLock lock(manager.RefLock());
	manager.Reset();
}

/**
* @brief Command: Reset all the modules except inputs
*
* @param rx_value unused
*/
void ControllerManager::ResetExceptInputs(mkjson& rx_value)
{
	Processable::WriteLock lock(manager.RefLock());
	manager.Reset(false);
}

/**
* @brief Command: Start all the modules
*
* @param rx_value unused
*/
void ControllerManager::Start(mkjson& rx_value)
{
	manager.Start();
}

/**
* @brief Command: Stop all processing and quit (only working if centralized)
*
* @param rx_value unused
*/
void ControllerManager::Quit(mkjson& rx_value)
{
	manager.Quit();
}


/**
* @brief Command: Stop all the modules
*
* @param rx_value
*/
void ControllerManager::Stop(mkjson& rx_value)
{
	manager.Stop();
}

/**
* @brief Command: Destroy and rebuild all modules
*
* @param rx_value
*/
void ControllerManager::Rebuild(mkjson& rx_value)
{
	Processable::WriteLock lock(manager.RefLock());
	manager.Rebuild();
}

/**
* @brief Command: The manager will sleep before next processing (centralized mode only)
*
* @param rx_value
*/
void ControllerManager::SetSleep(mkjson& rx_value)
{
	manager.SetSleep(rx_value.get<int>());
}


/**
* @brief Command: Process one frame
*
* @param rx_value
*/
void ControllerManager::ProcessOne(mkjson& rx_value)
{
	manager.Stop();
	manager.ProcessAndCatch();
}

/**
* @brief Commands: Print statistics related to the modules processing
*
* @param rx_value unused
*/
void ControllerManager::PrintStatistics(mkjson& rx_value)
{
	Processable::ReadLock lock(manager.RefLock());
	manager.PrintStatistics();
}

/**
* @brief Command: Outputs the status (last exceptions)
*
* @param rx_value unused
*/
void ControllerManager::Status(mkjson& rx_value)
{
	Processable::ReadLock lock(manager.RefLock());
	manager.Status();
}

/**
* @brief Command: Write the state of all the modules to the output directory
*
* @param rx_value unused
*/
void ControllerManager::WriteStateToDirectory(mkjson& rx_value)
{
	Processable::ReadLock lock(manager.RefLock());
	manager.WriteStateToDirectory("state_" + timeStamp());
}

ControllerManager::ControllerManager(Manager& rx_manager) :
	Controller("manager"),
	manager(rx_manager)
{
	m_actions.insert(make_pair("Reset",              &ControllerManager::Reset));
	m_actions.insert(make_pair("ResetExceptInputs",  &ControllerManager::ResetExceptInputs));
	m_actions.insert(make_pair("Quit",               &ControllerManager::Quit));
	m_actions.insert(make_pair("Start",              &ControllerManager::Start));
	m_actions.insert(make_pair("Stop",               &ControllerManager::Stop));
	m_actions.insert(make_pair("Rebuild",            &ControllerManager::Rebuild));
	m_actions.insert(make_pair("SetSleep",           &ControllerManager::SetSleep));
	m_actions.insert(make_pair("ProcessOne",         &ControllerManager::ProcessOne));
	m_actions.insert(make_pair("PrintStatistics",    &ControllerManager::PrintStatistics));
	m_actions.insert(make_pair("Status",             &ControllerManager::Status));
	m_actions.insert(make_pair("WriteStateToDir",    &ControllerManager::WriteStateToDirectory));
}

QWidget* ControllerManager::CreateWidget()
{
	return nullptr;
}
}
