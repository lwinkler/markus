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
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>

using namespace std;
using namespace cv;

log4cxx::LoggerPtr StreamEvent::m_logger(log4cxx::Logger::getLogger("StreamEvent"));

StreamEvent::StreamEvent(const string& x_name, Event& x_event, Module& rx_module, const string& rx_description) :
	Stream(x_name, rx_module, rx_description),
	m_event(x_event)
{
}


StreamEvent::~StreamEvent()
{

}

// Transmit the event to the connected module

void StreamEvent::ConvertInput()
{
	if(m_connected == nullptr) return;

	// Copy time stamp to output
	m_timeStamp = GetConnected().GetTimeStamp();

	const StreamEvent * pstream = dynamic_cast<const StreamEvent*>(m_connected);
	if(pstream == nullptr)
		throw MkException("Stream of event " + GetName() + " is not correctly connected", LOC);
	m_event = pstream->GetEvent();

	if(! m_event.IsRaised()) return;

	double ratioX = static_cast<double>(m_width) / pstream->GetWidth();
	double ratioY = static_cast<double>(m_height) / pstream->GetHeight();
	m_event.ScaleObject(ratioX, ratioY);
}

/// Randomize the content of the stream
void StreamEvent::Randomize(unsigned int& xr_seed)
{
	// random event
	m_event.Randomize(xr_seed, m_requirement, GetSize());
}

/// Render : to display the event
void StreamEvent::RenderTo(Mat& x_output) const
{
	if(m_event.IsRaised())
	{
		x_output.setTo(Scalar(255, 255, 255));
		m_event.GetObject().RenderTo(x_output, Scalar(255, 0, 0));
	}
	// else x_output.setTo(0);
}

/// Query : give info about cursor position
void StreamEvent::Query(int x_posX, int x_posY) const
{
	// check if out of bounds
	if(x_posX < 0 || x_posY < 0 || x_posX >= GetWidth() || x_posY >= GetHeight())
		return;
	
	LOG_ERROR(m_logger, "TODO");
}

void StreamEvent::Serialize(ostream& x_out, const string& x_dir) const
{
	Json::Value root;
	stringstream ss;
	Stream::Serialize(ss, x_dir);
	ss >> root;
	ss.clear();
	m_event.Serialize(ss, x_dir);
	ss >> root["event"];
	x_out << root;
}

void StreamEvent::Deserialize(istream& x_in, const string& x_dir)
{
	Json::Value root;
	x_in >> root;  // note: copy first for local use

	stringstream ss;
	ss << root;
	Stream::Deserialize(ss, x_dir);

	ss.clear();
	ss << root["event"];
	m_event.Deserialize(ss, x_dir);
}
