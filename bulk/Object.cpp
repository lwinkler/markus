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
using namespace cv;

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

Object::Object(const string& x_name, const Rect& x_rect) :
        m_name(x_name)
{
	m_posX 	 = x_rect.x + x_rect.width / 2;
	m_posY 	 = x_rect.y + x_rect.height / 2;
	m_width	 = x_rect.width;
	m_height = x_rect.height;
}

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

