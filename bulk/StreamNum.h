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

#ifndef STREAM_NUM_H
#define STREAM_NUM_H

#include "Stream.h"
#include "feature_util.h"
#include <sstream>

/// Class for a stream of images (or video) used for input and output
template<typename T>
class StreamNum : public Stream
{
public:
	StreamNum(const std::string& x_name, T& rx_scalar, Module& rx_module, const std::string& x_description) :
		Stream(x_name, rx_module, x_description),
		m_scalar(rx_scalar) {}
	virtual const std::string& GetClass() const {return m_class;}
	MKTYPE("Scalar")

	virtual void ConvertInput()
	{
		if(m_connected == nullptr)
		{
			m_scalar = 0;
			return;
		}
		// Copy time stamp to output
		m_timeStamp = GetConnected().GetTimeStamp();
		m_scalar = dynamic_cast<const StreamNum*>(m_connected)->GetScalar();
	}
	virtual void RenderTo(cv::Mat& x_output) const {}
	virtual void Query(int x_posX, int x_posY) const
	{
		LOG_INFO(m_logger, "TODO");
	}
	virtual void Serialize(std::ostream& x_out, const std::string& x_dir) const
	{
		Json::Value root;
		std::stringstream ss;
		Stream::Serialize(ss, x_dir);

		ss >> root;
		root["value"] = m_scalar;
		x_out << root;
	}
	virtual void Deserialize(std::istream& x_in, const std::string& x_dir)
	{
		Json::Value root;
		x_in >> root;  // note: copy first for local use
		std::stringstream ss;
		ss << root;
		Stream::Deserialize(ss, x_dir);
		m_scalar = root["value"].asDouble();
	}
	virtual void Randomize(unsigned int& xr_seed){randomize(m_scalar, xr_seed);}
	const T& GetScalar() const {return m_scalar;}

protected:
	T& m_scalar;
	static const std::string m_class;

private:
	DISABLE_COPY(StreamNum)
	static log4cxx::LoggerPtr m_logger;
};

#endif
