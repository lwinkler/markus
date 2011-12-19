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

#include "StreamImage.h"
#include <iostream>
using namespace std;
using namespace cv;

StreamImage::StreamImage(const std::string& x_name, Mat* x_image) : 
	Stream(x_name, STREAM_IMAGE, x_image->cols, x_image->rows),
	m_image(x_image)
{
}


StreamImage::~StreamImage()
{

}

void StreamImage::Render(Mat * xp_output) const
{
	//cout<<"xp_output->width "<<xp_output->width<<endl;
	//cout<<"GetWidth()"<<GetWidth()<<endl;
	//assert(xp_output->width == GetWidth());
	//assert(xp_output->height == GetHeight());
	cvCopy(m_image, xp_output);
}
