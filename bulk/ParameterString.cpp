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

void ParameterString::Export(ostream& rx_os, int x_tabs)
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
 * @brief Generate values in range
 *
 * @param x_nbSamples Number of valuew to generate
 * @param rx_values    Output
 * @param x_range      Range (if empty take parameter range)
 *
 */
void ParameterString::GenerateValues(int x_nbSamples, vector<string>& rx_values, const string& x_range) const
{
	// TODO string range = x_range == "" ? GetRange() : x_range;
	rx_values.clear();
	rx_values.push_back(m_default); // TODO : probably use requirements
	/*
	split(range.substr(1, range.size() - 2), ',', rx_values);
	if(rx_values.back() == "")
		rx_values.pop_back();
	if(rx_values.empty())
		throw MkException("Value array is empty, range= " + range, LOC);
		*/
}
