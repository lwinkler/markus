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

#include "CreationFunction.h"

namespace mk {
void to_json(mkjson& rx_json, const CreationFunction& x_ser)
{
	rx_json = mkjson{
		{"name", x_ser.name},
		{"number", x_ser.number},
		{"parameters", x_ser.parameters}
	};
}

void from_json(const mkjson& x_json, CreationFunction& rx_ser)
{
	rx_ser.name       = x_json.at("name").get<std::string>();
	rx_ser.number     = x_json.at("number").get<unsigned int>();
	rx_ser.parameters = x_json.at("parameters");
}
} // namespace mk
