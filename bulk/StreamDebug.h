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

#ifndef STREAM_DEBUG_H
#define STREAM_DEBUG_H

#include "cv.h"

#include "Stream.h"


class StreamDebug : public Stream
{
public:
	StreamDebug(int x_id, const std::string& x_name, cv::Mat* x_image, Module& rx_module);
	~StreamDebug();
	const cv::Mat* GetImageRef() const {return m_image;};
	
	virtual void ConvertInput();
	virtual void Render(cv::Mat * xp_output) const;
protected:
	const cv::Mat * m_image;
private:
	StreamDebug& operator=(const StreamDebug&);
	StreamDebug(const StreamDebug&);
};

#endif