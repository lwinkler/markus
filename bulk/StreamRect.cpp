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

StreamRect::StreamRect(const std::string& x_name, int x_width, int x_height, vector<Rect>& r_rects) : 
	OutputStream(x_name, STREAM_IMAGE, x_width, x_height),
	m_rects(r_rects),
	m_color(CV_RGB(128,128,0))
{
	//m_rects.clear();
}


StreamRect::~StreamRect()
{

}

// Render : Draw rectangles on image
void StreamRect::Render(IplImage * xp_output) const
{
	//cvSet(xp_output, CV_RGB(255,0,0));
	//m_rects.clear();
	for(vector<Rect>::const_iterator it = m_rects.begin() ; it != m_rects.end() ; it++)
	{
		// TODO : See if we move this to execute it once only
		Point p1(it->x, it->y);
		Point p2(it->x + it->width, it->y + it->height);
		
		
		// Draw the rectangle in the input image
		cvRectangle( xp_output, p1, p2, m_color, 1, 8, 0 );
		
		// Add rectangle to output streams
		//m_rects.push_back(cv::Rect(p1, p2));
        }
}
