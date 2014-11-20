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

#include "ParameterSerializable.h"

using namespace std;


void ParameterSerializable::Export(ostream& rx_os, int x_indentation)
{
	string tabs(x_indentation, '\t');
	rx_os<<tabs<<"<param name=\""<<m_name<<"\">"<<endl;
	tabs = string(x_indentation + 1, '\t');
	rx_os<<tabs<<"<type>"<<GetTypeString()<<"</type>"<<endl;
	rx_os<<tabs<<"<value default=\'"<<m_default<<"\'>";
	rx_os<<mr_value.SerializeToString()<<"</value>"<<endl;
	rx_os<<tabs<<"<description>"<<m_description<<"</description>"<<endl;
	tabs = string(x_indentation, '\t');
	rx_os<<tabs<<"</param>"<<endl;
}

/**
 * @brief Generate values in range
 *
 * @param x_nbSamples Number of valuew to generate
 * @param rx_values Output
 * @param x_range      Range (if empty take parameter range)
 *
 */
void ParameterSerializable::GenerateValues(int x_nbSamples, vector<string>& rx_values, const string& x_range) const
{
	// string range = x_range == "" ? GetRange() : x_range;
	rx_values.clear();
	// throw MkException("Method is not implemented", LOC);
	/*
	split(x_range.substr(1, x_range.size() - 2), ',', rx_values);

	// Remove last element if empty, due to an extra comma
	CPPUNIT_ASSERT(rx_values.size() > 0);
	if(rx_values.back() == "")
		rx_values.pop_back();
	assert(rx_values.size() > 0);
	*/
}
