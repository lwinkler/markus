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

#ifndef CREATION_FUNCTION_H
#define CREATION_FUNCTION_H

#include "serialize.h"

namespace mk {
/**
* @brief An object to define a creation function for an OpenCV class
*/
class CreationFunction
{
	public:
		std::string name;
		unsigned int number;
		mkjson parameters;
};

void to_json(mkjson& rx_json, const CreationFunction& x_ser);
void from_json(const mkjson& x_json, CreationFunction& rx_ser);

} // namespace mk

#endif
