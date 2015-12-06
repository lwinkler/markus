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

#ifndef STREAM_IMAGE_H
#define STREAM_IMAGE_H

#include "StreamT.h"
#include <map>

/// Structure used to keep the time stamp along with a buffer image (to speed up conversion)
struct BufferImage
{
	BufferImage() : timeStamp(TIME_STAMP_MIN) {}
	TIME_STAMP timeStamp;
	cv::Mat    image;
};

typedef StreamT<cv::Mat> StreamImage;


/// Class for a stream of images (or video) used for input and output
template<> class StreamT<typename cv::Mat> : public Stream
{
public:
	StreamT(const std::string& x_name, cv::Mat& x_image, Module& rx_module, const std::string& x_description, const std::string& x_requirements = "");
	MKCLASS("StreamImage")
	MKTYPE("Image")

	virtual void ConvertInput();
	virtual void RenderTo(cv::Mat& x_output) const;
	virtual void Query(int x_posX, int x_posY) const;
	virtual void Serialize(std::ostream& stream, const std::string& x_dir) const;
	virtual void Deserialize(std::istream& stream, const std::string& x_dir);
	virtual void Randomize(unsigned int& xr_seed);
	const cv::Mat& GetImage() const {return m_image;}
	void Connect(Stream * x_stream);

protected:
	static std::string createResolutionString(const cv::Size x_size, int x_depth, int x_channels)
	{
		std::stringstream ss;
		ss << x_size.width << "x" << x_size.height << "_" << x_depth << "_" << x_channels;
		return ss.str();
	}
	void ConvertToOutput(TIME_STAMP x_ts, cv::Mat& xr_output);
	cv::Mat& m_image;
	StreamImage* mp_connectedImage;

	std::map<std::string, BufferImage> m_buffers;

private:
	// DISABLE_COPY(Stream)
	static log4cxx::LoggerPtr m_logger;
};

#endif
