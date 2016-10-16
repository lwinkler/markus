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
#include <boost/noncopyable.hpp>


class Module;
class ParameterStructure;

typedef Json::Value ConfigReader;

void readFromFile(ConfigReader& rx_config, const std::string& x_fileName, bool x_allowCreation = false);
void overrideWith(ConfigReader& rx_config, const ConfigReader& xr_extraConfig);
void writeToFile(const ConfigReader& rx_config, const std::string& x_file);
void validate(const ConfigReader& rx_config);
void checkUniquenessOfId(const ConfigReader& rx_config, const std::string& x_group, const std::string& x_idLabel, const std::string& x_moduleName);
std::string jsonToString(const Json::Value& x_json);
Json::Value stringToJson(const std::string& x_string);

#endif
