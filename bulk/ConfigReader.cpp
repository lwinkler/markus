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
* @brief Save the config as a json file
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
* @brief Save the config as an json file
*
* @param x_file Name of the file with relative path
*/
void writeToFile(const ConfigReader& xr_config, const string& x_fileName)
{
	ofstream of(x_fileName);
	of << xr_config;
}

/**
* @brief Validate the configuration
*/
void validate(const ConfigReader& xr_config)
{
	// note: nothing to check yet
}

void overrideInputs(ConfigReader& xr_oldConfig, const ConfigReader& x_newConfig)
{
	for(const auto& elem : x_newConfig)
	{
		replaceOrAppendInArray(xr_oldConfig, "name", elem["name"].asString()) = elem;
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
	for(const auto& elem : x_extraConfig["modules"])
	{
		overrideInputs(findFirstInArray(xr_config["modules"], "name", elem["name"].asString())["inputs"], elem["inputs"]);
	}
}

std::string jsonToString(const Json::Value& x_json)
{
	stringstream ss;
	ss << x_json;
	return ss.str();
}

Json::Value stringToJson(const std::string& x_string)
{
	istringstream ss(x_string);
	Json::Value json;
	ss >> json;
	return json;
}

/**
* @brief Replace the first occurence or append to a config array
*
* @param xconf   Config to search
* @param x_name  Name of the field
* @param x_value Value of the field
*
* @return reference
*/
Json::Value& replaceOrAppendInArray(ConfigReader& x_conf, const std::string& x_name, const std::string& x_value)
{
	for(auto& elem : x_conf)
	{
		if(elem[x_name].asString() == x_value)
			return elem;
	}
	Json::Value root;
	root[x_name] = x_value;
	x_conf.append(root);
	return x_conf[x_conf.size() - 1];
}

/**
* @brief Find first occurence of name=value in array
*
* @param xconf   Config to search
* @param x_name  Name of the field
* @param x_value Value of the field
*
* @return reference
*/
Json::Value& findFirstInArray(ConfigReader& x_conf, const std::string& x_name, const std::string& x_value)
{
	for(auto& elem : x_conf)
	{
		if(elem[x_name].asString() == x_value)
			return elem;
	}
	throw MkException("No occurence of " + x_name + "=" + x_value + " in config", LOC);
}

/**
* @brief Find first occurence of name=value in array (constant)
*
* @param xconf   Config to search
* @param x_name  Name of the field
* @param x_value Value of the field
*
* @return reference
*/
const Json::Value& findFirstInArrayConst(const ConfigReader& x_conf, const std::string& x_name, const std::string& x_value)
{
	for(const auto& elem : x_conf)
	{
		if(elem[x_name].asString() == x_value)
			return elem;
	}
	throw MkException("No occurence of " + x_name + "=" + x_value + " in config", LOC);
}
