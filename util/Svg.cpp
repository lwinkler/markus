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

#include "Svg.h"

using namespace std;
using namespace cv;

#define SVG_WIDTH  640.0
#define SVG_HEIGHT 480.0
#define SVG_STYLE "fill-opacity: 0; stroke-width: 3"

log4cxx::LoggerPtr Svg::m_logger(log4cxx::Logger::getLogger("Svg"));

Svg::Svg(const std::string& x_fileName, const Size& x_size) :
	m_fileName(x_fileName),
	m_file(x_fileName, true, false),
	m_svg(m_file.FindRef("svg", true)),
	m_widthRatio (SVG_WIDTH  / x_size.width),
	m_heightRatio(SVG_HEIGHT / x_size.height)
{
	m_svg.SetAttribute("width",  SVG_WIDTH);
	m_svg.SetAttribute("height", SVG_HEIGHT);
	m_svg.SetAttribute("style",  SVG_STYLE);
}

void Svg::Draw(const Object& x_object, const string& x_color)
{
	ConfigReader rect = m_svg.Append("rect");
	Rect r(x_object.GetRect());
	rect.SetAttribute("x", r.x * m_widthRatio);
	rect.SetAttribute("y", r.y * m_heightRatio);
	rect.SetAttribute("width", r.width * m_widthRatio);
	rect.SetAttribute("height", r.height * m_heightRatio);
	rect.SetAttribute("stroke", x_color);
}

void Svg::Line(const Point& x_point1, const Point& x_point2, const string& x_color)
{
	ConfigReader line = m_svg.Append("line");
	line.SetAttribute("x1", x_point1.x * m_widthRatio);
	line.SetAttribute("y1", x_point1.y * m_heightRatio);
	line.SetAttribute("x2", x_point2.x * m_widthRatio);
	line.SetAttribute("y2", x_point2.y * m_heightRatio);
	line.SetAttribute("stroke", x_color);
}

void Svg::Draw(const Polygon& x_polygon, const std::string& x_color, double x_diagonal)
{
	vector <Point> pts;
	for(const auto& pt : x_polygon.points)
		pts.emplace_back(pt.x * x_diagonal, pt.y * x_diagonal);

	Draw(pts, x_color, true);
}