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
#include <json.hpp>


class Module;
class ParameterStructure;

typedef nlohmann::json ConfigReader;

void readFromFile(ConfigReader& rx_config, const std::string& x_fileName, bool x_allowCreation = false);
void overrideWith(ConfigReader& rx_config, const ConfigReader& xr_extraConfig);
void writeToFile(const ConfigReader& rx_config, const std::string& x_file);
void validate(const ConfigReader& rx_config);
std::string jsonToString(const ConfigReader& x_json);
ConfigReader stringToJson(const std::string& x_string);
ConfigReader& findFirstInArray(ConfigReader& x_conf, const std::string& x_name, const std::string& x_value);
ConfigReader& replaceOrAppendInArray(ConfigReader& x_conf, const std::string& x_name, const std::string& x_value);
const ConfigReader& findFirstInArrayConst(const ConfigReader& x_conf, const std::string& x_name, const std::string& x_value);

#endif
