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
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>


using namespace std;
using namespace cv;

/// Constructor
Polygon::Polygon()
{
	// m_points.push_back(Point2f(0.0,0.0));
}

/// Constructor from a set of points
Polygon::Polygon(const vector<cv::Point2d>& x_value)
	: m_points(x_value)
{
}

/// Specialization of method for Point: check if at boundary
template<> void Polygon::GetPoints<cv::Point>(std::vector<cv::Point>& xr_points, const cv::Size& x_size) const
{
	xr_points.clear();
	if(m_points.empty())
		return;
	assert(m_width * m_height > 0);
	assert(x_size.width * x_size.height > 0);
	double correctionRatioX = x_size.width  / m_width; // 1 / (m_width / m_height) * diag;
	double correctionRatioY = x_size.height / m_height; // 1 / (m_width / m_height) * diag;
	auto itpts = m_points.begin();

	xr_points.resize(m_points.size());
	for(auto & scaledPt : xr_points)
	{
		scaledPt.x = itpts->x * correctionRatioX;
		scaledPt.y = itpts->y * correctionRatioY;
		// std::cout << *itpts << " : " << scaledPt.x <<  ">=" << x_size.width << " " << scaledPt.y << ">=" << x_size.height << std::endl;

		// If at the boundary of image, decrease. This might be a rounding error
		if(scaledPt.x == x_size.width)
			scaledPt.x = x_size.width - 1;
		if(scaledPt.y == x_size.height)
			scaledPt.y = x_size.height - 1;
		if(scaledPt.x > x_size.width || scaledPt.y > x_size.height)
		{
			stringstream ss;
			ss << scaledPt << " is outside " << x_size;
			throw MkException("Drawing out of bounds of image: " + ss.str(), LOC);
		}
		++itpts;
	}
}

/**
* @brief Draw the mask corresponding with the polygon on target mat
*/
void Polygon::DrawMask(Mat& xr_target, const Scalar& x_color) const
{
	if(m_points.empty())
		return;
	assert(m_width * m_height > 0);
	const int npoints = m_points.size();
	vector<Point> scaledPts;
	GetPoints(scaledPts, xr_target.size());

	const Point* ppts[1];
	ppts[0] = scaledPts.data();
	fillPoly(xr_target, ppts, &npoints, 1, x_color);
}


void to_json(mkjson& _json, const Polygon& _ser)
{
	_json = mkjson{{"width", _ser.m_width}, {"height", _ser.m_height}, {"points", _ser.m_points}};
}

void from_json(const mkjson& _json, Polygon& _ser)
{
	_ser.m_width = _json.at("width").get<double>();
	_ser.m_height = _json.at("height").get<double>();
	_ser.m_points = _json.at("points").get<vector<Point2d>>();

	if((_ser.m_width == 0 || _ser.m_height == 0) && ! _ser.m_points.empty())
		throw MkException("Polygon was serialized without specifying width or height", LOC);
}
