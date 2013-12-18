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
#include "util.h"

using namespace std;
using namespace cv;

StreamImage::StreamImage(int x_id, const std::string& x_name, Mat* x_image, Module& rx_module, const string& rx_description) : 
	Stream(x_id, x_name, STREAM_IMAGE, x_image->cols, x_image->rows, rx_module, rx_description),
	m_image(x_image)
{
	m_img_tmp1 = NULL; // To convert the input
	m_img_tmp2 = NULL;
	m_img_input = NULL;
}


StreamImage::~StreamImage()
{
	if(m_img_tmp1 != NULL) delete m_img_tmp1;
	if(m_img_tmp2 != NULL) delete m_img_tmp2;
}

void StreamImage::ConvertInput()
{
	// Copy time stamp to output
	if(m_connected != NULL)
	{
		m_timeStamp = RefConnected().GetTimeStamp();
		adjust(m_img_input, m_image, m_img_tmp1, m_img_tmp2);
	}
	else
	{
		m_image->setTo(0);
	}
}


void StreamImage::RenderTo(Mat * xp_output) const
{
	m_image->copyTo(*xp_output);
}

void StreamImage::Connect(Stream* x_stream)
{
	m_connected = x_stream;
	m_img_input = dynamic_cast<const StreamImage*>(m_connected)->GetImageRef();
}
