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
// #include <iostream>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

StreamImage::StreamImage(const std::string& x_name, cv::Mat& x_image, Module& rx_module, const string& rx_description) : 
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
		m_timeStamp = GetConnected().GetTimeStamp();
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

void StreamImage::Serialize(std::ostream& x_out, const string& x_dir) const
{
	Json::Value root;
	stringstream ss;
	Stream::Serialize(ss, x_dir);
	ss >> root;
	ss.clear();
	string fileName = x_dir + "/" + GetModule().GetName() + "." + GetName() + ".jpg";
	cv::imwrite(fileName, m_image);
	root["image"] = fileName;
	x_out << root;
}

void StreamImage::Deserialize(std::istream& x_in, const string& x_dir)
{
	Json::Value root;
	x_in >> root;  // note: copy first for local use
	stringstream ss;
	ss << root;
	Stream::Deserialize(ss, x_dir);

	string fileName = root["image"].asString();
	m_image = cv::imread(fileName);
	if(m_image.empty())
		throw MkException("Cannot open serialized image from file " + fileName, LOC);
}

void StreamImage::Connect(Stream* x_stream)
{
	// This method was rewritten to avoid a dynamic cast at each ConvertInput
	m_connected = x_stream;
	if(m_connected == NULL)
		return;
	const StreamImage* tmp = dynamic_cast<const StreamImage*>(m_connected);
	if(tmp == NULL)
	{
		m_connected = NULL;
		throw MkException("Input stream cannot be connected probably because it is not of type StreamImage", LOC);
	}
	m_img_input = &tmp->GetImage();
}
