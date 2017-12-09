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

#include "StreamNum.h"

namespace mk {
using namespace std;

// Static variables
template<> const string StreamNum<bool>::className   = "StreamNum<bool>";
template<> const string StreamNum<int>::className    = "StreamNum<int>";
template<> const string StreamNum<uint>::className   = "StreamNum<uint>";
template<> const string StreamNum<float>::className  = "StreamNum<float>";
template<> const string StreamNum<double>::className = "StreamNum<double>";

}
