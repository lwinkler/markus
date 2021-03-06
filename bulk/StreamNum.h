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

#include <sstream>
#include <boost/circular_buffer.hpp>
#include "Stream.h"
#include "ParameterT.h"
#include "feature_util.h"
#include "cvplot.h"

#define PLOT_LENGTH 50

namespace mk {
/// Class for a stream of images (or video) used for input and output
template<typename T>
class StreamNum : public Stream
{
public:
	friend inline void to_json(mkjson& _json, const StreamNum<T>& _ser){_ser.Serialize(_json);}
	friend inline void from_json(const mkjson& _json, StreamNum<T>& _ser){_ser.Deserialize(_json);}

	StreamNum(const std::string& x_name, T& rx_scalar, Module& rx_module, const std::string& x_description, const mkjson& rx_requirement = nullptr) :
		Stream(x_name, rx_module, x_description, rx_requirement),
		m_scalars(PLOT_LENGTH),
		m_content(rx_scalar) {}
	const std::string& GetClass() const override {return className;}

	void ConvertInput() override
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
	void RenderTo(cv::Mat& x_output) const override
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
	void Query(std::ostream& xr_out, const cv::Point& x_pt) const override
	{
		xr_out << m_content << std::endl;
		LOG_INFO(m_logger, "value = " << m_content);
	}
	virtual void Serialize(mkjson& rx_json, MkDirectory* xp_dir = nullptr) const
	{
		Stream::Serialize(rx_json, xp_dir);
		to_mkjson(rx_json["value"], m_content);
	}
	virtual void Deserialize(const mkjson& x_json, MkDirectory* xp_dir = nullptr)
	{
		Stream::Deserialize(x_json, xp_dir);
		from_mkjson(x_json.at("value"), m_content);
	}
	void Randomize(unsigned int& xr_seed) override {randomize(m_content, xr_seed);}
	const T& GetScalar() const {return m_content;}

	void SetValue(const mkconf& x_value, ParameterConfigType x_confType) override
	{
		m_content = x_value.get<T>();
		m_confSource = x_confType;
	}
	void SetDefault(const mkconf& x_value) override{
		m_default = x_value.template get<T>();
	}
	void SetValueToDefault() override{m_content = m_default; m_confSource = PARAMCONF_DEF;}
	mkconf GetValue() const override{return m_content;}
	mkconf GetDefault() const override {return m_default;}

	static const std::string className;

protected:
	T& m_content;
	T  m_default = T{};
	// circular buffer for plot rendering
	boost::circular_buffer<float> m_scalars;
};

} // namespace mk
#endif
