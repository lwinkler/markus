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

#include "StreamDebug.h"


StreamDebug::StreamDebug(const std::string& x_name, IplImage* x_image) : 
	OutputStream(x_name, STREAM_DEBUG, x_image->width, x_image->height),
	m_image(x_image)
{
}


StreamDebug::~StreamDebug()
{

}

void StreamDebug::Render(IplImage * xp_output) const
{
	cvCopy(m_image, xp_output);
}
