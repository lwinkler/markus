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
#include "Object.h"
#include "util.h"
#include <jsoncpp/json/writer.h>
#include <jsoncpp/json/reader.h>

using namespace cv;
using namespace std;

Object::Object(const string& x_name) :
	m_name(x_name),
	m_id(-1),
	posX(0),
	posY(0),
	width(0),
	height(0)
{
};

Object::Object(const string& x_name, const cv::Rect& x_rect) :
        m_name(x_name)
{
	posX 	 = x_rect.x + x_rect.width / 2;
	posY 	 = x_rect.y + x_rect.height / 2;
	width	 = x_rect.width;
	height   = x_rect.height;
}

Object::Object(const Object & x_obj)
{
	m_name = x_obj.GetName();
	m_id = x_obj.GetId();
	posX = x_obj.posX;
	posY = x_obj.posY;
	width = x_obj.width;
	height = x_obj.height;
	// for(map<string, FeaturePtr>::const_iterator it = x_obj.GetFeatures().begin() ; it != x_obj.GetFeatures().end() ; it++)
		// m_feats.insert(std::make_pair(it->first, new FeatureFloat(*it->second)));
	m_feats = x_obj.GetFeatures();
}

Object& Object::operator=(const Object & x_obj)
{
	m_name = x_obj.GetName();
	m_id = x_obj.GetId();
	posX = x_obj.posX;
	posY = x_obj.posY;
	width = x_obj.width;
	height = x_obj.height;

	m_feats.clear();

	// for(map<string, FeaturePtr>::const_iterator it = x_obj.GetFeatures().begin() ; it != x_obj.GetFeatures().end() ; it++)
		// m_feats.insert(std::make_pair(it->first, FeaturePtr(new Feature(*it->second))));
	m_feats = x_obj.GetFeatures();
	return *this;
}

Object::~Object()
{
	m_feats.clear();
}

void Object::Serialize(std::ostream& x_out, const string& x_dir) const
{
	Json::Value root;
	root["id"]     = m_id;
	root["name"]   = m_name;
	root["x"]      = posX;
	root["y"]      = posY;
	root["width"]  = width;
	root["height"] = height;

	for(map <std::string, FeaturePtr>::const_iterator it = m_feats.begin() ; it != m_feats.end() ; it++)
	{
		stringstream ss;
		it->second.Serialize(ss, x_dir);
		root["features"][it->first] = ss.str();
	}

	x_out << root;
}

void Object::Deserialize(std::istream& x_in, const string& x_dir)
{
	Json::Value root;
	x_in >> root;

	m_id   = root["id"].asInt();
	m_name = root["name"].asString();
	posX   = root["x"].asDouble();
	posY   = root["y"].asDouble();
	width  = root["width"].asDouble();
	height = root["height"].asDouble();

	m_feats.clear();
	Json::Value::Members members = root["features"].getMemberNames();
	for(Json::Value::Members::const_iterator it = members.begin() ; it != members.end() ; it++)
		AddFeature(*it, root["features"][*it].asFloat());
}


/**
* @brief Draw an object on an image (for visualization)
*
* @param x_output Image to draw the object on
* @param x_color  Color to use
*/
void Object::RenderTo(Mat& x_output, const Scalar& x_color) const
{
	//Rect rect = it1->GetRect();
	Point p1(posX - width / 2, posY - height / 2);
	Point p2(posX + width / 2, posY + height / 2);

	// Draw the rectangle in the input image
	// if id is present, draw to the equivalent color
#ifndef MARKUS_DEBUG_STREAMS
	rectangle(x_output, p1, p2, Scalar(20,0,230), 3, 8, 0 );
#else
	Scalar color = x_color;
	Point pText = p1;

	if(GetId() >= 0)
	{
		color = colorFromId(GetId());
		ostringstream text;
		text<<GetName()<<" "<<GetId();
		pText.y -= 3;
		putText(x_output, text.str(), pText,  FONT_HERSHEY_COMPLEX_SMALL, 0.4, color);
	}
	else
	{
		// color from stream
		pText.y -= 3;
		putText(x_output, GetName(), pText, FONT_HERSHEY_COMPLEX_SMALL, 0.4, color);
	}
	rectangle(x_output, p1, p2, color, 1, 8, 0 );

	// Print features and values
	pText.x += 2;
	int i = 0;
	for(map<string, FeaturePtr>::const_iterator it2 = GetFeatures().begin() ; it2 != GetFeatures().end() ; it2++)
	{
		//try
		{
			ostringstream text;
			text << it2->first << "=" << *it2->second;
			pText.y += 7;
			putText(x_output, text.str(), pText,  FONT_HERSHEY_COMPLEX_SMALL, 0.4, color);
			i++;
		}
		//catch(...){}
	}
#endif
}
