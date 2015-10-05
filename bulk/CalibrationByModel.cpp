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
#include "jsoncpp/json/reader.h"
#include "jsoncpp/json/writer.h"


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

void CalibrationByModel::Serialize(MkJson& xr_out, const std::string& x_dir) const
{
	xr_out["height"] = camera_height;
	xr_out["yaw"]           = yaw;
	xr_out["roll"]          = roll;
	xr_out["focal"]         = focal;
	xr_out["nrows"]         = nrows;
	xr_out["ncols"]         = ncols;
}

void CalibrationByModel::Deserialize(MkJson& xr_in, const std::string& x_dir)
{
	camera_height = xr_in["height"].AsDouble(); // TODO: Deserialization SHOULD always check if the value exists
	yaw           = xr_in["yaw"].AsDouble();
	roll          = xr_in["roll"].AsDouble();
	focal         = xr_in["focal"].AsDouble();
	nrows         = xr_in["nrows"].AsInt();
	ncols         = xr_in["ncols"].AsInt();
}
