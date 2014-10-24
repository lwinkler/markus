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

#include "Stream.h"

/// Class for a stream of images (or video) used for input and output

class StreamImage : public Stream
{
public:
	StreamImage(const std::string& x_name, cv::Mat& x_image, Module& rx_module, const std::string& rx_description);
	~StreamImage();
	MKCLASS("StreamImage")
	MKTYPE("Image")
	
	virtual void ConvertInput();
	virtual void RenderTo(cv::Mat& x_output) const;
	virtual void Serialize(std::ostream& stream, const std::string& x_dir) const;
	virtual void Deserialize(std::istream& stream, const std::string& x_dir);
	virtual void Randomize(unsigned int& xr_seed);
	const cv::Mat& GetImage() const {return m_image;}
	void Connect(Stream * x_stream);
protected:
	cv::Mat& m_image;
	const cv::Mat* m_img_input;
	cv::Mat * mp_img_tmp1; // To convert the input
	cv::Mat * mp_img_tmp2;

private:
	DISABLE_COPY(StreamImage)
	static log4cxx::LoggerPtr m_logger;
};

#endif
