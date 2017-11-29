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

log4cxx::LoggerPtr ParameterSerializable::m_logger(log4cxx::Logger::getLogger("ParameterSerializable"));


/**
 * @brief Generate values in range
 *
 * @param x_nbSamples Number of valuew to generate
 * @param rx_values Output
 * @param x_range      Range (if empty take parameter range)
 *
 */
mkjson ParameterSerializable::GenerateValues(int x_nbSamples, const mkjson& x_range) const
{
	return mkjson::array();
	// string range = x_range == "" ? GetRange() : x_range;
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

void ParameterSerializable::SetValueToDefault() 
{
	if(IsLocked())
		throw MkException("You tried to set the value of a locked parameter.", LOC);

	std::stringstream ss;
	ss << m_default;
	// mr_value.Deserialize(ss);
	ss << mr_value.dump();
	m_confSource = PARAMCONF_DEF;
}
