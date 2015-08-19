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
	m_img_input = nullptr;
}


StreamImage::~StreamImage()
{
}

// Convert an input (image from a different module) to the correct resolution into m_image. This method keeps a map containing all temporary image in case they are needed later
void StreamImage::ConvertInput()
{
	if(m_connected == nullptr)
	{
		m_image.setTo(0);
		return;
	}

	mp_connectedImage->ConvertToOutput(mr_module.GetCurrentTimeStamp(), m_image);
}

// Convert to an output (from the input) to take advantage of the buffer
void StreamImage::ConvertToOutput(TIME_STAMP x_ts, cv::Mat& xr_output)
{
	const Mat* corrected = &m_image;
	TIME_STAMP ts = mr_module.GetCurrentTimeStamp();

	if(corrected->cols != xr_output.cols || corrected->rows != xr_output.rows)
	{
		// Create a corrected image to the right size
		BufferImage* buf_out = &m_buffers[createResolutionString(xr_output.size(), corrected->depth(), corrected->channels())];
		if(buf_out->timeStamp != x_ts)
		{
#ifdef MK_AREA_SCALING
			// note: Maybe one day, parametrize the interpolation method
			//       This method is more CPU intensive and has not shown better results so far
			if(corrected->cols >= xr_output.cols)
			{
				resize(*corrected, buf_out->image, xr_output.size(), 0, 0, CV_INTER_AREA);
			}
			else
			{
				resize(*corrected, buf_out->image, xr_output.size(), 0, 0, CV_INTER_LINEAR);
			}
#else
			resize(*corrected, buf_out->image, xr_output.size(), 0, 0, CV_INTER_LINEAR);
#endif
			buf_out->timeStamp = x_ts;
		}
		corrected = &buf_out->image;
	}

	if(corrected->channels() != xr_output.channels())
	{
		// Create a corrected image to the right number of channels
		BufferImage* buf_out = &m_buffers[createResolutionString(xr_output.size(), corrected->depth(), xr_output.channels())];
		if(buf_out->timeStamp != x_ts)
		{
			if(corrected->channels() == 1 && xr_output.channels() == 3)
			{
				cvtColor(*corrected, buf_out->image, CV_GRAY2BGR);
			}
			else if(corrected->channels() == 3 && xr_output.channels() == 1)
			{
				cvtColor(*corrected, buf_out->image, CV_BGR2GRAY);
			}
			else throw MkException("Cannot convert channels", LOC);
			buf_out->timeStamp = x_ts;
		}
		corrected = &buf_out->image;
	}

	if(corrected->depth() != xr_output.depth())
	{
		// Create a corrected image with the correct depth
		BufferImage* buf_out = &m_buffers[createResolutionString(xr_output.size(), xr_output.depth(), xr_output.channels())];
		if(buf_out->timeStamp != x_ts)
		{
			if(corrected->depth() == CV_8U && xr_output.depth() == CV_32F)
			{
				corrected->convertTo(buf_out->image, xr_output.type(), 1.0 / 255);
			}
			else if(corrected->depth() == CV_32F && xr_output.depth() == CV_8U)
			{
				corrected->convertTo(buf_out->image, xr_output.type(), 255);
			}
			else throw MkException("Cannot convert depth", LOC);
			buf_out->timeStamp = x_ts;
		}
		corrected = &buf_out->image;
	}

	// Copy the correct image to output
	corrected->copyTo(xr_output);
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
	assert(x_stream != nullptr);
	m_connected = x_stream;
	if(x_bothWays)
		x_stream->Connect(this, false);

	mp_connectedImage = dynamic_cast<StreamImage*>(m_connected);
	if(mp_connectedImage == nullptr)
	{
		m_connected = nullptr;
		throw MkException("Input stream cannot be connected probably because it is not of type StreamImage", LOC);
	}
	m_img_input = &mp_connectedImage->GetImage();
}
