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

#ifndef CONFIGREADER_H
#define CONFIGREADER_H

#include <tinyxml.h>
#include <string>
#include "Parameter.h"
#include <vector>


class Module;
//class Manager;std::vector<ParameterValue> 

class ConfigReader
{
public:
	ConfigReader(const char * x_fileName)
	{
		m_fileName = x_fileName;
		//m_parameterList.clear();
	};
	~ConfigReader(){};
	std::vector<ParameterValue> ReadConfigObject(const std::string& x_type, const std::string& x_moduleName);
	std::vector<ParameterValue> ReadConfigObjectFromVect(const std::string& x_vectorType, const std::string& x_type, int x_objectNumber);
	int ReadConfigGetVectorSize(const std::string& x_vectorType, const std::string& x_objectType);
	static ParameterValue GetParameterValue(const std::string& x_name, const std::vector<ParameterValue> & x_parameterList);
	//void SetParameterValues(ParameterStructure& x_param);
private:
	//std::vector<ParameterValue> m_parameterList;
	std::string m_fileName;
};

class Configurable
{
public:
	Configurable(ConfigReader& x_confReader) : m_configReader(x_confReader){};
	~Configurable(){};
protected:
	void ReadParametersFromConfig();
	ConfigReader& m_configReader;
	virtual const ParameterStructure & GetRefParameter() const = 0;
};

#endif