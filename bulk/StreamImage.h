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

#ifndef STREAM_IMAGE_H
#define STREAM_IMAGE_H

#include "cv.h"

#include "OutputStream.h"


class StreamImage : public OutputStream
{
public:
	StreamImage(const std::string& x_name, IplImage* x_image);
	~StreamImage();
	const std::string& GetName() const {return m_name;};
	const IplImage* GetImageRef() const {return m_image;};
	inline int GetWidth() const {return m_width;};
	inline int GetHeight() const {return m_height;};
	
	virtual void Render(IplImage * xp_output) const;
protected:
	const IplImage * m_image;
private:
	StreamImage& operator=(const StreamImage&);
	StreamImage(const StreamImage&);
};

#endif