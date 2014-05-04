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


StreamObject::StreamObject(const string& rx_name, vector<Object>& xr_objects, Module& rx_module, const string& rx_description):
	Stream(rx_name, rx_module, rx_description),
	m_objects(xr_objects)
{}

StreamObject::~StreamObject()
{

}

/// Convert the input to the right format

void StreamObject::ConvertInput()
{
	if(m_connected == NULL) return;

	// Copy time stamp to output
	m_timeStamp = GetConnected().GetTimeStamp();

	const StreamObject * pstream = dynamic_cast<const StreamObject*>(m_connected);
	if(pstream == NULL) return;
	std::vector<Object> rectsTarget = pstream->m_objects;
	double ratioX = static_cast<double>(m_width) / pstream->GetWidth();
	double ratioY = static_cast<double>(m_height) / pstream->GetHeight();

	m_objects.clear();
	for(vector<Object>::const_iterator it = rectsTarget.begin() ; it != rectsTarget.end() ; it++)
	{
		m_objects.push_back(*it);
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
	for(vector<Object>::const_iterator it1 = m_objects.begin() ; it1 != m_objects.end() ; it1++)
	{
		it1->RenderTo(x_output, DEFAULT_STREAM_COLOR);
	}
}


void StreamObject::Serialize(std::ostream& x_out, const string& x_dir) const
{
	Json::Value root;
	stringstream ss;
	Stream::Serialize(ss, x_dir);
	ss >> root;
	int cpt = 0;

	if(m_objects.size() == 0)
		root["objects"] = Json::Value(Json::arrayValue); // Empty array
	for(vector<Object>::const_iterator it1 = m_objects.begin() ; it1 != m_objects.end() ; it1++)
	{
		ss.clear();
		it1->Serialize(ss, x_dir);
		ss >> root["objects"][cpt];
		cpt++;
	}
	x_out << root;
}

void StreamObject::Deserialize(std::istream& x_in, const string& x_dir)
{
	// TODO
}
