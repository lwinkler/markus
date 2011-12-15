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

#include "OutputStream.h"


class StreamRect : public OutputStream
{
public:
	StreamRect(const std::string& x_name, int x_width, int x_height, std::vector<cv::Rect>& r_rects);
	~StreamRect();
	void Clear() {m_rects.clear();};
	void AddRect(cv::Rect x_rect) {m_rects.push_back(x_rect);};
	
	virtual void Render(IplImage * xp_output) const;
protected:
	std::vector<cv::Rect> & m_rects;
private:
	const CvScalar m_color;
	StreamRect& operator=(const StreamRect&);
	StreamRect(const StreamRect&);
};

#endif