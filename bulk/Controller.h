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

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <assert.h>
#include <log4cxx/logger.h>
#include <string>
#include <vector>
#include "Parameter.h"

class QWidget;

namespace mk {
class Module;

// Use macros to declare members of Controller childs, not the cleanest thing but you are free to try to
// improve on this
#define DECLARE_CALL_ACTION(action) \
void CallAction(const std::string& x_name, mkjson& rx_value) override\
{\
	auto it = m_actions.find(x_name);\
	if(it == m_actions.end())\
		throw MkException("Cannot find action " + x_name + " in controller", LOC);\
	(this->*(it->second))(rx_value);\
}
#define DECLARE_LIST_ACTION(action) \
void ListActions(std::vector<std::string>& xr_actions) const override\
{\
	for(const auto& elem : m_actions)\
		xr_actions.push_back(elem.first);\
}

/**
* @brief A class to send commands to a controllable object (module, parameter, ...)
*/
class Controller
{
public:
	explicit Controller(const std::string& x_name);
	virtual ~Controller();

	/// Create a QT Widget for this controller
	virtual QWidget* CreateWidget() = 0;
	const std::string& GetName() const {return m_name;}
	virtual const std::string& GetClass() const = 0;

	/**
	* @brief List all actions
	*
	* @param xr_actions output
	*/
	virtual void ListActions(std::vector<std::string>& xr_actions) const = 0;

	/**
	* @brief Call an action of the controller
	*
	* @param x_name   Action
	* @param xp_value Input/Output of the action (Ignored if null)
	*/
	virtual void CallAction(const std::string& x_name, mkjson& rx_value) = 0;

protected:
	static log4cxx::LoggerPtr m_logger;
	const std::string m_name;
};


/**
* @brief Class representing a controllable object (e.g. Module or Manager)
*/
class Controllable
{
public:
	Controllable() {}
	virtual ~Controllable()
	{
		for(auto & elem : m_controls)
			delete(elem.second);
	}
	const std::map<std::string, Controller*>& GetControllersList() const {return m_controls;}
	Controller& FindController(const std::string& x_name) const;
	/**
	* @brief Find if controller exists by name
	*
	* @param x_name
	*
	* @return true or false
	*/
	inline bool HasController(const std::string& x_name) const {return m_controls.find(x_name) != m_controls.end();}
	/**
	* @brief Add a controller
	*
	* @param xp_ctr Controller pointer
	*
	*/
	inline void AddController(Controller* xp_ctr)
	{
		// check that we are not inserting 2x the same controller
		assert(m_controls.find(xp_ctr->GetName()) == m_controls.end());
		m_controls.insert(make_pair(xp_ctr->GetName(), xp_ctr));
	}

private:
	std::map<std::string, Controller*> m_controls;
};

} // namespace mk
#endif
