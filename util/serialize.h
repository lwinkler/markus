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
#include "json.hpp"

namespace mk {
/// Class for all serializable objects
using mkjson = nlohmann::json;
template<class T> inline void to_mkjson(mkjson& rx_js, const T& x_obj) 
{
	// rx_js = std::move(x_obj);
	// mkjson js(x_obj);
	// std::swap(rx_js, js);
	nlohmann::to_json(rx_js, x_obj);
}
template<class T> inline void from_mkjson(const mkjson& x_js, T& rx_obj) 
{
	// rx_obj = std::move(x_js.get<T>());
	// T obj{x_js};
	// std::swap(rx_obj, obj);
	nlohmann::from_json(x_js, rx_obj);
}

inline std::string oneLine(const mkjson& x_json)
{
	return x_json.dump(-1, '\t');
}

inline std::string multiLine(const mkjson& x_json)
{
	return x_json.dump(1, '\t');
}

std::string signatureJsonTree(const mkjson &x_root, int x_depth);
inline std::string signatureOfFeature(const mkjson& x_json)
{
	return signatureJsonTree(x_json, 0);
}

inline bool exist(const mkjson x_arr, const std::string& x_element){return x_arr.find(x_element) != x_arr.end();}


} // namespace mk
#endif
