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

#include "CalibrationByModel.h"
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>


using namespace std;

CalibrationByModel::CalibrationByModel()
{
	camera_height = 0.0;
	yaw           = 0.0;
	roll          = 0.0;
	focal         = 0.0;
	nrows         = 0;
	ncols         = 0;
}


CalibrationByModel::CalibrationByModel(double x_camera_height, double x_yaw, double x_roll,double x_focal, int x_height_model, int x_width_model)
{
	camera_height = x_camera_height;
	yaw           = x_yaw;
	roll          = x_roll;
	focal         = x_focal;
	nrows         = x_height_model;
	ncols         = x_width_model;
}

void CalibrationByModel::Serialize(std::ostream& x_out, MkDirectory* xp_dir) const
{

	Json::Value root;
	root["height"] = camera_height;
	root["yaw"]           = yaw;
	root["roll"]          = roll;
	root["focal"]         = focal;
	root["nrows"]         = nrows;
	root["ncols"]         = ncols;
	x_out << root;
}

void CalibrationByModel::Deserialize(std::istream& x_in, MkDirectory* xp_dir)
{
	Json::Value root;
	x_in >> root;
	camera_height = root["height"].asDouble();
	yaw           = root["yaw"].asDouble();
	roll          = root["roll"].asDouble();
	focal         = root["focal"].asDouble();
	nrows         = root["nrows"].asInt();
	ncols         = root["ncols"].asInt();
}
