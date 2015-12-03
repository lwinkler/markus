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

#include "ParameterString.h"
#include "util.h"

using namespace std;

// Static variables 
log4cxx::LoggerPtr ParameterString::m_logger(log4cxx::Logger::getLogger("ParameterString"));

void ParameterString::Export(ostream& rx_os, int x_tabs) const
{
	string tabs(x_tabs, '\t');
	rx_os<<tabs<<"<param name=\""<<m_name<<"\">"<<endl;
	tabs = string(x_tabs + 1, '\t');
	rx_os<<tabs<<"<type>"<<GetTypeString()<<"</type>"<<endl;
	rx_os<<tabs<<"<value default=\""<<m_default<<"\">"<<GetValue()<<"</value>"<<endl;
	rx_os<<tabs<<"<description>"<<m_description<<"</description>"<<endl;
	tabs = string(x_tabs, '\t');
	rx_os<<tabs<<"</param>"<<endl;
}

/**
* @brief Set the range of acceptable values
*
* @param x_range Range in the form "[val1,val2,val3]"
*/
void ParameterString::SetRange(const string& x_range)
{
	if(x_range.substr(0, 1) != "[" || x_range.substr(x_range.size() - 1, 1) != "]")
		throw MkException("Error in range " + x_range, LOC);
	split(x_range.substr(1, x_range.size() - 2), ',', m_valuesInRange);
	// Remove last element if empty, due to an extra comma
	if(!m_valuesInRange.empty() && m_valuesInRange.back() == "")
		m_valuesInRange.pop_back();
}

/**
* @brief Return the range of values
*
*/
std::string ParameterString::GetRange() const
{
	return "[" + join(m_valuesInRange, ',') + "]";
}

/**
 * @brief Generate values in range
 *
 * @param x_nbSamples Number of valuew to generate
 * @param rx_values    Output
 * @param x_range      Range (if empty take parameter range)
 *
 */
void ParameterString::GenerateValues(int x_nbSamples, vector<string>& rx_values, const string& x_range) const
{
	if(!x_range.empty())
	{
		rx_values.clear();
		if(!x_range.empty())
			split(x_range.substr(1, x_range.size() - 2), ',', rx_values);
		else
			rx_values.push_back(m_default);
	}
	else rx_values = m_valuesInRange;
}
