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
#include "Object.h"

using namespace cv;
using namespace std;

Object::Object(const string& x_name) :
	m_name(x_name),
	m_id(-1),
	posX(0),
	posY(0),
	width(0),
	height(0)
{
};

Object::Object(const string& x_name, const cv::Rect& x_rect) :
        m_name(x_name)
{
	posX 	 = x_rect.x + x_rect.width / 2;
	posY 	 = x_rect.y + x_rect.height / 2;
	width	 = x_rect.width;
	height   = x_rect.height;
}

Object::~Object(){}

/// Draw an object on an image (for visualization)
void Object::RenderTo(Mat* xp_output, const Scalar& x_color) const
{
	//Rect rect = it1->GetRect();
	Point p1(posX - width / 2, posY - height / 2);
	Point p2(posX + width / 2, posY + height / 2);

	/*float scale = static_cast<float>(xp_output->cols) / width;
	p1.x = p1.x * scale;
	p2.x = p2.x * scale;
	scale = static_cast<float>(xp_output->rows) / height;
	p1.y = p1.y * scale;
	p2.y = p2.y * scale;*/

	// Draw the rectangle in the input image
	// if id is present, draw to the equivalent color
#ifndef MARKUS_DEBUG_STREAMS
	rectangle( *xp_output, p1, p2, Scalar(20,0,230), 3, 8, 0 );
#else
	Scalar color = x_color;
	Point pText = p1;

	if(GetId() >= 0)
	{
		color = colorFromId(GetId());
		ostringstream text;
		text<<GetName()<<" "<<GetId();
		pText.y -= 3;
		putText(*xp_output, text.str(), pText,  FONT_HERSHEY_COMPLEX_SMALL, 0.4, color);
	}
	else
	{
		// color from stream
		pText.y -= 3;
		putText(*xp_output, GetName(), pText, FONT_HERSHEY_COMPLEX_SMALL, 0.4, color);
	}
	rectangle( *xp_output, p1, p2, color, 1, 8, 0 );

	// Print features and values
	pText.x += 2;
	int i = 0;
	for(map<string, Feature>::const_iterator it2 = GetFeatures().begin() ; it2 != GetFeatures().end() ; it2++)
	{
		//try
		{
			ostringstream text;
			text<<it2->first<<"="<<it2->second.value;
			pText.y += 7;
			putText(*xp_output, text.str(), pText,  FONT_HERSHEY_COMPLEX_SMALL, 0.4, color);
			i++;
		}
		//catch(...){}
	}
#endif
}
