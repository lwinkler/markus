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
#ifdef MARKUS_DEBUG_STREAMS

using namespace std;

/// This class represents a debug stream, a stream that is only used to help the user 
/// it can typically be used to show what is happening inside a detection module

StreamDebug::StreamDebug(int x_id, const std::string& x_name, cv::Mat& x_image, Module& rx_module, const string& rx_description) : 
	Stream(x_id, x_name, rx_module, rx_description),
	m_image(x_image)
{
	assert(x_image.cols == rx_module.GetWidth() && x_image.rows == rx_module.GetHeight());
}


StreamDebug::~StreamDebug()
{

}

/// Convert the input: not used since no debug stream is connected
void StreamDebug::ConvertInput()
{
	assert(false); // Should not be used
}

/// Draw the stream on an image
void StreamDebug::RenderTo(cv::Mat& x_output) const
{
	m_image.copyTo(x_output);
}

#endif
