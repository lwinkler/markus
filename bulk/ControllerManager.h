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

#ifndef CONTROLLER_MANAGER_H
#define CONTROLLER_MANAGER_H

#include "Controller.h"

namespace mk {
class Manager;

/**
* @brief Control class for the manager
*/
class ControllerManager : public Controller
{
public:
	explicit ControllerManager(Manager& rx_module);
	virtual ~ControllerManager() {}
	MKCLASS("ControllerManager")

	QWidget* CreateWidget() override;

	typedef void (ControllerManager::*action)(mkjson&);
	DECLARE_CALL_ACTION(action);
	DECLARE_LIST_ACTION(action);

	// Controls
	void Reset(mkjson& rx_value);
	void ResetExceptInputs(mkjson& rx_value);
	void Quit(mkjson& rx_value);
	void Start(mkjson& rx_value);
	void Stop(mkjson& rx_value);
	void Rebuild(mkjson& rx_value);
	void SetSleep(mkjson& rx_value);
	void ProcessOne(mkjson& rx_value);
	void PrintStatistics(mkjson& rx_value);
	void Status(mkjson& rx_value);
	void WriteStateToDirectory(mkjson& rx_value);

protected:
	std::map<std::string, const action> m_actions;
	Manager  & manager;
};
} // namespace mk
#endif
