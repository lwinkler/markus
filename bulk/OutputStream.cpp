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

#include "OutputStream.h"


OutputStream::OutputStream(const std::string& x_name, StreamType x_type, IplImage* x_image) :
	m_name(x_name),
	m_type(x_type),
	m_image(x_image),
	m_width(x_image->width),
	m_height(x_image->height),
	m_color(cvScalar(255,255,255))
{
}

OutputStream::OutputStream(const std::string& x_name, StreamType x_type, int x_width, int x_height) :
	m_name(x_name),
	m_type(x_type),
	m_image(NULL),
	m_width(x_width),
	m_height(x_height),
	m_color(cvScalar(255,255,255))
{
}

OutputStream::~OutputStream()
{

}
