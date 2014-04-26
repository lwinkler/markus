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

#include <string>
#include <vector>
#include <assert.h>

#include "Parameter.h"

#define PSTR2INT(pstr)      atoi((pstr)->c_str())
#define PSTR2FLOAT(pstr)    atof((pstr)->c_str())
#define INT2PSTR(x, pstr)   { stringstream ss; ss<<x; *(pstr) = ss.str();}
#define FLOAT2PSTR(x, pstr) { stringstream ss; ss<<x; *(pstr) = ss.str();}

class Module;
class QWidget;

// Use macros to declare members of Controller childs, not the cleanest thing but you are free to try to 
// improve on this
#define DECLARE_CALL_ACTION(action) \
const void CallAction(const std::string& x_name, std::string* xp_value)\
{\
	std::map<std::string, const action>::const_iterator it = m_actions.find(x_name);\
	if(it == m_actions.end())\
		throw MkException("Cannot find action in controller", LOC);\
	(this->*(it->second))(xp_value);\
}
#define DECLARE_LIST_ACTION(action) \
void ListActions(std::vector<std::string>& xr_actions) const\
{\
	for(std::map<std::string, const action>::const_iterator it = m_actions.begin() ; it != m_actions.end() ; it++)\
		xr_actions.push_back(it->first);\
}

/**
* @brief A class to send commands to a controllable object (module, parameter, ...)
*/
class Controller
{
public:
	Controller(const std::string& x_name, const std::string& x_type);
	virtual ~Controller();

	virtual QWidget* CreateWidget() = 0;
	const std::string& GetName() {return m_name;}
	const std::string& GetType(){return m_type;}  // TODO: Type must be set in sub class !
	virtual void ListActions(std::vector<std::string>& xr_actions) const = 0;
	virtual const void CallAction(const std::string& x_name, std::string* xp_value) = 0;
	
protected:
	std::string m_name;
	std::string m_type;
};


/**
* @brief Class representing a controllable object (e.g. Module or Manager)
*/
class Controllable
{
	public:
		Controllable(){}
		~Controllable()
		{
			for(std::map<std::string, Controller* >::iterator it = m_controls.begin() ; it != m_controls.end() ; it++)
				delete(it->second);
		}
		const std::map<std::string, Controller*>& GetControllersList() const {return m_controls;}
		Controller* FindController(const std::string& x_name) const;
		inline void AddController(Controller* xp_ctr)
		{
			// check that we are not inserting 2x the same controller
			assert(m_controls.find(xp_ctr->GetName()) == m_controls.end());
			m_controls.insert(make_pair(xp_ctr->GetName(), xp_ctr));
		}
		// inline bool HasNoControllers()const{return m_controls.size() == 0;} // TODO: Check otherwise

	private:
		std::map<std::string, Controller*> m_controls;
};
	
#endif
