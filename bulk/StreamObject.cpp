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


StreamObject::StreamObject(int x_id, const string& x_name, int x_width, int x_height, 
		       vector<Object>& xr_objects, const CvScalar& x_color, Module& rx_module, const string& rx_description) : 
	Stream(x_id, x_name, STREAM_IMAGE, x_width, x_height, rx_module, rx_description),
	m_objects(xr_objects),
	m_color(x_color),
	m_isColorSet(true)
{
}

StreamObject::StreamObject(int x_id, const string& rx_name, int x_width, int x_height, 
			vector<Object>& xr_objects, Module& rx_module, const string& rx_description):
	Stream(x_id, rx_name, STREAM_IMAGE, x_width, x_height, rx_module, rx_description),
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
		obj.m_posX *= ratioX;
		obj.m_posY *= ratioY;
		obj.m_width *= ratioX;
		obj.m_height *= ratioY;
	}
}

/// Render : Draw rectangles on image

void StreamObject::RenderTo(Mat * xp_output) const
{
	for(vector<Object>::const_iterator it = m_objects.begin() ; it != m_objects.end() ; it++)
	{
		//Rect rect = it->GetRect();
		Point p1(it->m_posX - it->m_width / 2, it->m_posY - it->m_height / 2);
		Point p2(it->m_posX + it->m_width / 2, it->m_posY + it->m_height / 2);
		
		float scale = static_cast<float>(xp_output->cols) / m_width;
		p1.x = p1.x * scale;
		p2.x = p2.x * scale;
		scale = static_cast<float>(xp_output->rows) / m_height;
		p1.y = p1.y * scale;
		p2.y = p2.y * scale;
		
		// Draw the rectangle in the input image
		// if id is present, draw to the equivalent color
		
		if(it->GetId() >= 0)
		{
			rectangle( *xp_output, p1, p2, colorFromId(it->GetId()), 1, 8, 0 );
			ostringstream text;
			text<<it->GetName()<<" "<<it->GetId();
			p1.y -= 2;
			putText(*xp_output, text.str(), p1,  FONT_HERSHEY_COMPLEX_SMALL, 0.5, colorFromId(it->GetId()));
		}
		else
		{
			// color from stream
			rectangle( *xp_output, p1, p2, m_color, 1, 8, 0 );
			p1.y -= 2;
			putText(*xp_output, it->GetName(), p1, FONT_HERSHEY_COMPLEX_SMALL, 0.5, m_color);
		}
        }
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/// Get a value from a feature vector
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
double StreamObject::GetFeatureValue(const std::vector< Feature >& x_vect, const char* x_name)
{
	int cpt = 0;
	
	for ( vector<Feature>::const_iterator it1= x_vect.begin() ; it1 != x_vect.end(); it1++ )
	{
		if(!m_featureNames.at(cpt).compare(x_name))// !strcmp((const char*) Feature::m_names.at(cpt).compare(x_name)/* it1->m_name* /, x_name))
			return it1->m_value;
		cpt++;
	}
	throw("GetFeatureValue : cannot find feature " + string(x_name));
}

void StreamObject::PrintObjects() const
{
	//cout<<"print region name="<<m_objects.at(0).GetFeatures().at(0).m_name<<endl;
	for ( vector<Object>::const_iterator it1= m_objects.begin() ; it1 < m_objects.end(); it1++ )
	{
		int cpt=0;
		cout<<"Object "/*<<(int)it1->GetNum()*/<<" : ";
		for ( vector<Feature>::const_iterator it2=it1->GetFeatures().begin() ; it2 < it1->GetFeatures().end(); it2++ )
		{
			cout<<" "<<m_featureNames.at(cpt)<<"="<<it2->GetValue()<<"|";
			cpt++;
		}
		cout<<endl;
	}	
}

