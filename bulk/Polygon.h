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

#ifndef POLYGON_H
#define POLYGON_H

#include "Serializable.h"
#include "MkException.h"
#include <opencv2/opencv.hpp>
#include <vector>


class Polygon : public Serializable
{
public:

	Polygon();
	Polygon(const std::vector<cv::Point2d>& x_value);
	void DrawMask(cv::Mat& xr_target, const cv::Scalar& x_color) const;
	template<typename T> void GetPoints(std::vector<T>& xr_points, const cv::Size& x_size) const
	{
		xr_points.clear();
		if(points.empty())
			return;
		assert(m_width * m_height > 0);
		double correctionRatioX = x_size.width  / m_width; // 1 / (m_width / m_height) * diag;
		double correctionRatioY = x_size.height / m_height; // 1 / (m_width / m_height) * diag;
		auto itpts = points.begin();
		xr_points.resize(points.size());
		for(auto & scaledPt : xr_points)
		{
			scaledPt.x = itpts->x * correctionRatioX;
			scaledPt.y = itpts->y * correctionRatioY;
			// std::cout << *itpts << " : " << scaledPt.x <<  ">=" << x_size.width << " " << scaledPt.y << ">=" << x_size.height << std::endl;
			// if(scaledPt.x >= x_size.width || scaledPt.y >= x_size.height)
			// throw MkException("Drawing out of bounds of image ", LOC);
			++itpts;
		}
	}
	inline size_t Size() const {return points.size();}
	virtual void Serialize(std::ostream& x_out, MkDirectory* xp_dir = nullptr) const override;
	virtual void Deserialize(std::istream& x_in, MkDirectory* xp_dir = nullptr) override;

protected:
	std::vector<cv::Point2d> points;
	double m_width  = 0;
	double m_height = 0;
};

// partial specialization
template<> void Polygon::GetPoints<cv::Point>(std::vector<cv::Point>& xr_points, const cv::Size& x_size) const;


#endif
