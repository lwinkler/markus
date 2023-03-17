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

#include "StreamState.h"
#include "util.h"

namespace mk {
using namespace std;
using namespace cv;

template<> const string StreamState::className        = "StreamState";

// Transmit the state to the connected module

template<> void StreamState::ConvertInput()
{
	if(m_connected == nullptr)
	{
		m_content = false;
		return;
	}
	assert(m_connected->IsConnected());

	// Copy time stamp to output
	m_timeStamp = GetConnected().GetTimeStamp();

	const StreamState * pstream = dynamic_cast<const StreamState*>(m_connected);
	if(pstream == nullptr)
		throw MkException("Stream of state " + GetName() + " is not correctly connected", LOC);
	m_content = pstream->GetContent();
}

/// Render : to display the state we simply color the image in black/white

template<> void StreamState::RenderTo(Mat& x_output) const
{
	x_output.setTo(Scalar(255 * m_content, 255 * m_content, 255 * m_content));
}

/// Query : give info about cursor position
template<> void StreamState::Query(std::ostream& xr_out, const cv::Point& x_pt) const
{
	// check if out of bounds
	if(!Rect(Point(0, 0), GetSize()).contains(x_pt))
		return;
	
	xr_out << static_cast<int>(m_content) << endl;
	LOG_INFO(m_logger, "State =" << static_cast<int>(m_content));
}

/// Randomize the content of the stream
template<> void StreamState::Randomize(unsigned int& xr_seed)
{
	// random state
	// TODO if(rand_r(&xr_seed) < RAND_MAX / 10)
		m_content = !m_content;
}

template<> void StreamState::Serialize(mkjson& rx_json, MkDirectory* xp_dir) const
{
	Stream::Serialize(rx_json, xp_dir);
	to_mkjson(rx_json["state"], m_content);
}

template<> void StreamState::Deserialize(const mkjson& x_json, MkDirectory* xp_dir)
{
	Stream::Deserialize(x_json, xp_dir);
	from_mkjson(x_json.at("state"), m_content);
}
} // namespace mk
