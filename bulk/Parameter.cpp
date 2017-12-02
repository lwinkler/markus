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

#include "Parameter.h"
#include "util.h"
#include "json.hpp"

using namespace std;

const char Parameter::configType[PARAMCONF_SIZE][16] = {"unset", "def", "json", "gui", "cmd", "unk"};

// Static variables
log4cxx::LoggerPtr Parameter::m_logger(log4cxx::Logger::getLogger("Parameter"));


// TODO: Maybe one day avoid having a range for each instance. Link it with module.

/**
* @brief Export the parameter for module description
*
* @param rx_os         Output stream
* @param x_indentation Number of tabs for indentation
*/
void Parameter::Export(ostream& rx_os) const
{
	mkjson json;
	json["type"] = GetType();
	json["name"] = GetName();
	json["description"] = GetDescription();
	json["default"] = GetDefault();
	json["range"] = GetRange();
	rx_os << multiLine(json);
}


/**
* @brief Print the description of the parameter for logging
*
* @param rx_os         Output stream
*/
void Parameter::Print(std::ostream& os) const
{
	// note: remove line return to shorten json objects
	os<<m_name<<"="<< oneLine(GetValue()) << " ("<<configType[m_confSource]<<"); ";
}

