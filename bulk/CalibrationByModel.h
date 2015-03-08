/*----------------------------------------------------------------------------------
*
*    MARKUS : a manager for video analysis modules
*
*    Author : Florian Rossier <florian.rossier@gmail.com>
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

#ifndef CALIBRATION_BY_MODEL_H
#define CALIBRATION_BY_MODEL_H

#include "Serializable.h"

class CalibrationByModel : public Serializable
{
public:

	CalibrationByModel();
	CalibrationByModel(double x_camera_height, double x_yaw, double x_roll, double x_focal, int x_height_model, int x_width_model);
	void Serialize(std::ostream& x_out, const std::string& x_dir ="") const;
	void Deserialize(std::istream& x_in, const std::string& x_dir ="");

	double camera_height; // in m
	double yaw;           // in degree
	double roll;          // in degree
	double focal;         // un m
	int height_model;     // in px
	int width_model;      // in px
};


#endif
