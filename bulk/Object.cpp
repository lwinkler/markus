#include "Object.h"


Object::Object()
{
	//m_num = x_num;//m_counter;
	//m_counter++;
};

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

Object::~Object(){};

