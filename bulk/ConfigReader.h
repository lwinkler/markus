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
	ConfigReader(const std::string& x_fileName)
	{
		//m_fileName = x_fileName;
		//m_parameterList.clear();
		mp_doc = new TiXmlDocument(x_fileName);
		if (! mp_doc->LoadFile())
			throw("Could not load test file '" + x_fileName + "'. Error='" + mp_doc->ErrorDesc() + "'. Exiting.");
		mp_node = mp_doc;//mp_doc->FirstChild( "application" );
	};
	ConfigReader(TiXmlNode * xp_node)
	{
		mp_doc = NULL;
		mp_node = xp_node;
	};
	~ConfigReader()
	{
		delete mp_doc;
	};
	ConfigReader SubConfig(const std::string& x_objectType, std::string x_objectName = "") const;
	ConfigReader NextSubConfig(const std::string& x_objectType, std::string x_objectName = "") const;
	bool IsEmpty(){ return mp_doc == NULL && mp_node == NULL;};
	std::vector<ParameterValue> ReadParameters(const std::string& rx_type, const std::string& rx_name = "") const;
	/*std::vector<ParameterValue> ReadParameters(const std::string& x_type, const std::string& x_moduleName, bool x_getClassOnly = false) const;
	std::vector<ParameterValue> ReadModules(const std::string& x_vectorType, const std::string& x_type, int x_objectNumber, bool x_getClassOnly = false) const;
	int ReadConfigGetVectorSize(const std::string& x_vectorType, const std::string& x_objectType) const;
	*/
	static ParameterValue GetParameterValue(const std::string& x_name, const std::vector<ParameterValue> & x_parameterList);
	const std::string GetAttribute(const std::string& x_attributeName) const;
private:
	//std::string m_fileName;
	TiXmlNode * mp_node;
	TiXmlDocument * mp_doc;
};

class Configurable
{
public:
	Configurable(const ConfigReader& x_confReader) : m_configReader(x_confReader){};
	~Configurable(){};
protected:
	void ReadParametersFromConfig();
	const ConfigReader m_configReader;
	inline virtual const ParameterStructure & GetRefParameter() const = 0;
};

#endif