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
#include "util.h"
#include "feature_util.h"
#include "jsoncpp/json/reader.h"
#include "jsoncpp/json/writer.h"


using namespace std;
using namespace cv;

Polygon::Polygon()
{
	// points.push_back(Point2f(0.0,0.0));
}

Polygon::Polygon(const vector<Point2f>& x_value)
: points(x_value)
{
}

/**
* @brief Draw the mask corresponding with the polygon on target mat
*/
void Polygon::DrawMask(cv::Mat& xr_target, const cv::Scalar& x_color)
{
	if(points.empty())
		return;
	const int npoints = points.size();
	double diag = diagonal(xr_target);
	vector<Point> scaledPts;
	vector<Point2f>::const_iterator itpts = points.begin();
	scaledPts.resize(points.size());
	for(vector<Point>::iterator it = scaledPts.begin() ; it != scaledPts.end() ; it++)
	{
		*it  = *itpts * diag;
	cout<<*it<<"ADSF"<<endl;
		itpts++;
	}

	const Point* ppts[1];
	ppts[0] = scaledPts.data();
	fillPoly(xr_target, ppts, &npoints, 1, x_color);
}

void Polygon::Serialize(ostream& x_out, const string& x_dir) const
{
	x_out<<"{\"points\":";
	serialize(x_out, points);
	x_out<<"}";
}

void Polygon::Deserialize(istream& x_in, const string& x_dir)
{
	Json::Value root;
	x_in >> root;
	stringstream ss;
	ss << root["points"];
	deserialize(ss, points);
}
