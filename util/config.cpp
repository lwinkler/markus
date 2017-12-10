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

#include <fstream>
#include <boost/filesystem.hpp>
#include "config.h"
#include "MkException.h"
#include "util.h"

namespace mk {
using namespace std;


/**
* @brief Save the config as a json file
*
* @param rx_config Result config
* @param x_fileName File name
* @param x_allowCreation Allow to create the file
*/
void readFromFile(mkconf& rx_config, const std::string& x_fileName, bool x_allowCreation)
{
	if(!boost::filesystem::exists(x_fileName))
	{
		if(x_allowCreation)
			createEmptyConfigFile(x_fileName);
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
void writeToFile(const mkconf& xr_config, const string& x_fileName)
{
	ofstream of(x_fileName);
	of << xr_config;
}

void overrideInputs(mkconf& xr_oldConfig, const mkconf& x_newConfig)
{
	for(const auto& elem : x_newConfig)
	{
		replaceOrAppendInArray(xr_oldConfig, "name", elem.at("name").get<string>()) = elem;
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

void overrideWith(mkconf& xr_config, const mkconf& x_extraConfig)
{
	for(const auto& elem : x_extraConfig.at("modules"))
	{
		overrideInputs(findFirstInArray(xr_config.at("modules"), "name", elem.at("name").get<string>())["inputs"], elem["inputs"]);
	}
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
mkjson& replaceOrAppendInArray(mkconf& x_conf, const std::string& x_name, const std::string& x_value)
{
	for(auto& elem : x_conf)
	{
		if(elem[x_name].get<string>() == x_value)
			return elem;
	}
	mkjson root;
	root[x_name] = x_value;
	x_conf.push_back(root);
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
mkjson& findFirstInArray(mkconf& x_conf, const std::string& x_name, const std::string& x_value)
{
	for(auto& elem : x_conf)
	{
		if(elem.at(x_name).get<string>() == x_value)
			return elem;
	}
	throw MkException("No occurence of " + x_name + "=" + x_value + " in config", LOC);
}

/**
* @brief Find first occurence of name=value in array (constant)
*
* @param xconf   Config to search
* @paam x_name  Name of the field
* @param x_value Value of the field
*
* @return reference
*/
const mkjson& findFirstInArrayConst(const mkconf& x_conf, const std::string& x_name, const std::string& x_value)
{
	for(const auto& elem : x_conf)
	{
		if(elem.at(x_name).get<string>() == x_value)
			return elem;
	}
	throw MkException("No occurence of " + x_name + "=" + x_value + " in config", LOC);
}
}
