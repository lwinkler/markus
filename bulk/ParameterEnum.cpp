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
	auto it = m_allowedValues.find(mr_value);
	if(it != m_allowedValues.end())
		return it->second;
	else
		return m_allowAllValues;
}

/**
* @brief Print the range of possible values
*
* @return Range as a string
*/
string ParameterEnum::GetRange() const
{
	vector<string> result;
	for(const auto & elem : GetEnum())
	{
		// If a value is specified in allowed values we respect this
		// otherwise look at m_allowAllValues
		auto it2 = m_allowedValues.find(elem.second);
		if(it2 != m_allowedValues.end())
		{
			if(it2->second)
				result.push_back(elem.first);
		}
		else
		{
			if(m_allowAllValues)
				result.push_back(elem.first);
		}
	}
	return "[" + join(result, ',') + "]";
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
* @brief Set the range of acceptable values
*
* @param x_range Range in the form "[val1,val2,val3]"
*/
void ParameterEnum::SetRange(const string& x_range)
{
	vector<string> values;
	if(x_range.substr(0, 1) != "[" || x_range.substr(x_range.size() - 1, 1) != "]")
		throw MkException("Error in range " + x_range, LOC);
	split(x_range.substr(1, x_range.size() - 2), ',', values);
	// Remove last element if empty, due to an extra comma
	assert(values.size() > 0);
	if(values.back() == "")
		values.pop_back();
	assert(values.size() > 0);

	AllowAllValues(false);
	m_allowedValues.clear();
	for(const auto& elem : values)
	{
		AllowValue(elem, true);
	}
}

/**
 * @brief Generate values in range
 *
 * @param x_nbSamples Number of valuew to generate
 * @param rx_values Output
 * @param x_range      Range (if empty take parameter range)
 *
 */
void ParameterEnum::GenerateValues(int x_nbSamples, vector<string>& rx_values, const string& x_range) const
{
	string range = x_range == "" ? GetRange() : x_range;
	rx_values.clear();
	split(range.substr(1, range.size() - 2), ',', rx_values);

	// Remove last element if empty, due to an extra comma
	if(rx_values.empty())
		throw MkException("Value array is empty, range= " + range, LOC);
	if(rx_values.back() == "")
	{
		rx_values.pop_back();
		if(rx_values.empty())
			throw MkException("Value array is empty, range= " + range, LOC);
	}
}
