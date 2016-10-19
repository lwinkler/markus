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

#include "ParameterEnum.h"
#include "util.h"

using namespace std;

// Static variables
log4cxx::LoggerPtr ParameterEnum::m_logger(log4cxx::Logger::getLogger("ParameterEnum"));

/**
* @brief Set the value of the parameter
*
* @param rx_value   String containing the value
* @param x_confType Source of the configuration (default, xml, GUI, ...)
*/
void ParameterEnum::SetValue(const ConfigReader& rx_value, ParameterConfigType x_confType)
{
	if(IsLocked())
		throw ParameterException("You tried to set the value of a locked parameter.", LOC);
	mr_value = GetEnum().at(rx_value.asString());
	m_confSource = x_confType;
}

/**
* @brief Set the default value
*
* @param rx_value Value to set
*/
void ParameterEnum::SetDefault(const ConfigReader& rx_value)
{
	m_default = GetEnum().at(rx_value.asString());
}

/**
* @brief Check that the parameter value is in range
*
* @return true if valid
*/
bool ParameterEnum::CheckRange() const
{
	if(!m_range.isMember("allowed"))
		return true;

	for(const auto& elem : m_range["allowed"])
	{
		if(elem.asString() == GetReverseEnum().at(mr_value))
			return true;
	}
	return false;
}

/**
* @brief Print the parameter with default value and range
*
* @param os Output stream
*/
void ParameterEnum::Print(ostream& os) const
{
	try
	{
		os<<GetName()<<"="<<GetReverseEnum().at(mr_value)<<" ("<<mr_value<<") ("<<configType[m_confSource]<<"); ";
	}
	catch(exception &e)
	{
		LOG_WARN(m_logger, "Exception while printing " << GetName() << " with value " << mr_value);
		throw MkException("Exception with parameter " + GetName() + ": " + string(e.what()), LOC);
	}
}

/**
* @brief Create a reverse map. Used internally
*
* @return map
*/
map<int,string> ParameterEnum::CreateReverseMap(const map<string, int>& x_map)
{
	map<int, string> out;
	for(const auto& elem : x_map)
	{
		auto it = out.find(elem.second);
		if(it == out.end())
			out.insert(make_pair(elem.second, elem.first));
		else
			throw MkException("ParameterEnum contains 2 entries with the same value " + elem.first + " and " + it->second, LOC);
	}
	return out;
}

/**
 * @brief Generate values in range
 *
 * @param x_nbSamples Number of valuew to generate
 * @param x_range      Range (if empty take parameter range)
 *
 */
Json::Value ParameterEnum::GenerateValues(int x_nbSamples, const Json::Value& x_range) const
{
	if(!x_range.isMember("allowed"))
		return x_range["allowed"];
	if(!x_range.isMember("advised"))
		return x_range["advised"];
	Json::Value root = Json::arrayValue;
	for(const auto& elem : GetEnum())
		root.append(elem.first);
	return root;
}
