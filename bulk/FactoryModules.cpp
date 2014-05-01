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

#include "FactoryModules.h"
#include "AllModules.h"

using namespace std;

FactoryModules::FactoryModules()
{
	registerAllModules(*this);	
}

/**
* @brief Create a module of given type
*
* @param x_type   Type of the module as string
* @param x_config Relative path and name of the configuration file of the module
*
* @return 
*/
Module * FactoryModules::CreateModule(const std::string& x_type, const ConfigReader& x_config)
{
	ModuleRegistry::iterator it = m_register.find(x_type);

	if (it == m_register.end())
	{
		throw MkException("Cannot find a constructor for module of type " + x_type, LOC);
	}

	CreateModuleFunc func = it->second;
	Module* pmod = func(x_config);
	assert(pmod->GetClass() == x_type);
	return pmod;
}


/**
* @brief List all modules
*
* @param xr_types Output: The list of modules is added to this vector
*/
void FactoryModules::ListModules(vector<string>& xr_types) const
{
	for(ModuleRegistry::const_iterator it = m_register.begin() ; it != m_register.end() ; it++)
		xr_types.push_back(it->first);
}
