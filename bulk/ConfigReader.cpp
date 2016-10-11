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
#include "MkException.h"
#include "util.h"

using namespace std;


/**
* @brief Save the config as an xml file
*
* @param rx_config Result config
* @param x_fileName File name
* @param x_allowCreation Allow to create the file
*/
void readFromFile(ConfigReader& rx_config, const std::string& x_fileName, bool x_allowCreation)
{
	if(!boost::filesystem::exists(x_fileName))
	{
		if(x_allowCreation)
			createEmptyConfigFile(x_fileName, true);
		else throw MkException("File " + x_fileName + " does not exist", LOC);
	}
	ifstream ifs(x_fileName);
	ifs >> rx_config;
}

/**
* @brief Save the config as an xml file
*
* @param x_file Name of the file with relative path
*/
void saveToFile(const ConfigReader& xr_config, const string& x_fileName)
{
	ofstream of(x_fileName);
	of << xr_config;
}

/**
* @brief Validate the configuration
*/
void validate(const ConfigReader& xr_config)
{
	const ConfigReader& appConf = xr_config["application"];
	if(appConf.isNull())
		throw MkException("Configuration must contain <application> subconfiguration", LOC);
	map<int,bool> moduleIds;
	
	for(const auto& name : appConf["modules"].getMemberNames())
	{
		const auto& conf = appConf["modules"][name];
		const auto& id   = conf["id"];
		if(!id.isInt())
			throw MkException("Module " + name + " has no id", LOC);
		if(moduleIds[id.asInt()])
			throw MkException("Module with id=" + to_string(id.asInt()) + " must be unique", LOC);
		moduleIds[id.asInt()] = true;

		checkUniquenessOfId(conf, "inputs",     "id",   name);
		checkUniquenessOfId(conf, "outputs",    "id",   name);
	}
}

/**
* @brief Check that an id is unique: for validation purpose
*
* @param x_group      Group (e.g. inputs, parameters, ...)
* @param x_type       Type (e.g. input)
* @param x_idLabel    Label
* @param x_moduleName Name of the module
*/
void checkUniquenessOfId(const ConfigReader& xr_config, const string& x_group, const string& x_idLabel, const string& x_moduleName)
{
	// Check that input streams are unique
	map<int, bool> ids;
	const ConfigReader& conf(xr_config[x_group]);
	if(conf.isNull())
		return;
	for(const auto& name : xr_config.getMemberNames())
	{
		int id = conf[name][x_idLabel].asInt();
		if(ids[id])
			throw MkException("Json property " + x_group + " " + x_idLabel + "=" + to_string(id) + " must be unique for module name=" + x_moduleName, LOC);
		ids[id] = true;
	}
}



void overrideParameters(ConfigReader& x_oldConfig, const ConfigReader& x_newConfig, const string& x_parentName)
{
	if(x_newConfig.isObject() && x_newConfig.isObject())
	{
		for(const auto& name : x_newConfig.getMemberNames())
		{
			if(!x_newConfig.isMember(name))
				continue;
			if(x_parentName == "parameters")
			{
				x_oldConfig[name] = x_newConfig[name];
			}
			else
			{
				// recursive call
				overrideParameters(x_oldConfig[name], x_newConfig[name], name);
			}
		}
	}
}

/**
* @brief Apply extra JSON config to modify the initial config (used with option -x)
*
* @param xr_config     Config to override
* @param x_extraConfig Extra config to use for overriding
*
* @return
*/

void overrideWith(ConfigReader& xr_config, const ConfigReader& x_extraConfig)
{
	// Note: This method is very specific to our type of configuration
	overrideParameters(xr_config["application"]["modules"], x_extraConfig["application"]["modules"], "modules");
}
