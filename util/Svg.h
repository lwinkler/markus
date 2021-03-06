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

#ifndef MK_SVG_H
#define MK_SVG_H

#include "ConfigXml.h"
#include "Object.h"
#include "Polygon.h"


namespace mk {
/**
* @brief A helper class to easely write SVG files
*/
class Svg
{
public:
	Svg(const std::string& x_fileName, const cv::Size& x_size);

	// Draw an object (as a box)
	void Draw(const Object& x_object, const std::string& x_color);

	// Draw a line segment
	void Line(const cv::Point& x_point1, const cv::Point& x_point2, const std::string& x_color);

	/// Draw a path or polygon
	template<class P>
	void Draw(const std::vector<P>& x_points, const std::string& x_color, bool x_closed)
	{
		std::stringstream ss;

		if(x_points.empty())
			return;
		// throw MkException("Cannot draw path containing less than 1 point yet", LOC);

		ss << "M" << x_points.begin()->x * m_widthRatio << " " << x_points.begin()->y * m_heightRatio;

		for(typename std::vector<P>::const_iterator it = x_points.begin() + 1 ; it != x_points.end(); ++it)
			ss << " L" << it->x * m_widthRatio << " " << it->y * m_heightRatio;
		if(x_closed)
			ss << " Z";

		ConfigXml path = m_svg.Append("path");
		path.SetAttribute("d", ss.str());
		path.SetAttribute("stroke", x_color);
	}

	/// Draw a polygon
	inline void Draw(const Polygon& x_polygon, const std::string& x_color, const cv::Size& x_size)
	{
		std::vector <cv::Point2d> pts;
		x_polygon.GetPoints(pts, x_size);

		Draw(pts, x_color, true);
	}

	/// Write the SVG file
	void Write() const {m_file.SaveToFile(m_fileName);}

	/// Include an external image file (as link) use the whole canvas
	void LinkImage(const std::string& x_file);

	/// Include an external image file (as link) specify the position
	void LinkImage(const std::string& x_file, const cv::Rect& x_rect);

private:
	static log4cxx::LoggerPtr m_logger;

protected:
	const std::string m_fileName;
	ConfigFileXml m_file;
	ConfigXml     m_svg;
	const double  m_widthRatio;
	const double  m_heightRatio;
};

} // namespace mk
#endif
