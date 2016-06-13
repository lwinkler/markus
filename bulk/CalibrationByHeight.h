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

#ifndef CALIBRATION_BY_HEIGHT_H
#define CALIBRATION_BY_HEIGHT_H

#include "Serializable.h"

class CalibrationByHeight : public Serializable
{
public:

	CalibrationByHeight();
	CalibrationByHeight(double x_value_x, double x_value_y, double x_height);
	void Serialize(std::ostream& x_out, MkDirectory* xp_dir = nullptr) const;
	void Deserialize(std::istream& x_in, MkDirectory* xp_dir = nullptr);

	double x;
	double y;
	double height;
};


#endif
