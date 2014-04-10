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
#include "util.h"
#include <iostream>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>

using namespace std;
using namespace cv;

StreamImage::StreamImage(const std::string& x_name, Mat& x_image, Module& rx_module, const string& rx_description) : 
	Stream(x_name, rx_module, rx_description),
	m_image(x_image)
{
	assert(x_image.cols == rx_module.GetWidth() && x_image.rows == rx_module.GetHeight());
	mp_img_tmp1 = NULL; // To convert the input
	mp_img_tmp2 = NULL;
	m_img_input = NULL;
}


StreamImage::~StreamImage()
{
	if(mp_img_tmp1 != NULL) delete mp_img_tmp1;
	if(mp_img_tmp2 != NULL) delete mp_img_tmp2;
}

void StreamImage::ConvertInput()
{
	// Copy time stamp to output
	if(m_connected != NULL)
	{
		m_timeStamp = RefConnected().GetTimeStamp();
		adjust(*m_img_input, m_image, mp_img_tmp1, mp_img_tmp2);
	}
	else
	{
		m_image.setTo(0);
	}
}


void StreamImage::RenderTo(Mat& x_output) const
{
	m_image.copyTo(x_output);
}

/// Serialize the stream content to a directory
/// @param x_in Input stream
/// @param x_dir Input directory (for images)
void StreamImage::Serialize(std::ostream& x_out, const string& x_dir) const
{
	Json::Value root;
	stringstream ss;
	Stream::Serialize(ss, x_dir);
	ss >> root;
	ss.clear();
	string fileName = x_dir + "/" + GetModule().GetName() + "." + GetName() + ".png";
	imwrite(fileName, m_image);
	root["image"]["file"]   = fileName;
	root["image"]["type"]   = ParameterImageType::ReverseEnum.at(m_image.type());
	root["image"]["width"]  = m_image.cols;
	root["image"]["height"] = m_image.rows;
	x_out << root;
}

/// Deserialize the stream from JSON
/// @param x_in Input stream
/// @param x_dir Input directory (for images)
void StreamImage::Deserialize(std::istream& x_in, const string& x_dir)
{
	// TODO
}

void StreamImage::Connect(Stream* x_stream)
{
	// TODO: Why do we rewrite this function ? 
	m_connected = x_stream;
	if(m_connected == NULL)
		return;
	const StreamImage* tmp = dynamic_cast<const StreamImage*>(m_connected);
	if(tmp == NULL)
	{
		m_connected = NULL;
		throw MkException("Input stream cannot be connected probably because it is not of type StreamImage", LOC);
	}
	m_img_input = &tmp->RefImage();
}
