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

#ifndef STREAM_OBJECT_H
#define STREAM_OBJECT_H

#include "StreamT.h"
#include "Object.h"

typedef StreamT<std::vector<Object>> StreamObject;

// partial specialization
template<> StreamObject::StreamT(const std::string& rx_name, std::vector<Object>& rx_object, Module& rx_module, const std::string& rx_description, const mkjson& rx_requirement);
template<>void StreamObject::ConvertInput();
template<>void StreamObject::RenderTo(cv::Mat& x_output) const;
template<>void StreamObject::Query(std::ostream& xr_out, const cv::Point& x_pt) const;
template<>void StreamObject::Randomize(unsigned int& xr_seed);
template<>void StreamObject::Serialize(mkjson& rx_json, MkDirectory* xp_dir) const;
template<>void StreamObject::Deserialize(const mkjson& x_json, MkDirectory* xp_dir);

#endif
