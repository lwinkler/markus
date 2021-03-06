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

#ifndef STREAM_STATE_H
#define STREAM_STATE_H

#include "StreamT.h"

namespace mk {
typedef StreamT<bool> StreamState;

// partial specialization
template<> void StreamT<bool>::ConvertInput();
template<> void StreamT<bool>::RenderTo(cv::Mat& x_output) const;
template<> void StreamT<bool>::Query(std::ostream& xr_out, const cv::Point& x_pt) const;
template<> void StreamT<bool>::Randomize(unsigned int& xr_seed);
template<> void StreamT<bool>::Serialize(mkjson& rx_json, MkDirectory* xp_dir) const;
template<> void StreamT<bool>::Deserialize(const mkjson& x_json, MkDirectory* xp_dir);

} // namespace mk
#endif
