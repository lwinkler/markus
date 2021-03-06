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

namespace mk {
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
	friend inline void to_json(mkjson& _json, const StreamT& _ser){_ser.Serialize(_json);}
	friend inline void from_json(const mkjson& _json, StreamT& _ser){_ser.Deserialize(_json);}

	StreamT(const std::string& x_name, cv::Mat& x_image, Module& rx_module, const std::string& x_description, const std::string& x_requirements = "");
	const std::string& GetClass() const override {return className;}

	void ConvertInput() override;
	void RenderTo(cv::Mat& x_output) const override;
	void Query(std::ostream& xr_out, const cv::Point& x_pt) const override;
	void Serialize(mkjson& rx_json, MkDirectory* xp_dir = nullptr) const;
	void Deserialize(const mkjson& x_json, MkDirectory* xp_dir = nullptr);
	void Randomize(unsigned int& xr_seed) override;
	const cv::Mat& GetImage() const {return m_content;}
	void Connect(Stream& xr_stream) override;
	void Disconnect() override;

	void SetValue(const mkconf& x_value, ParameterConfigType x_confType) override
	{
		LOG_WARN(m_logger, "Impossible to set the value of a stream of type image as a parameter");
		// m_confSource = x_confType;
	}
	void SetDefault(const mkconf& x_value) override {LOG_WARN(m_logger, "Impossible to set the default value of a stream of type image as a parameter");}
	void SetValueToDefault() override {m_content.setTo(0); m_confSource = PARAMCONF_DEF;};
	// note: This will not work with images
	mkconf GetValue() const override
	{
		// note: for simplicity, we only serialize the size
		mkjson root;
		root["width"] = m_content.cols;
		root["height"] = m_content.rows;
		return root;
	}
	mkconf GetDefault() const override
	{
		mkjson root;
		root["width"] = m_content.cols;
		root["height"] = m_content.rows;
		return root;
	}

	static const std::string className;

protected:
	static std::string createResolutionString(const cv::Size x_size, int x_depth, int x_channels)
	{
		std::stringstream ss;
		ss << x_size.width << "x" << x_size.height << "_" << x_depth << "_" << x_channels;
		return ss.str();
	}
	void ConvertToOutput(TIME_STAMP x_ts, cv::Mat& xr_output);
	StreamImage* mp_connectedImage = nullptr;
	std::map<std::string, BufferImage> m_buffers;
	cv::Mat& m_content;
};

} // namespace mk
#endif
