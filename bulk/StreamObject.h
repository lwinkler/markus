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

#ifndef STREAM_RECT_H
#define STREAM_RECT_H

#include "cv.h"

#include "Stream.h"

/// Stream in the form of located objects

class StreamObject : public Stream
{
public:
	StreamObject(int x_id, const std::string& x_name, int x_width, int x_height, 
		   std::vector<cv::Rect>& r_rects, const CvScalar& x_color, Module& rx_module, const std::string& rx_description);
	~StreamObject();
	void Clear() {m_rects.clear();};
	void AddRect(cv::Rect x_rect) {m_rects.push_back(x_rect);};
	
	virtual void ConvertInput();
	virtual void RenderTo(cv::Mat * xp_output) const;
	inline virtual const std::string GetTypeString()const {return "Rect";};
protected:
	std::vector<cv::Rect> & m_rects;
private:
	CvScalar m_color;
	StreamObject& operator=(const StreamObject&);
	StreamObject(const StreamObject&);
};

#endif