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
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>

using namespace std;
using namespace cv;

template<> log4cxx::LoggerPtr StreamState::m_logger(log4cxx::Logger::getLogger("StreamState"));
template<> const string StreamState::m_type         = "State";
template<> const string StreamState::m_class        = "StreamState";

// Transmit the state to the connected module

template<> void StreamState::ConvertInput()
{
	if(m_connected == nullptr)
	{
		m_object = false;
		return;
	}
	assert(m_connected->IsConnected());

	// Copy time stamp to output
	m_timeStamp = GetConnected().GetTimeStamp();

	const StreamState * pstream = dynamic_cast<const StreamState*>(m_connected);
	if(pstream == nullptr)
		throw MkException("Stream of state " + GetName() + " is not correctly connected", LOC);
	m_object = pstream->GetObject();
}

/// Render : to display the state we simply color the image in black/white

template<> void StreamState::RenderTo(Mat& x_output) const
{
	x_output.setTo(Scalar(255 * m_object, 255 * m_object, 255 * m_object));
}

/// Query : give info about cursor position
template<> void StreamState::Query(int x_posX, int x_posY) const
{
	// check if out of bounds
	if(x_posX < 0 || x_posY < 0 || x_posX >= GetWidth() || x_posY >= GetHeight())
		return;
	
	LOG_INFO(m_logger, "State =" << static_cast<int>(m_object));
}

/// Randomize the content of the stream
template<> void StreamState::Randomize(unsigned int& xr_seed)
{
	// random state
	if(rand_r(&xr_seed) < RAND_MAX / 10)
		m_object = !m_object;
}

template<> void StreamState::Serialize(ostream& x_out, const string& x_dir) const
{
	Json::Value root;
	stringstream ss;
	Stream::Serialize(ss, x_dir);
	ss >> root;
	root["state"] = m_object;
	x_out << root;
}

template<> void StreamState::Deserialize(istream& x_in, const string& x_dir)
{
	Json::Value root;
	x_in >> root;  // note: copy first for local use
	stringstream ss;
	ss << root;
	Stream::Deserialize(ss, x_dir);

	m_object = root["state"].asBool();
}
