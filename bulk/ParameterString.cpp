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

/**
 * @brief Generate values in range
 *
 * @param x_nbSamples Number of valuew to generate
 * @param rx_values    Output
 * @param x_range      Range (if empty take parameter range)
 *
 */
mkjson ParameterString::GenerateValues(int x_nbSamples, const mkjson& x_range) const
{
	const mkjson range = x_range.is_null() ? GetRange() : x_range;
	if(range.find("allowed") != range.end())
	{
		return range.at("allowed");
	}
	if(range.find("advised") != range.end())
	{
		return range["advised"];
	}
	else return GetDefault();
}
