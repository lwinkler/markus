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
	virtual ~ParameterStructure();
	inline const ConfigReader& GetConfig() const {return m_configReader;}
	inline ConfigReader& RefConfig() {return m_configReader;}
	void Initialize();
	void SetFromConfig();
	void UpdateConfig() const;
	void SetValueToDefault();
	virtual void CheckRange(bool x_checkRelated) const;
	void PrintParameters() const;
	//void SetValueByName(const std::string& x_name, const std::string& x_value, ParameterConfigType x_configType = PARAMCONF_UNKNOWN);
	const Parameter & GetParameterByName(const std::string& x_name) const;
	const std::vector<Parameter*>& GetList() const {return m_list;}
	void LockParameterByName(const std::string& x_name) {RefParameterByName(x_name).Lock();}
	void LockIfRequired();
	bool ParameterExists(const std::string& x_name) const;
	void AddParameter(Parameter* xr_param);
	void AddParameterForStream(Parameter* xr_param);

protected:
	Parameter & RefParameterByName(const std::string& x_name);
	bool m_writeAllParamsToConfig;

private:
	std::vector<Parameter*> m_list;
	std::string m_moduleName;

	ConfigReader m_configReader; // Warning this still contains reference to the tinyxml config! // TODO: maybe avoid storing this
	static log4cxx::LoggerPtr m_logger;
	// Note: Disable copies of parameters as a safety
	DISABLE_COPY(ParameterStructure)
};


#endif
