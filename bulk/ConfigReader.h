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
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>
#include <boost/noncopyable.hpp>

// #include "define.h"
// #include <log4cxx/xml/domconfigurator.h>


class Module;
class ParameterStructure;

typedef Json::Value ConfigReader;


/**
* @brief       Class used to manipulate configuration files
* @brief       A ConfigReader object is a reference to a sub part of a ConfigFile.
*/
// class ConfigReader : public Json::Value, boost::noncopyable
// {
// public:
	// ConfigReader(){}
	// ConfigReader(const Json::Value& xr_jsonRoot);
	// ConfigReader(Json::Value& xr_jsonRoot);
	/*
	ConfigReader(const ConfigReader& x_conf);
	virtual ~ConfigReader();

	// Method to access elements of the config
	const ConfigReader GetSubConfig(const std::string& x_tagName) const;
	const ConfigReader GetSubConfig(const std::string& x_attrName, const std::string& x_attrValue) const;
	ConfigReader RefSubConfig(const std::string& x_tagName, bool x_allowCreation = false);
	ConfigReader RefSubConfig(const std::string& x_attrName, const std::string& x_attrValue, bool x_allowCreation = false);
	ConfigReader Append(const std::string& x_tagName);

	/// Check if the config object is empty
	bool IsEmpty() const;
	std::string GetValue() const;
	template<typename T> inline void SetValue(const T& x_value)
	{
		// TODO x_value >> *mp_jsonRoot;
	}
	const std::string GetAttribute(const std::string& x_attributeName) const;
	const std::string GetAttribute(const std::string& x_attributeName, const std::string& x_default) const;
	template<typename T> inline void SetAttribute(const std::string& x_attributeName, const T& x_value)
	{
		std::stringstream ss;
		ss << x_value;
		SetAttribute(x_attributeName, ss.str());
	}
	void Validate() const;
	/// Redefinition of == operator
	bool operator == (const ConfigReader &a);
	*/
	void readFromFile(ConfigReader& rx_config, const std::string& x_fileName, bool x_allowCreation = false);
	void overrideWith(ConfigReader& rx_config, const ConfigReader& xr_extraConfig);

	// New access functions with JQuery-like syntax
	// const ConfigReader& Find(const std::string& x_searchString) const;
	// ConfigReader& FindRef(const std::string& x_searchString, bool x_allowCreation = false);
	// std::vector<ConfigReader> FindAll(const std::string& x_searchString) const;
	void saveToFile(const ConfigReader& rx_config, const std::string& x_file);
	void validate(const ConfigReader& rx_config);
	/*

protected:
	ConfigReader(Json::Value* xp_node);
	ConfigReader& operator = (const ConfigReader& x_conf);
	// ConfigReader NextSubConfig(const std::string& x_tagName, const std::string& x_attrName = "", const std::string& x_attrValue = "") const;
	*/
	void checkUniquenessOfId(const ConfigReader& rx_config, const std::string& x_group, const std::string& x_type, const std::string& x_idLabel, const std::string& x_moduleName);
	/*
	static void overrideParameters(const ConfigReader& x_newConfig, ConfigReader x_oldConfig);

	Json::Value* mp_jsonRoot = nullptr;
	*/

// private:
	// static log4cxx::LoggerPtr m_logger;
// };

/**
* @brief       Class used to manipulate configuration files
* @brief       A ConfigFile object is created from an XML file. It can browse the different tags of the file by creating sub config objects. Each sub config object is
               a reference to the sub configuration. Not a copy.
*/
/*
class ConfigFile : public ConfigReader, boost::noncopyable
{
public:
	ConfigFile(const std::string& x_fileName, bool x_allowCreation = false, bool x_header = true);
	virtual ~ConfigFile();
	void SaveToFile(const std::string& x_file) const;

private:
	Json::Value* mp_jsonRoot = nullptr;
};
*/

/**
* @brief       Class used to manipulate configuration files
* @brief       A ConfigFile object is created from an XML file. It can browse the different tags of the file by creating sub config objects. Each sub config object is
               a reference to the sub configuration. Not a copy.
               This utility relies on TinyXml.
*/
/*
class ConfigString : public ConfigReader, boost::noncopyable
{
public:
	ConfigString(const std::string& x_string);
	virtual ~ConfigString();

private:
	Json::Value* mp_jsonRoot = nullptr;
};
*/
#endif
