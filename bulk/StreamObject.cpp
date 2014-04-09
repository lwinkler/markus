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

using namespace std;
using namespace cv;


StreamObject::StreamObject(const string& rx_name, vector<Object>& xr_objects, Module& rx_module, const string& rx_description):
	Stream(rx_name, rx_module, rx_description),
	m_objects(xr_objects),
	// m_color(cvScalar(255, 255, 255)),
	m_isColorSet(false)
{}

StreamObject::~StreamObject()
{

}

/// Convert the input to the right format

void StreamObject::ConvertInput()
{
	if(m_connected == NULL) return;

	// Copy time stamp to output
	m_timeStamp = RefConnected().GetTimeStamp();

	const StreamObject * pstream = dynamic_cast<const StreamObject*>(m_connected);
	if(pstream == NULL) return;
	std::vector<Object> rectsTarget = pstream->m_objects;
	double ratioX = static_cast<double>(m_width) / pstream->GetInputWidth();
	double ratioY = static_cast<double>(m_height) / pstream->GetInputHeight();

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


/// Write the stream content to a directory
void StreamObject::WriteToDirectory(const std::string x_directory) const
{
	ofstream of;
	string fileName = x_directory + "/" + GetModule().GetName() + "." + GetName() + ".objects.txt";
	of.open(fileName.c_str());
	stringstream ss;
	of << m_objects.at(0).Serialize(ss).str(); // TODO: Serialize all objects
	of.close();
}
