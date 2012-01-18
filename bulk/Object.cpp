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
Object::~Object(){};

