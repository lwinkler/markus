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

#ifndef OUTPUT_STREAM_H
#define OUTPUT_STREAM_H

#include "cv.h"

enum StreamType
{
	STREAM_DEBUG,
	STREAM_IMAGE,
	STREAM_RECTS,
	STREAM_POINTS
};


class Stream
{
public:
	Stream(const std::string& x_name, StreamType x_type);
	Stream(const std::string& x_name, StreamType x_type, int x_width, int x_height);
	~Stream();
	const std::string& GetName() const {return m_name;};
	inline int GetWidth() const {return m_width;};
	inline int GetHeight() const {return m_height;};
	inline StreamType GetType() const {return m_type;};
	virtual void Render(IplImage * x_output) const  = 0;
protected:
	const std::string m_name;
	const StreamType m_type;
	const int m_width;
	const int m_height;
private:
	Stream& operator=(const Stream&);
	Stream(const Stream&);
};

#endif