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

void Parameter::SetValue(const ConfigReader& x_value, ParameterConfigType x_confType){
	std::stringstream ss;
	ss << x_value;
	SetValue(ss.str(), x_confType);
}


void Parameter::SetRange(const mkjson& x_range)
{
	if(x_range.is_string())
	{
		// For legacy and convenience, we accept [min:max] and [1,2,3] syntaxes
		mkjson root;
		double min, max;
		const string& range(x_range.get<string>());
		if(range.empty())
			throw MkException("Empty range", LOC);
		if(2 == sscanf(range.c_str(), "[%16lf:%16lf]", &min, &max))
		{
			if(GetType() == "bool")
			{
				if(min == 1)
					root["min"] = true;
				if(max == 0)
					root["max"] = false;
			}
			else
			{
				root["min"] = min;
				root["max"] = max;
			}
		}
		else if(range.at(0) == '[' && range.back() == ']')
		{
			vector<string> values;
			split(range.substr(1, range.size() - 2), ',', values);
			// Remove last element if empty, due to an extra comma
			if(values.size() > 0)
			{
				if(values.back() == "")
					values.pop_back();
			}

			for(const auto& elem : values)
			{
				root["allowed"].push_back(elem);
			}
		}
		else root = mkjson(range);
		m_range = root;
	}
	else
	{
		m_range = x_range;
	}
}
