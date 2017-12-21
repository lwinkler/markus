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

namespace mk {
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
	m_svg.SetAttribute("xmlns", "http://www.w3.org/2000/svg");
}

void Svg::Draw(const Object& x_object, const string& x_color)
{
	ConfigXml rect = m_svg.Append("rect");
	Rect r(x_object.GetRect());
	rect.SetAttribute("x", r.x * m_widthRatio);
	rect.SetAttribute("y", r.y * m_heightRatio);
	rect.SetAttribute("width", r.width * m_widthRatio);
	rect.SetAttribute("height", r.height * m_heightRatio);
	rect.SetAttribute("stroke", x_color);
}

void Svg::Line(const Point& x_point1, const Point& x_point2, const string& x_color)
{
	ConfigXml line = m_svg.Append("line");
	line.SetAttribute("x1", x_point1.x * m_widthRatio);
	line.SetAttribute("y1", x_point1.y * m_heightRatio);
	line.SetAttribute("x2", x_point2.x * m_widthRatio);
	line.SetAttribute("y2", x_point2.y * m_heightRatio);
	line.SetAttribute("stroke", x_color);
}

void Svg::LinkImage(const std::string& x_file)
{
	ConfigXml image = m_svg.Append("image");
	// link to an image can later create a broken link
	// image.SetAttribute("xlink:href", x_file);
	// No need to set x,y -> set to 0 by default
	// image.SetAttribute("x", 0);
	// image.SetAttribute("y", 0);
	image.SetAttribute("width", SVG_WIDTH);
	image.SetAttribute("height", SVG_HEIGHT);
}

void Svg::LinkImage(const std::string& x_file, const Rect& x_rect)
{
	ConfigXml image = m_svg.Append("image");
	// image.SetAttribute("xlink:href", x_file);
	image.SetAttribute("x", x_rect.x);
	image.SetAttribute("y", x_rect.y);
	image.SetAttribute("width", x_rect.width);
	image.SetAttribute("height", x_rect.height);
}
} // namespace mk
