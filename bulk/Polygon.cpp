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

#include "Polygon.h"
#include "jsoncpp/json/reader.h"
#include "jsoncpp/json/writer.h"


using namespace std;
using namespace cv;

Polygon::Polygon()
{
	m_value.push_back(Point(0.0,0.0));
}

Polygon::Polygon(const vector<Point>& x_value)
: m_value(x_value)
{
}

void Polygon::Serialize(ostream& x_out, const string& x_dir) const
{
	Json::Value root;
	Json::Value vect;
	for (uint i=0;i<m_value.size();i++)
	{
		Json::Value point;
		point.append(Json::Value(m_value[i].x));
		point.append(Json::Value(m_value[i].y));
		vect.append(point);
	}
	root["points"] = vect;
	x_out << root;
}

void Polygon::Deserialize(istream& x_in, const string& x_dir)
{
	m_value.clear();
	Json::Value root;
	x_in >> root;
	const Json::Value array = root["points"];
	for(unsigned int point_id=0; point_id<array.size();++point_id)
	{
		Point p(array[point_id][0].asFloat(),array[point_id][1].asFloat());
		m_value.push_back(p);
	}
}
