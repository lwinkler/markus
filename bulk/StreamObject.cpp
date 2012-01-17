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

#include "StreamObject.h"
#include <iostream>

using namespace std;
using namespace cv;

StreamObject::StreamObject(int x_id, const std::string& x_name, int x_width, int x_height, 
		       vector<Rect>& r_rects, const CvScalar& x_color, Module& rx_module, const string& rx_description) : 
	Stream(x_id, x_name, STREAM_IMAGE, x_width, x_height, rx_module, rx_description),
	m_rects(r_rects),
	m_color(x_color)
{
}


StreamObject::~StreamObject()
{

}

/// Convert the input to the right format

void StreamObject::ConvertInput()
{
	assert(m_connected != NULL);
	const StreamObject * pstream = dynamic_cast<const StreamObject*>(m_connected);
	std::vector<Rect> rectsTarget = pstream->m_rects;
	double ratioX = static_cast<double>(m_width) / pstream->GetInputWidth();
	double ratioY = static_cast<double>(m_height) / pstream->GetInputHeight();
	for(vector<Rect>::const_iterator it = m_rects.begin() ; it != m_rects.end() ; it++)
	{
		Rect rect;  
		rect.x 		= it->x * ratioX;
		rect.y 		= it->y * ratioY;
		rect.width 	= it->width  * ratioX;
		rect.height 	= it->height * ratioY;
		rectsTarget.push_back(rect);
	}
}

/// Render : Draw rectangles on image

void StreamObject::RenderTo(Mat * xp_output) const
{
	for(vector<Rect>::const_iterator it = m_rects.begin() ; it != m_rects.end() ; it++)
	{
		Point p1(it->x, it->y);
		Point p2(it->x + it->width, it->y + it->height);
		
		float scale = static_cast<float>(xp_output->cols) / m_width;
		p1.x = p1.x * scale;
		p2.x = p2.x * scale;
		scale = static_cast<float>(xp_output->rows) / m_height;
		p1.y = p1.y * scale;
		p2.y = p2.y * scale;
		
		// Draw the rectangle in the input image
		rectangle( *xp_output, p1, p2, m_color, 1, 8, 0 );
        }
}
