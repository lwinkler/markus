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

#ifndef PARAMETER_STRUCTURE_H
#define PARAMETER_STRUCTURE_H

#include "define.h"
#include "ConfigReader.h"
#include "Parameter.h"
#include "MkException.h"
#include <log4cxx/logger.h>

/// Represents a set of parameters for a configurable objects
class ParameterStructure
{
public:
	ParameterStructure(const ConfigReader& x_configReader);
	~ParameterStructure();
	void Init();
	void SetFromConfig();
	void UpdateConfig() const;
	void SetValueToDefault();
	virtual void CheckRange(bool x_checkRelated) const;
	void PrintParameters() const;
	//void SetValueByName(const std::string& x_name, const std::string& x_value, ParameterConfigType x_configType = PARAMCONF_UNKNOWN);
	const Parameter & GetParameterByName(const std::string& x_name) const;
	const std::vector<Parameter*>& GetList() const {return m_list;}
	void LockParameterByName(const std::string& x_name) {RefParameterByName(x_name).Lock();}
	
protected:
	std::vector<Parameter*> m_list;
	const ConfigReader m_configReader; // Warning this still contains reference to the tinyxml config!
	std::string m_moduleName;
	bool m_writeAllParamsToConfig;
	Parameter & RefParameterByName(const std::string& x_name);

private:
	static log4cxx::LoggerPtr m_logger;
};


#endif
