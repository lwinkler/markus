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
template<> class StreamT<cv::Mat> : public Stream
{
public:
	StreamT(const std::string& x_name, cv::Mat& x_image, Module& rx_module, const std::string& x_description, const std::string& x_requirements = "");
	MKCLASS("StreamImage")
	MKTYPE("Image")
	MKPARAMTYPE(PARAM_UNKNOWN)

	virtual void ConvertInput() override;
	virtual void RenderTo(cv::Mat& x_output) const override;
	virtual void Query(std::ostream& xr_out, const cv::Point& x_pt) const override;
	virtual void Serialize(std::ostream& stream, MkDirectory* xp_dir = nullptr) const override;
	virtual void Deserialize(std::istream& stream, MkDirectory* xp_dir = nullptr) override;
	virtual void Randomize(unsigned int& xr_seed) override;
	const cv::Mat& GetImage() const {return m_content;}
	void Connect(Stream& xr_stream) override;
	void Disconnect() override;

	virtual void SetValue(const ConfigReader& x_value, ParameterConfigType x_confType) override
	{
		LOG_WARN(m_logger, "Impossible to set the value of a stream of type image as a parameter");
		// m_confSource = x_confType;
	}
	virtual void SetDefault(const ConfigReader& x_value) override {LOG_WARN(m_logger, "Impossible to set the default value of a stream of type image as a parameter");}
	virtual void SetValueToDefault() override {m_content.setTo(0); m_confSource = PARAMCONF_DEF;};
	// note: This will not work with images
	virtual ConfigReader GetValue() const override {std::stringstream ss; ss << m_content; return ss.str();}
	virtual ConfigReader GetDefault() const override {cv::Mat def(m_content.size(), m_content.type()); def.setTo(0); std::stringstream ss; ss << def; return ss.str();}

protected:
	static std::string createResolutionString(const cv::Size x_size, int x_depth, int x_channels)
	{
		std::stringstream ss;
		ss << x_size.width << "x" << x_size.height << "_" << x_depth << "_" << x_channels;
		return ss.str();
	}
	void ConvertToOutput(TIME_STAMP x_ts, cv::Mat& xr_output);
	StreamImage* mp_connectedImage;
	std::map<std::string, BufferImage> m_buffers;
	cv::Mat& m_content;

private:
	static log4cxx::LoggerPtr m_logger;
};

#endif
