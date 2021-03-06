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

#ifndef CONTROLLER_MODULE_H
#define CONTROLLER_MODULE_H

#include "Controller.h"

namespace mk {
class ControllerModule;
class Module;

/// Control class for a module
class ControllerModule : public Controller
{
public:
	explicit ControllerModule(Module& rx_module);
	~ControllerModule() override {}
	MKCLASS("ControllerModule")

	QWidget* CreateWidget() override;

	// Controls
	void Reset(mkjson& rx_value);
	void Start(mkjson& rx_value);
	void Stop(mkjson& rx_value);
	void PrintStatistics(mkjson& rx_value);

	typedef void (ControllerModule::*action)(mkjson&);
	DECLARE_CALL_ACTION(action);
	DECLARE_LIST_ACTION(action);

protected:
	std::map<std::string, const action> m_actions;
	Module  & module;
};
} // namespace mk
#endif
