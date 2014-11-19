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

/**
* @brief Command: Reset the module
*
* @param xp_value unused
*/
void ControllerModule::Reset(string* xp_value)
{
	module.Reset();
}

/**
* @brief Pause the module
*
* @param xp_value unused
*/
void ControllerModule::Pause(string* xp_value)
{
	module.Pause(true);
}

/**
* @brief Unpause the module
*
* @param xp_value unused
*/
void ControllerModule::Unpause(string* xp_value)
{
	module.Pause(false);
}

/**
* @brief Print the statistics related to processing time
*
* @param xp_value unused
*/
void ControllerModule::PrintStatistics(string* xp_value)
{
	string benchFileName = module.GetContext().GetOutputDir() + "/" + module.GetName() + ".benchmark.xml";
	ConfigReader summary(benchFileName, true);
	module.PrintStatistics(summary);
	summary.SaveToFile(benchFileName);
}


ControllerModule::ControllerModule(Module& rx_module) :
	Controller("module"),
	module(rx_module)
{
	m_actions.insert(std::make_pair("Reset",           &ControllerModule::Reset));
	m_actions.insert(std::make_pair("Pause",           &ControllerModule::Pause));
	m_actions.insert(std::make_pair("Unpause",         &ControllerModule::Unpause));
	m_actions.insert(std::make_pair("PrintStatistics", &ControllerModule::PrintStatistics));
}

QWidget* ControllerModule::CreateWidget()
{
	return NULL;
}
