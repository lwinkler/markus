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

#include "StreamRect.h"
#include <iostream>

using namespace std;
using namespace cv;

StreamRect::StreamRect(const std::string& x_name, int x_width, int x_height, vector<Rect>& r_rects, const CvScalar& x_color) : 
	Stream(x_name, STREAM_IMAGE, x_width, x_height),
	m_rects(r_rects),
	m_color(x_color)
{
	//m_rects.clear();
}


StreamRect::~StreamRect()
{

}

void StreamRect::ConvertInput()
{
	assert(m_connected != NULL);
	assert(false); // TODO
}

// Render : Draw rectangles on image
void StreamRect::Render(Mat * xp_output) const
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
