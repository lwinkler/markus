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
#include "cvplot.h"
#include <sstream>
#include <boost/circular_buffer.hpp>

#define PLOT_LENGTH 50

/// Class for a stream of images (or video) used for input and output
template<typename T>
class StreamNum : public Stream
{
public:
	StreamNum(const std::string& x_name, T& rx_scalar, Module& rx_module, const std::string& x_description, const std::string& rx_requirement = "") :
		Stream(x_name, rx_module, x_description, rx_requirement),
		m_scalars(PLOT_LENGTH),
		m_content(rx_scalar) {}
	virtual const std::string& GetClass() const {return m_class;}
	virtual const std::string& GetType() const {return m_type;}
	virtual const ParameterType& GetParameterType() const {return m_parameterType;}

	virtual void ConvertInput()
	{
		if(m_connected == nullptr)
		{
			m_content = 0;
			return;
		}
		assert(m_connected->IsConnected());

		// Copy time stamp to output
		m_timeStamp = GetConnected().GetTimeStamp();
		m_content = dynamic_cast<const StreamNum&>(*m_connected).GetScalar();
	}
	/// Method to be called at the end of each step to store the last point for the plot
	inline void Store() {m_scalars.push_back(m_content);}
	virtual void RenderTo(cv::Mat& x_output) const
	{
		std::vector<float> pts;
		for(auto elem : m_scalars)
			pts.push_back(elem);
		x_output.setTo(255);
		CvPlot::Figure fig("test");
		CvPlot::Series s(pts);
		s.SetColor(cv::Scalar(22,0,255), false);
		fig.Add(s);
		fig.Initialize();
		fig.DrawAxis(x_output);
		fig.DrawPlots(x_output);
		fig.DrawLabels(x_output, x_output.cols - 100, 10);
	}
	virtual void Query(std::ostream& xr_out, const cv::Point& x_pt) const
	{
		xr_out << m_content << std::endl;
		LOG_INFO(m_logger, "value = " << m_content);
	}
	virtual void Serialize(std::ostream& x_out, MkDirectory* xp_dir = nullptr) const
	{
		Json::Value root;
		std::stringstream ss;
		Stream::Serialize(ss, xp_dir);

		ss >> root;
		root["value"] = m_content;
		x_out << root;
	}
	virtual void Deserialize(std::istream& x_in, MkDirectory* xp_dir = nullptr)
	{
		Json::Value root;
		x_in >> root;  // note: copy first for local use
		std::stringstream ss;
		ss << root;
		Stream::Deserialize(ss, xp_dir);
		m_content = root["value"].asDouble();
	}
	virtual void Randomize(unsigned int& xr_seed) {randomize(m_content, xr_seed);}
	const T& GetScalar() const {return m_content;}

	virtual void SetValue(const ConfigReader& x_value, ParameterConfigType x_confType)
	{
		// TODO std::stringstream ss(x_value);
		// TODO deserialize(ss, m_content);
		// TODO m_confSource = x_confType;
	}
	virtual void SetDefault(const ConfigReader& x_value){
		//TODO std::stringstream ss(x_value);
		//TODO deserialize(ss, m_content);
	}
	virtual void SetValueToDefault(){m_content = m_default; m_confSource = PARAMCONF_DEF;}
	virtual ConfigReader GetValue() const{std::stringstream ss; serialize(ss, m_content); return ss.str();}
	virtual ConfigReader GetDefault() const {std::stringstream ss; serialize(ss, m_default); return ss.str();}

protected:
	T& m_content;
	T  m_default = T{};
	// circular buffer for plot rendering
	boost::circular_buffer<float> m_scalars;
	static const std::string m_class;
	static const std::string m_type;
	static const ParameterType m_parameterType;

private:
	static log4cxx::LoggerPtr m_logger;
};

#endif
