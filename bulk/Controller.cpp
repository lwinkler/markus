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

#include "Controller.h"

using namespace std;


Controller::Controller(const string& x_name, const string& x_type) :
	m_name(x_name),
	m_type(x_type)
{
}


Controller::~Controller()
{
	// if(m_widget != NULL) delete m_widget; // no need to delete
}

/// find a controller in map by name
const void Controller::CallAction(const string& x_name, string* xp_value)
{
	map<string, const px_action>::const_iterator it = m_actions.find(x_name);
	if(it == m_actions.end())
		throw MkException("Cannot find action in controller", LOC);

	// Call the function pointer associated with the action
	// cout<<(it->first)<<endl;
	(*(it->second))(this, xp_value);
}


void Controller::ListActions(vector<string>& xr_actions) const
{
	for(map<string, const px_action>::const_iterator it = m_actions.begin() ; it != m_actions.end() ; it++)
		xr_actions.push_back(it->first);
}

/// find a controller in map by name
Controller* Controllable::FindController(const std::string& x_name) const
{
	map<string, Controller*>::const_iterator it = m_controls.find(x_name);
	if(it == m_controls.end())
		throw MkException("Cannot find controller " + x_name + " in controls list", LOC);

	// Call the function pointer associated with the action
	return it->second;
}


