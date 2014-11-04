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

CalibrationByModel::CalibrationByModel() {
	camera_height = 0.0;
	yaw = 0.0;
	roll = 0.0;
	focal = 0.0;
	height_model = 0;
	width_model = 0;
}


CalibrationByModel::CalibrationByModel(double camera_height, double yaw, double roll,double focal, int height_model, int width_model) {
	this->camera_height = camera_height;
	this->yaw = yaw;
	this->roll = roll;
	this->focal = focal;
	this->height_model = height_model;
	this->width_model = width_model;
}

void CalibrationByModel::Serialize(std::ostream& x_out, const std::string& x_dir) const {
	
	Json::Value root;
	root["camera_height"] = this->camera_height;
	root["yaw"] = this->yaw;
	root["roll"] = this->roll;
	root["focal"] = this->focal;
	root["height_model"] = this->height_model;
	root["width_model"] = this->width_model;
	x_out << root;
}

void CalibrationByModel::Deserialize(std::istream& x_in, const std::string& x_dir) {
	Json::Value root;
	x_in >> root;
	camera_height = root["camera_height"].asDouble();
	yaw = root["yaw"].asDouble();
	roll = root["roll"].asDouble();
	focal = root["focal"].asDouble();
	height_model = root["height_model"].asInt();
	width_model = root["width_model"].asInt();
}
