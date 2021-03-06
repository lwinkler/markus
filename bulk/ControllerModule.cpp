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
#include "util.h"

namespace mk {
using namespace std;


/*--------------------------------------------------------------------------------*/

/**
* @brief Command: Reset the module
*
* @param rx_value unused
*/
void ControllerModule::Reset(mkjson& rx_value)
{
	module.LockAndReset();
}


/**
* @brief Start the module
*
* @param rx_value unused
*/
void ControllerModule::Start(mkjson& rx_value)
{
	module.Start();
}

/**
* @brief Stop the module
*
* @param rx_value unused
*/
void ControllerModule::Stop(mkjson& rx_value)
{
	module.Stop();
}

/**
* @brief Print the statistics related to processing time
*
* @param rx_value unused
*/
void ControllerModule::PrintStatistics(mkjson& rx_value)
{
	Processable::ReadLock lock(module.RefLock());
	if(module.RefContext().RefOutputDir().FileExists(module.GetName() + ".benchmark.json"))
		module.RefContext().RefOutputDir().Rm(module.GetName() + ".benchmark.json");
	string benchFileName = module.RefContext().RefOutputDir().ReserveFile(module.GetName() + ".benchmark.json");

	mkconf summary;
	readFromFile(summary, benchFileName, true);
	module.PrintStatistics(summary);
	writeToFile(summary, benchFileName);
}


ControllerModule::ControllerModule(Module& rx_module) :
	Controller("module"),
	module(rx_module)
{
	m_actions.insert(make_pair("Reset",           &ControllerModule::Reset));
	m_actions.insert(make_pair("Start",           &ControllerModule::Start));
	m_actions.insert(make_pair("Stop",            &ControllerModule::Stop));
	m_actions.insert(make_pair("PrintStatistics", &ControllerModule::PrintStatistics));
}

QWidget* ControllerModule::CreateWidget()
{
	return nullptr;
}
} // namespace mk
