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
#include <vector>


class Module;
class ParameterStructure;

/// Class to read a config file

class ConfigReader
{
public:
	ConfigReader(const std::string& x_fileName);
	ConfigReader(TiXmlNode * xp_node);
	~ConfigReader();
	ConfigReader GetSubConfig(const std::string& x_objectType, std::string x_objectName = "") const;
	ConfigReader RefSubConfig(const std::string& x_objectType, std::string x_objectName = "", bool x_allowCreation = false);
	ConfigReader NextSubConfig(const std::string& x_objectType, std::string x_objectName = "") const;
	inline bool IsEmpty() const{return mp_doc == NULL && mp_node == NULL;}
	const std::string GetValue() const;
	void SetValue(const std::string& x_value);
	const std::string GetAttribute(const std::string& x_attributeName) const;
	void SaveToFile(const std::string& x_file) const;
	void Validate() const;
private:
	void CheckUniquenessOfId(const std::string& x_group, const std::string& x_type, const std::string& x_idLabel, const std::string& x_moduleName) const;
	TiXmlNode * mp_node;
	TiXmlDocument * mp_doc;
};

/// Class for a configurable object (containing parameters)

class Configurable
{
public:
	Configurable(const ConfigReader& x_confReader) : m_configReader(x_confReader){}
	~Configurable(){}
protected:
	const ConfigReader m_configReader;
	inline virtual const ParameterStructure & RefParameter() = 0;
};

#endif
