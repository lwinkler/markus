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

#include "StreamEvent.h"
#include "util.h"
#include "Manager.h"

using namespace std;
using namespace cv;

template<> log4cxx::LoggerPtr StreamEvent::m_logger(log4cxx::Logger::getLogger("StreamEvent"));
template<> const string StreamEvent::m_type         = "Event";
template<> const string StreamEvent::m_class        = "StreamEvent";
template<> const ParameterType StreamEvent::m_parameterType = PARAM_STREAM_EVENT;

// Transmit the event to the connected module

template<> void StreamEvent::ConvertInput()
{
	m_content.Clean();

	if(m_connected == nullptr) return;
	assert(m_connected->IsConnected());

	// Copy time stamp to output
	m_timeStamp = GetConnected().GetTimeStamp();

	const StreamEvent * pstream = dynamic_cast<const StreamEvent*>(m_connected);
	if(pstream == nullptr)
		throw MkException("Stream of event " + GetName() + " is not correctly connected", LOC);
	m_content = pstream->GetContent();

	if(! m_content.IsRaised()) return;

	m_content.ScaleObject(ratio(GetSize(), pstream->GetSize()));
}

/// Randomize the content of the stream
template<> void StreamEvent::Randomize(unsigned int& xr_seed)
{
	// random event
	m_content.Randomize(xr_seed, m_range, GetSize());
}

/// Render : to display the event
template<> void StreamEvent::RenderTo(Mat& x_output) const
{
	if(m_content.IsRaised())
	{
		x_output.setTo(Scalar(255, 255, 255));
		m_content.GetObject().RenderTo(x_output, Scalar(255, 0, 0));
	}
	// else x_output.setTo(0);
}

/// Query : give info about cursor position
template<> void StreamEvent::Query(std::ostream& xr_out, const cv::Point& x_pt) const
{
	mkjson json(m_content);
	xr_out << multiLine(json) << endl;
	LOG_INFO(m_logger, multiLine(json));
}

template<> void StreamEvent::Serialize(mkjson& rx_json, MkDirectory* xp_dir) const
{
	Stream::Serialize(rx_json, xp_dir);
	to_mkjson(rx_json["event"], m_content);
}

template<> void StreamEvent::Deserialize(const mkjson& x_json, MkDirectory* xp_dir)
{
	Stream::Deserialize(x_json, xp_dir);
	from_mkjson(x_json.at("event"), m_content);
}
