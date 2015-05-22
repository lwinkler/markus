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

/// This module can be used as parent to all background subtraction modules

#include "BackgroundSubtraction.h"
#include "ControllerBackground.h"
#include "util.h"

using namespace std;


BackgroundSubtraction::BackgroundSubtraction(ParameterStructure& xr_params) : 
	Module(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params))
{}

BackgroundSubtraction::~BackgroundSubtraction()
{
}

void BackgroundSubtraction::Reset()
{
	Module::Reset();
	m_online_learn_time_ms = m_param.online_learn_time * 1000;

	// Add a new control to play forward and rewind
	if(!HasController("background"))
		AddController(new ControllerBackground(*this));
}
