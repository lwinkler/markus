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


StreamObject::StreamObject(int x_id, const string& x_name, 
		vector<Object>& xr_objects, const Scalar& x_color, Module& rx_module, const string& rx_description) :
	Stream(x_id, x_name, DEFAULT_STREAM_WIDTH, DEFAULT_STREAM_HEIGHT, rx_module, rx_description),
	m_objects(xr_objects),
	m_color(x_color),
	m_isColorSet(true)
{
}

StreamObject::StreamObject(int x_id, const string& rx_name, 
		vector<Object>& xr_objects, Module& rx_module, const string& rx_description):
	Stream(x_id, rx_name, DEFAULT_STREAM_WIDTH, DEFAULT_STREAM_HEIGHT, rx_module, rx_description),
	m_objects(xr_objects),
	m_color(cvScalar(255, 255, 255)),
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

void StreamObject::RenderTo(Mat * xp_output) const
{
	for(vector<Object>::const_iterator it1 = m_objects.begin() ; it1 != m_objects.end() ; it1++)
	{
		it1->RenderTo(xp_output, m_color);
	}
}


void StreamObject::PrintObjects() const
{
	//cout<<"print region name="<<m_objects.at(0).GetFeatures().at(0).m_name<<endl;
	for ( vector<Object>::const_iterator it1= m_objects.begin() ; it1 < m_objects.end(); it1++ )
	{
		int cpt=0;
		cout<<"Object "/*<<(int)it1->GetNum()*/<<" : ";
		for ( map<string, Feature>::const_iterator it2 = it1->GetFeatures().begin() ; it2 != it1->GetFeatures().end(); it2++ )
		{
			cout<<" "<<it2->first<<"="<<it2->second.value<<"|";
			cpt++;
		}
		cout<<endl;
	}
}

