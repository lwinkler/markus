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

#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <iostream>
#include <sstream> // TODO remove
#include "json.hpp"
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>

using mkjson = nlohmann::json;

/// Class for all serializable objects

class MkDirectory;

std::string signatureJsonTree(const mkjson &x_root, int x_depth);
inline std::string signature(const mkjson& x_json)
{
	return signatureJsonTree(x_json, 0);
}

// std::string signature(std::istream& x_in);


#endif
