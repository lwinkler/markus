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

#include <string>
#include <vector>
#include <sstream>
#include <boost/noncopyable.hpp>

#include "define.h"
#include <log4cxx/xml/domconfigurator.h>


class Module;
class ParameterStructure;
class TiXmlNode;
class TiXmlDocument;


/**
* @brief       Class used to manipulate configuration files
* @brief       A ConfigReader object is a reference to a sub part of a ConfigFile.
               This utility relies on TinyXml.
*/
class ConfigReader
{
public:
	ConfigReader(const ConfigReader& x_conf);
	virtual ~ConfigReader();

	// Method to access elements of the config
	const ConfigReader GetSubConfig(const std::string& x_tagName) const;
	const ConfigReader GetSubConfigIgnoreNamespace(const std::string& x_tagName) const;
	const ConfigReader GetSubConfig(const std::string& x_tagName, const std::string& x_attrName, const std::string& x_attrValue) const;
	ConfigReader RefSubConfig(const std::string& x_tagName, bool x_allowCreation = false);
	ConfigReader RefSubConfig(const std::string& x_tagName, const std::string& x_attrName, const std::string& x_attrValue, bool x_allowCreation = false);
	ConfigReader Append(const std::string& x_tagName);

	/// Check if the config object is empty
	inline bool IsEmpty() const {return mp_node == nullptr;}
	std::string GetValue() const;
	void SetValue(const std::string& x_value);
	template<typename T> inline void SetValue(const T& x_value)
	{
		std::stringstream ss;
		ss << x_value;
		SetValue(ss.str());
	}
	const std::string GetAttribute(const std::string& x_attributeName) const;
	const std::string GetAttribute(const std::string& x_attributeName, const std::string& x_default) const;
	void SetAttribute(const std::string& x_attributeName, const std::string& x_value);
	template<typename T> inline void SetAttribute(const std::string& x_attributeName, const T& x_value)
	{
		std::stringstream ss;
		ss << x_value;
		SetAttribute(x_attributeName, ss.str());
	}
	void Validate() const;
	/// Redefinition of == operator
	inline bool operator == (const ConfigReader &a) {return a.mp_node == mp_node;}
	void OverrideWith(const ConfigReader& xr_extraConfig);

	// New access functions with JQuery-like syntax
	const ConfigReader Find(const std::string& x_searchString) const;
	ConfigReader    FindRef(const std::string& x_searchString, bool x_allowCreation = false);
	std::vector<ConfigReader> FindAll(const std::string& x_searchString) const;
	bool IsFinal() const;

protected:
	ConfigReader(TiXmlNode* xp_node);
	ConfigReader& operator = (const ConfigReader& x_conf);
	ConfigReader NextSubConfig(const std::string& x_tagName, const std::string& x_attrName = "", const std::string& x_attrValue = "") const;
	void CheckUniquenessOfId(const std::string& x_group, const std::string& x_type, const std::string& x_idLabel, const std::string& x_moduleName) const;
	static void overrideParameters(const ConfigReader& x_newConfig, ConfigReader x_oldConfig);

	TiXmlNode* mp_node;

private:
	static log4cxx::LoggerPtr m_logger;
};

/**
* @brief       Class used to manipulate configuration files
* @brief       A ConfigFile object is created from an XML file. It can browse the different tags of the file by creating sub config objects. Each sub config object is
               a reference to the sub configuration. Not a copy.
               This utility relies on TinyXml.
*/
class ConfigFile : public ConfigReader, boost::noncopyable
{
public:
	ConfigFile(const std::string& x_fileName, bool x_allowCreation = false, bool x_header = true);
	virtual ~ConfigFile();
	void SaveToFile(const std::string& x_file) const;

private:
	TiXmlDocument* mp_doc;
};

/**
* @brief       Class used to manipulate configuration files
* @brief       A ConfigFile object is created from an XML file. It can browse the different tags of the file by creating sub config objects. Each sub config object is
               a reference to the sub configuration. Not a copy.
               This utility relies on TinyXml.
*/
class ConfigString : public ConfigReader, boost::noncopyable
{
public:
	ConfigString(const std::string& x_string);
	virtual ~ConfigString();

private:
	TiXmlDocument* mp_doc;
};


/**
* @brief Parent class for all configurable objects (containing parameters)
*/
class Configurable : boost::noncopyable
{
public:
	Configurable(ParameterStructure& x_param);
	virtual ~Configurable() {}
	virtual void WriteConfig(ConfigReader xr_config) const;
	virtual const ParameterStructure& GetParameters() const {return m_param;}

private:
	ParameterStructure& m_param;
};

#endif
