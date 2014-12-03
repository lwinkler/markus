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
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public License
*    along with Markus.  If not, see <http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------------------*/

#ifndef FACTORY_MODULES_H
#define FACTORY_MODULES_H

#include "MkException.h"
#include "Module.h"

#include <map>
#include <string>



/// This class is a factory for modules: it creates a module of each type as specified by a string
class FactoryModules
{
	typedef Module* (*CreateModuleFunc)(const ConfigReader& x_config);
	typedef std::map<std::string, CreateModuleFunc> ModuleRegistry;
	template<class T> static Module* createModule(const ConfigReader& x_config) {return new T(x_config);}

public:
	FactoryModules();
	template<class T> void RegisterModule(const std::string& name)
	{
		CreateModuleFunc func = createModule<T>;
		m_register.insert(ModuleRegistry::value_type(name, func));
	}
	Module * CreateModule(const std::string& x_type, const ConfigReader& x_config) const;
	void RegisterAllModules();
	void ListModules(std::vector<std::string>& xr_types) const;

protected:
	ModuleRegistry m_register;
};
#endif
