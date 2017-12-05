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

#include "StreamObject.h"
#include "util.h"

#define DEFAULT_STREAM_COLOR cv::Scalar(255, 255, 255)

using namespace std;
using namespace cv;

template<> const string StreamObject::m_class        = "StreamObjects";

template<> StreamObject::StreamT(const std::string& rx_name, vector<Object>& rx_object, Module& rx_module, const std::string& rx_description, const mkjson& rx_requirement) :
	Stream(rx_name, rx_module, rx_description, rx_requirement),
	m_content(rx_object)
{
	m_default.clear();
	m_default.push_back(Object("screen", Rect(Point(0, 0), GetSize())));
}

/// Convert the input to the right format
template<>void StreamObject::ConvertInput()
{
	if(m_connected == nullptr)
	{
		LOG_DEBUG(m_logger, "Object input not connected, use the whole image");
		return;
	}
	assert(m_connected->IsConnected());

	// Copy time stamp to output
	m_timeStamp = GetConnected().GetTimeStamp();

	const StreamObject * pstream = dynamic_cast<const StreamObject*>(m_connected);
	if(pstream == nullptr)
		throw MkException("Stream of objects " + GetName() + " is not correctly connected", LOC);
	vector<Object> rectsTarget = pstream->m_content;
	double ratioX = static_cast<double>(GetSize().width) / pstream->GetSize().width;
	double ratioY = static_cast<double>(GetSize().height) / pstream->GetSize().height;

	m_content.clear();
	for(const auto& elem : rectsTarget)
	{
		m_content.push_back(elem);
		Object& obj(m_content[m_content.size() - 1]);
		obj.posX   *= ratioX;
		obj.posY   *= ratioY;
		obj.width  *= ratioX;
		obj.height *= ratioY;
	}
}

/// Render : Draw rectangles on image

template<>void StreamObject::RenderTo(Mat& x_output) const
{
	if(x_output.size() != GetSize())
		throw MkException("Cannot render, image must have the same size as the stream", LOC);
	for(const auto& elem : m_content)
	{
		elem.RenderTo(x_output, DEFAULT_STREAM_COLOR);
	}
}

/// Query : give info about cursor position
template<>void StreamObject::Query(std::ostream& xr_out, const cv::Point& x_pt) const
{
	// check if out of bounds
	if(!Rect(Point(0, 0), GetSize()).contains(x_pt))
		return;

	for(const auto& elem : m_content)
		if(elem.GetRect().contains(x_pt))
		{
			mkjson json(elem);
			xr_out << multiLine(json) << endl;
			LOG_INFO(m_logger, multiLine(json));
		}

}

/// Randomize the content of the stream
template<>void StreamObject::Randomize(unsigned int& xr_seed)
{
	int minNb = 0;
	int maxNb = 10;

	if(!m_range.is_null())
	{
		minNb = m_range.value<int>("min", minNb);
		maxNb = m_range.value<int>("max", maxNb);
	}

	int nb = (maxNb - minNb) == 0 ? minNb : minNb + rand_r(&xr_seed) % (maxNb - minNb);
	m_content.clear();
	for(int i = 0 ; i < nb ; i++)
	{
		Object obj("random");
		obj.Randomize(xr_seed, m_range, GetSize());
		m_content.push_back(obj);
	}
}

template<>void StreamObject::Serialize(mkjson& rx_json, MkDirectory* xp_dir) const
{
	Stream::Serialize(rx_json, xp_dir);
	to_mkjson(rx_json["objects"], m_content);
}

template<>void StreamObject::Deserialize(const mkjson& x_json, MkDirectory* xp_dir)
{
	Stream::Deserialize(x_json, xp_dir);
	from_mkjson(x_json.at("objects"), m_content);
}
