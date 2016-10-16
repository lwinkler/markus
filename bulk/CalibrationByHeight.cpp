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

#include "CalibrationByHeight.h"
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>


using namespace std;

CalibrationByHeight::CalibrationByHeight()
{
	x = 0.0;
	y = 0.0;
	height = 0.0;
}

CalibrationByHeight::CalibrationByHeight(double x_value_x, double x_value_y, double x_height)
{
	x = x_value_x;
	y = x_value_y;
	height = x_height;
}

void CalibrationByHeight::Serialize(ostream& x_out, MkDirectory* xp_dir) const
{
	Json::Value root;

	root["x"] = x;
	root["y"] = y;
	root["height"] = height;


	x_out << root;
}

void CalibrationByHeight::Deserialize(istream& x_in, MkDirectory* xp_dir)
{
	Json::Value root;
	x_in >> root;
	x = root["x"].asDouble();
	y = root["y"].asDouble();
	height = root["height"].asDouble();
}
