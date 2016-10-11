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

#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>
#include <fstream>
#include <boost/filesystem.hpp>
#include "ConfigReader.h"
#include "ParameterStructure.h"
#include "util.h"

using namespace std;


//TODO rewrite this class as a simple wrapper (inherited from Json::Value) and add
// - Find methods
// - Overwrite methods

// log4cxx::LoggerPtr ConfigReader::m_logger(log4cxx::Logger::getLogger("ConfigReader"));


/**
* @brief Save the config as an xml file
*
* @param rx_config Result config
* @param x_fileName File name
* @param x_allowCreation Allow to create the file
*/
void readFromFile(ConfigReader& rx_config, const std::string& x_fileName, bool x_allowCreation)
{
	assert(false); // TODO
}

/**
* @brief Save the config as an xml file
*
* @param x_file Name of the file with relative path
*/
void saveToFile(const ConfigReader& xr_config, const string& x_file)
{
	// if(!mp_doc->SaveFile(x_file))
		// throw MkException("Error saving to file " + x_file, LOC);
}

/**
* @brief Validate the configuration
*/
void validate(const ConfigReader& xr_config)
{
	/*
	ConfigReader appConf = GetSubConfig("application");
	if(appConf.IsEmpty())
		throw MkException("Configuration must contain <application> subconfiguration", LOC);
	map<string,bool> moduleIds;
	map<string,bool> moduleNames;
	ConfigReader conf = appConf.GetSubConfig("module");

	while(!conf.IsEmpty())
	{
		string id = conf.GetAttribute("id", "");
		string name = conf.GetAttribute("name", "");
		if(id == "")
			throw MkException("Module " + name + " has no id", LOC);
		if(name == "")
			throw MkException("Module " + id + " has no name", LOC);
		if(moduleIds[id])
			throw MkException("Module with id=" + id + " must be unique", LOC);
		moduleIds[id] = true;
		if(moduleNames[name])
			throw MkException("Module with name=" + name + " must be unique", LOC);
		moduleNames[name] = true;

		conf.CheckUniquenessOfId("inputs",     "input",  "id",   name);
		conf.CheckUniquenessOfId("outputs",    "output", "id",   name);
		conf.CheckUniquenessOfId("parameters", "param",  "name", name);
		conf = conf.NextSubConfig("module");
	}
	*/
}

/**
* @brief Check that an id is unique: for validation purpose
*
* @param x_group      Group (e.g. inputs, parameters, ...)
* @param x_type       Type (e.g. input)
* @param x_idLabel    Label
* @param x_moduleName Name of the module
*/
void checkUniquenessOfId(const ConfigReader& xr_config, const string& x_group, const string& x_type, const string& x_idLabel, const string& x_moduleName)
{
	// Check that input streams are unique
	/*
	map<string,bool> ids;
	ConfigReader conf = GetSubConfig(x_group);
	if(conf.IsEmpty())
		return;
	conf = conf.GetSubConfig(x_type);
	while(!conf.IsEmpty())
	{
		string id = conf.GetAttribute(x_idLabel);
		if(ids[id])
			throw MkException(x_type + " with " + x_idLabel + "=" + id + " must be unique for module name=" + x_moduleName, LOC);
		ids[id] = true;
		conf = conf.NextSubConfig(x_type);
	}
	*/
}



	/*
void ConfigReader::overrideParameters(const ConfigReader& x_newConfig, ConfigReader x_oldConfig)
{
	// if(x_newConfig.GetSubConfig("parameters").IsEmpty())
	// return;
	for(const auto& conf2 : x_newConfig.GetSubConfig("parameters").FindAll("param"))
	{
		try
		{
			// cout << x_newConfig.GetAttribute("name") << ":" << conf2.GetAttribute("name") << endl;
			if(x_oldConfig.IsEmpty())
			{
				LOG_WARN(m_logger, "Module " << x_newConfig.GetAttribute("name") << " cannot be overridden since it does not exist in the current config");
				continue;
			}
			// Override parameter
			LOG_DEBUG(m_logger, "Override parameter " << conf2.GetAttribute("name") << " with value " << conf2.GetValue());
			x_oldConfig.RefSubConfig("parameters").RefSubConfig("param", "name", conf2.GetAttribute("name"), true)
			.SetValue(conf2.GetValue());
		}
		catch(MkException& e)
		{
			LOG_WARN(m_logger, "Cannot read parameters from extra config: "<<e.what());
		}
	}
}
	*/

/**
* @brief Apply extra XML config to modify the initial config (used with option -x)
*
* @param x_extraConfig Extra config to use for overriding
*
* @return
*/

void overrideWith(ConfigReader& xr_config, const ConfigReader& x_extraConfig)
{
	/*
	// Note: This method is very specific to our type of configuration
	overrideParameters(x_extraConfig.GetSubConfig("application"), RefSubConfig("application"));

	for(const auto& conf1 : x_extraConfig.GetSubConfig("application").FindAll("module"))
	{
		overrideParameters(conf1, RefSubConfig("application").RefSubConfig("module", "name", conf1.GetAttribute("name")));
	}
	*/
}

/**
* @brief Find a sub config (with a similar syntax as JQuery)
*
* @param  x_searchString The search path with jquery-like syntax
* @return value
*/
/*
const ConfigReader ConfigReader::Find(const string& x_searchString)
{
	// If empty return node: for recurrent function
	if(x_searchString.empty())
		return *this;

	string tagName, attrName, attrValue, searchString2;
	splitTagName(x_searchString, tagName, attrName, attrValue, searchString2);

	// cout << "Search in config: " << tagName << "[" << attrName << "=\"" << attrValue << "\"]" << endl;

	if(attrName == "")
		return GetSubConfig(tagName).Find(searchString2);
	else
		return GetSubConfig(tagName, attrName, attrValue).Find(searchString2);
}
*/

/**
* @brief Find a sub config (with a similar syntax as JQuery)
*
* @param  x_searchString The search path with jquery-like syntax
* @return value
*/
/*
ConfigReader ConfigReader::FindRef(const string& x_searchString, bool x_allowCreation)
{
	if(x_searchString.empty())
		return *this;

	string tagName, attrName, attrValue, searchString2;
	splitTagName(x_searchString, tagName, attrName, attrValue, searchString2);

	if(attrName == "")
		return RefSubConfig(tagName, x_allowCreation).FindRef(searchString2, x_allowCreation);
	else
		return RefSubConfig(tagName, attrName, attrValue, x_allowCreation).FindRef(searchString2, x_allowCreation);
}
*/


/**
* @brief Find all sub configs (with a similar syntax as JQuery)
*
* @param  x_searchString The search path with jquery-like syntax
* @return A vector of configurations
*/
/*
vector<ConfigReader> ConfigReader::FindAll(const string& x_searchString) const
{
	vector<ConfigReader> results;

	if(IsEmpty())
	{
		return results;
	}
	// If empty return node: for recurrent function
	if(x_searchString.empty())
	{
		results.push_back(*this);
		return results;
	}

	string tagName, attrName, attrValue, searchString2;
	splitTagName(x_searchString, tagName, attrName, attrValue, searchString2);

	if(searchString2 == "")
	{
		Json::Value arr = (*mp_jsonRoot); // TODO tagName is useless [tagName];
		if(!arr.isArray())
			throw MkException("Expecting array in find all for tag " + tagName, LOC);
		while(!conf.IsEmpty())
		for(auto& elem : arr)
		{
			if(attrName == "" || (elem.isMember(attrName) && elem[attrName] == attrValue))
				return ConfigReader(&elem);
			results.push_back(conf);
		}
		return results;
	}
	else if(attrName == "")
		return GetSubConfig(tagName).FindAll(searchString2);
	else
		return GetSubConfig(tagName, attrName, attrValue).FindAll(searchString2);
}
*/
