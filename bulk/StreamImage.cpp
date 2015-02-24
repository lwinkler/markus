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

log4cxx::LoggerPtr StreamImage::m_logger(log4cxx::Logger::getLogger("StreamImage"));

StreamImage::StreamImage(const string& x_name, Mat& x_image, Module& rx_module, const string& rx_description) :
	Stream(x_name, rx_module, rx_description),
	m_image(x_image)
{
	LOG_DEBUG(m_logger, "Create image "<<x_image.cols<<"x"<<x_image.rows<<" for module "<<rx_module.GetName()<<" of size "<<rx_module.GetWidth()<<"x"<<rx_module.GetHeight());
	// assert(x_image.cols == rx_module.GetWidth() && x_image.rows == rx_module.GetHeight()); // Disable this for unit tests
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

/// Query : give info about cursor position
void StreamImage::Query(int x_posX, int x_posY) const
{
	// check if out of bounds
	if(x_posX < 0 || x_posY < 0 || x_posX >= GetWidth() || x_posY >= GetHeight())
		return;
	
	Rect rect(x_posX, x_posY, 1, 1);
	LOG_INFO(m_logger, "Pixel value at " << x_posX << "," << x_posY << " = " << m_image(rect));
}

/// Randomize the content of the stream
void StreamImage::Randomize(unsigned int& xr_seed)
{
	// random image
	m_image = Mat(m_image.size(), m_image.type());
	m_image.setTo(0);
	int nb = rand_r(&xr_seed) % 100;
	for ( int i = 0; i < nb; i++ )
	{
		Point center;
		center.x = rand_r(&xr_seed) % m_image.cols;
		center.y = rand_r(&xr_seed) % m_image.rows;

		Size axes;
		axes.width  = rand_r(&xr_seed) % 200;
		axes.height = rand_r(&xr_seed) % 200;

		double angle = rand_r(&xr_seed) % 180;
		Scalar randomColor(rand_r(&xr_seed) % 255, rand_r(&xr_seed) % 255, rand_r(&xr_seed) % 255);

		ellipse(m_image, center, axes, angle, angle - 100, angle + 200,
				randomColor, (rand_r(&xr_seed) % 10) - 1);
	}
}

void StreamImage::Serialize(ostream& x_out, const string& x_dir) const
{
	Json::Value root;
	stringstream ss;
	Stream::Serialize(ss, x_dir);
	ss >> root;
	stringstream fileName;
	fileName << x_dir << "/" << GetModule().GetName() << "." << GetName() << "." << m_timeStamp << ".jpg";
	imwrite(fileName.str(), m_image);
	root["image"] = fileName.str();
	x_out << root;
}

void StreamImage::Deserialize(istream& x_in, const string& x_dir)
{
	Json::Value root;
	x_in >> root;  // note: copy first for local use
	stringstream ss;
	ss << root;
	Stream::Deserialize(ss, x_dir);

	string fileName = root["image"].asString();
	m_image = imread(fileName);
	if(m_image.empty())
		throw MkException("Cannot open serialized image from file " + fileName, LOC);
}

void StreamImage::Connect(Stream* x_stream, bool x_bothWays)
{
	// This method was rewritten to avoid a dynamic cast at each ConvertInput
	assert(x_stream != NULL);
	m_connected = x_stream;
	if(x_bothWays)
		x_stream->Connect(this, false);

	const StreamImage* tmp = dynamic_cast<const StreamImage*>(m_connected);
	if(tmp == NULL)
	{
		m_connected = NULL;
		throw MkException("Input stream cannot be connected probably because it is not of type StreamImage", LOC);
	}
	m_img_input = &tmp->GetImage();
}
