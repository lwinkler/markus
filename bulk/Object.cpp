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

using namespace std;

Object::Object(const string& x_name) :
	m_name(x_name),
	m_id(-1),
	m_posX(0),
	m_posY(0),
	m_width(0),
	m_height(0)
	//m_color(cvScalar(255,255,255))
{
};

/*
Object::Object(const Object& r)
{
	//m_num = r.GetNum();
	m_feats = r.GetFeatures();
	m_posX = r.m_posX;
	m_posY = r.m_posY;
}

Object& Object::operator = (const Object& r)
{
	//m_num = r.GetNum();
	m_feats = r.GetFeatures();
	m_posX = r.m_posX;
	m_posY = r.m_posY;

	return *this;
}
*/
Object::~Object(){}

const Feature& Object::GetFeature(int x_index) const
{
	return m_feats.at(x_index);
}

/// Search a feature by name by using the list of feature names (probably from StreamObject)
const Feature& Object::GetFeatureByName(const std::string& x_name, const vector<std::string>& x_featureNames) const
{
	vector<std::string>::const_iterator names = x_featureNames.begin();
	
	if(m_feats.size() > x_featureNames.size())
		throw("Error: feature vector length is not equal to the length of feature names vector in Object::GetFeatureByName");

	for(vector<Feature>::const_iterator it = m_feats.begin() ; it != m_feats.end() ; it++)
	{
		if(names->compare(x_name) == 0)
		{
			return *it;
		}
		names++;
	}
	throw("Error: feature not found in Object::GetFeatureByName");
}

void Object::SetFeatureByName(const std::string& x_name, const std::vector<std::string>& x_featureNames, double x_value)
{
	vector<std::string>::const_iterator names = x_featureNames.begin();
	
	if(m_feats.size() > x_featureNames.size())
		throw("Error: feature vector length is not equal to the length of feature names vector in Object::GetFeatureByName");

	for(vector<Feature>::iterator it = m_feats.begin() ; it != m_feats.end() ; it++)
	{
		if(names->compare(x_name) == 0)
		{
			it->value = x_value;
			return;
		}
		names++;
	}
	throw("Error: feature not found in Object::GetFeatureByName");
}
