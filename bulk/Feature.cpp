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
