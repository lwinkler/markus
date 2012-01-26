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
#include "Feature.h"

using namespace std;

Feature::Feature(/*const string& x_name, */double x_value)
{
	//sprintf(m_name,"%s", x_name);
	//printf("name %s %s\n",x_name, m_name);
	m_value = x_value;
	m_variance = 0.1; // TODO : Default variance
}

Feature::Feature(const Feature& f)
{
	//strcpy(m_name, f.GetName());
	m_value=f.GetValue();
	m_variance = f.GetVariance();
};

Feature&  Feature::operator = (const Feature& f)
{
	//strcpy(m_name, f.GetName());
	m_value=f.GetValue();
	m_variance = f.GetVariance();

	return *this;
};

Feature::~Feature(){}


/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Get a value from a feature vector */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
double Feature::GetFeatureValue(const std::vector<Feature>& x_vect, const char* x_name)
{
	int cpt = 0;
	
	for ( vector<Feature>::const_iterator it1= x_vect.begin() ; it1 != x_vect.end(); it1++ )
	{
		if(! Feature::m_names.at(cpt).compare(x_name))// !strcmp((const char*) Feature::m_names.at(cpt).compare(x_name)/* it1->m_name*/, x_name))
			return it1->m_value;
		cpt++;
	}
	throw("GetFeatureValue : cannot find feature " + string(x_name));

}
