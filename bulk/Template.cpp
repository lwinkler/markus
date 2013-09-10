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
#include "Template.h"

using namespace std;

#define POW2(x) (x) * (x)

int Template::m_counter = 0;


Template::Template(int x_maxNbFramesDisappearance)
{
	m_num = m_counter;
	m_bestMatchingObject = -1;
	m_counterClean = x_maxNbFramesDisappearance;
	m_posX = 0;
	m_posY = 0;

	m_counter++;
}

Template::Template(const Template& t, int x_maxNbFramesDisappearance)
{
	m_num = t.GetNum();
	m_matchingObjects = t.GetMatchingObjects();
	m_feats = t.GetFeatures();
	m_posX = t.m_posX;
	m_posY = t.m_posY;

	m_bestMatchingObject = -1;
	m_counterClean = x_maxNbFramesDisappearance;

}

Template::Template(const Object& x_reg, int x_maxNbFramesDisappearance)
{
	m_num = m_counter;
	m_counter++;
	m_feats = x_reg.GetFeatures();
	m_posX = x_reg.m_posX;
	m_posY = x_reg.m_posY;
	m_bestMatchingObject = -1;
	m_counterClean = x_maxNbFramesDisappearance;

	//cout<<"Object "<<x_reg.GetNum()<<" is used to create template "<<m_num<<" with "<<x_reg.GetFeatures().size()<<" features"<<endl;
}

Template& Template::operator = (const Template& t)
{
	m_num = t.GetNum();
	m_matchingObjects = t.GetMatchingObjects();
	m_feats = t.GetFeatures();
	m_posX = t.m_posX;
	m_posY = t.m_posY;
	
	m_bestMatchingObject = -1;
	m_counterClean = t.m_counterClean;// ::m_maxNbFramesDisappearance;

	return *this;
}

Template::~Template()
{
	
}


/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Compare a candidate region with the template */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/

double Template::CompareWithObject(const Object& x_reg) const
{
	double sum = 0;
	//cout<<"m_feats.size() ="<<m_feats.size()<<endl;
	assert(m_feats.size() == x_reg.GetFeatures().size());
	assert(m_feats.size() > 0);
	
	for ( unsigned int i=0 ; i < m_feats.size() ; i++)
	{
		//cout<<"m_feats[i].GetValue()"<<m_feats[i].GetValue()<<endl;
		//cout<<"x_reg.GetFeatures()[i].GetValue()"<<x_reg.GetFeatures()[i].GetValue()<<endl;
		
		//cout<<"temp val ="<<m_feats[i].GetValue()<<" region val="<<x_reg.GetFeatures()[i].GetValue()<<" temp var="<<m_feats[i].GetVariance()<<endl;
		sum += POW2(m_feats[i].GetValue() - x_reg.GetFeatures()[i].GetValue()) 
			/ POW2(m_feats[i].GetSqVariance());
	}
	return sqrt(sum) / m_feats.size();
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Update the template's features based on the latest 50 matching regions */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/

void Template::UpdateFeatures()
{
	for ( unsigned int i=0 ; i < m_feats.size() ; i++)
	{
		double mean=0;
		double sqstdev=0;
		int size = m_matchingObjects.size();
		if (size <= 0) return;
		
		//cout<<"Updating template "<<m_num<<" with "<<m_matchingObjects.size()<<" matching regions."<<endl;
		
		for ( list<Object>::iterator it1= m_matchingObjects.begin() ; it1 != m_matchingObjects.end(); it1++ )
			mean += it1->GetFeatures().at(i).GetValue();
		mean /= size;
		
		for ( list<Object>::iterator it1= m_matchingObjects.begin() ; it1 != m_matchingObjects.end(); it1++ )
			sqstdev += (it1->GetFeatures().at(i).GetValue() - mean) * (it1->GetFeatures().at(i).GetValue() - mean);
		sqstdev /= size;

		//cout<<"UpdateFeatures : "<<m_feats[i].GetValue()<<"->"<<mean<<endl;
		//cout<<"UpdateFeatures : "<<m_feats[i].GetVariance()<<"->"<<sqrt(sqstdev)<<endl;
		m_feats[i].SetValue(mean);
		m_feats[i].SetSqVariance(sqstdev<0.01 ? 0.01 : sqstdev);
	}
	
}
