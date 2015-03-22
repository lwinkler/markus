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
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>

using namespace std;
using namespace cv;

log4cxx::LoggerPtr StreamObject::m_logger(log4cxx::Logger::getLogger("StreamObject"));

StreamObject::StreamObject(const string& rx_name, vector<Object>& xr_objects, Module& rx_module, const string& rx_description, const string& rx_requirement):
	Stream(rx_name, rx_module, rx_description, rx_requirement),
	m_objects(xr_objects)
{}

StreamObject::~StreamObject()
{

}

/// Convert the input to the right format

void StreamObject::ConvertInput()
{
	if(m_connected == nullptr)
	{
		// LOG_DEBUG(m_logger, "Object input not connected, use the whole image");
		m_objects.clear();
		m_objects.push_back(Object("screen", Rect(0, 0, GetWidth(), GetHeight())));
		return;
	}

	// Copy time stamp to output
	m_timeStamp = GetConnected().GetTimeStamp();

	const StreamObject * pstream = dynamic_cast<const StreamObject*>(m_connected);
	if(pstream == nullptr)
		throw MkException("Stream of objects " + GetName() + " is not correctly connected", LOC);
	vector<Object> rectsTarget = pstream->m_objects;
	double ratioX = static_cast<double>(m_width) / pstream->GetWidth();
	double ratioY = static_cast<double>(m_height) / pstream->GetHeight();

	m_objects.clear();
	for(const auto& elem : rectsTarget)
	{
		m_objects.push_back(elem);
		Object& obj(m_objects[m_objects.size() - 1]);
		obj.posX   *= ratioX;
		obj.posY   *= ratioY;
		obj.width  *= ratioX;
		obj.height *= ratioY;
	}
}

/// Render : Draw rectangles on image

void StreamObject::RenderTo(Mat& x_output) const
{
	if(x_output.cols != m_width || x_output.rows != m_height)
		throw MkException("Cannot render, image must have the same size as the stream", LOC);
	for(const auto& elem : m_objects)
	{
		elem.RenderTo(x_output, DEFAULT_STREAM_COLOR);
	}
}

/// Query : give info about cursor position
void StreamObject::Query(int x_posX, int x_posY) const
{
	// check if out of bounds
	if(x_posX < 0 || x_posY < 0 || x_posX >= GetWidth() || x_posY >= GetHeight())
		return;

	Point pt(x_posX, x_posY);

	for(const auto& elem : m_objects)
		if(elem.GetRect().contains(pt))
			LOG_INFO(m_logger, elem);
	
}

/// Randomize the content of the stream
void StreamObject::Randomize(unsigned int& xr_seed)
{
	m_objects.clear();
	int nb = rand_r(&xr_seed) % 10;
	for(int i = 0 ; i < nb ; i++)
	{
		Object obj("random");
		obj.Randomize(xr_seed, m_requirement, Size(GetWidth(), GetHeight()));
		m_objects.push_back(obj);
	}
}

void StreamObject::Serialize(ostream& x_out, const string& x_dir) const
{
	Json::Value root;
	stringstream ss;
	Stream::Serialize(ss, x_dir);
	ss >> root;
	int cpt = 0;

	if(m_objects.size() == 0)
		root["objects"] = Json::Value(Json::arrayValue); // Empty array

	// Serialize vector of objects
	for(const auto& elem : m_objects)
	{
		ss.clear();
		elem.Serialize(ss, x_dir);
		ss >> root["objects"][cpt];
		cpt++;
	}
	x_out << root;
}

void StreamObject::Deserialize(istream& x_in, const string& x_dir)
{
	Json::Value root;
	x_in >> root;  // note: copy first for local use
	stringstream ss;
	ss << root;
	Stream::Deserialize(ss, x_dir);

	Clear();
	for(const auto & elem : root["objects"])
	{
		ss.clear();
		Object obj("empty");
		AddObject(obj);
		ss << elem;
		m_objects.back().Deserialize(ss, x_dir);
	}
}
