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
	// m_bestMatchingObject = -1;
	m_lastMatchingObject = NULL;
	m_counterClean = x_maxNbFramesDisappearance;

	m_counter++;
}

Template::Template(const Template& t, int x_maxNbFramesDisappearance)
{
	m_num = t.GetNum();
	m_lastMatchingObject = NULL; //  t.GetMatchingObjects();
	m_feats = t.GetFeatures();

	// m_bestMatchingObject = -1;
	m_counterClean = x_maxNbFramesDisappearance;

}

Template::Template(const Object& x_reg, int x_maxNbFramesDisappearance)
{
	m_num = m_counter;
	m_counter++;
	m_feats = x_reg.GetFeatures();
	// m_bestMatchingObject = -1;
	m_lastMatchingObject = NULL;
	m_counterClean = x_maxNbFramesDisappearance;

	//cout<<"Object "<<x_reg.GetNum()<<" is used to create template "<<m_num<<" with "<<x_reg.GetFeatures().size()<<" features"<<endl;
}

Template& Template::operator = (const Template& t)
{
	m_num = t.GetNum();
	m_lastMatchingObject = NULL; // t.GetMatchingObjects();
	m_feats = t.GetFeatures();
	
	// m_bestMatchingObject = -1;
	m_counterClean = t.m_counterClean;// ::m_maxNbFramesDisappearance;

	return *this;
}

Template::~Template()
{
	
}


/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Compare a candidate region with the template */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/

double Template::CompareWithObject(const Object& x_reg, const vector<string>& x_features) const
{
	double sum = 0;
	//cout<<"m_feats.size() ="<<m_feats.size()<<endl;
	assert(m_feats.size() >= x_reg.GetFeatures().size());
	assert(m_feats.size() > 0);
	
	for (vector<string>::const_iterator it = x_features.begin() ; it != x_features.end() ; it++)
	{
		const Feature & f1(GetFeature(*it));
		const Feature & f2(x_reg.GetFeature(*it));
		//cout<<"m_feats[i].GetValue()"<<m_feats[i].GetValue()<<endl;
		//cout<<"x_reg.GetFeatures()[i].GetValue()"<<x_reg.GetFeatures()[i].GetValue()<<endl;
		
		//cout<<"temp val ="<<m_feats[i].GetValue()<<" region val="<<x_reg.GetFeatures()[i].GetValue()<<" temp var="<<m_feats[i].GetVariance()<<endl;
		sum += POW2(f1.value - f2.value) 
			/ POW2(f1.sqVariance); // TODO: See if sqVariance has a reasonable value !!
	}
	return sqrt(sum) / x_features.size();
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Update the template's features */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/

void Template::UpdateFeatures(double x_alpha)
{
	if(m_lastMatchingObject != NULL)
	{
		for (map<string,Feature>::iterator it = m_feats.begin() ; it != m_feats.end() ; it++)
		{
			it->second.Update(m_lastMatchingObject->GetFeature(it->first).value, x_alpha);
		}
	}
	
}
