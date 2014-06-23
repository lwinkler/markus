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
log4cxx::LoggerPtr Template::m_logger(log4cxx::Logger::getLogger("Module"));

void copyFeaturesToTemplate(const map<string, FeaturePtr>& x_source, map<string, FeatureFloatInTime>& xr_dest)
{
	xr_dest.clear();
	for(map<string,FeaturePtr>::const_iterator it = x_source.begin() ; it != x_source.end() ; it++)
	{
		const FeatureFloat* const ff = dynamic_cast<const FeatureFloat* const>(&*it->second);
		// Skip all other features
		if(ff == NULL)
			continue;
		xr_dest.insert(std::make_pair(it->first, FeatureFloatInTime(*ff)));
	}
}

Template::Template()
{
	m_num = m_counter;
	m_lastMatchingObject = NULL;
	m_lastSeen = TIME_STAMP_MIN;

	m_counter++;
}

Template::Template(const Template& t)
{
	m_num = t.GetNum();
	m_lastMatchingObject = t.m_lastMatchingObject;
	m_feats = t.GetFeatures();
	m_lastSeen = TIME_STAMP_MIN;
}

Template::Template(const Object& x_obj)
{
	m_num = m_counter;
	m_counter++;
	copyFeaturesToTemplate(x_obj.GetFeatures(), m_feats);
	m_lastMatchingObject = NULL; // &x_obj;
	m_lastSeen = TIME_STAMP_MIN;

	//cout<<"Object "<<x_obj.GetNum()<<" is used to create template "<<m_num<<" with "<<x_obj.GetFeatures().size()<<" features"<<endl;
}

Template& Template::operator = (const Template& t)
{
	m_num = t.GetNum();
	m_lastMatchingObject = t.m_lastMatchingObject;
	m_feats = t.GetFeatures();
	m_lastSeen = t.m_lastSeen;// ::m_timeDisappear;

	return *this;
}

Template::~Template()
{
	
}

/**
* @brief Compare a candidate object with the template 
*
* @param x_obj      Object for comparison
* @param x_features Vector of features to compare
*
* @return 
*/
double Template::CompareWithObject(const Object& x_obj, const vector<string>& x_features) const
{
	double sum = 0;
	//cout<<"m_feats.size() ="<<m_feats.size()<<endl;
	//assert(m_feats.size() >= x_obj.GetFeatures().size());
	if(m_feats.size() <= 0)
		throw MkException("Feature vector of Template cannot be empty", LOC);
	
	for (vector<string>::const_iterator it = x_features.begin() ; it != x_features.end() ; it++)
	{
		const FeatureFloatInTime& f1(GetFeature(*it));
		const FeatureFloat&       f2(dynamic_cast<const FeatureFloat &>(x_obj.GetFeature(*it)));
		sum += POW2(f1.value - f2.value) 
			/ POW2(f1.sqVariance); // TODO: See if sqVariance has a reasonable value !!
		// sum += POW2(f1.mean - f2.value) 
			// / POW2(f1.sqVariance);
	}
	return sqrt(sum) / x_features.size();
}

/**
* @brief Update the template's features
*
* @param x_alpha            Alpha coefficient for lowpass filter
* @param m_currentTimeStamp Current time
*/
void Template::UpdateFeatures(double x_alpha, TIME_STAMP m_currentTimeStamp)
{
	if(m_lastMatchingObject != NULL)
	{
		for (map<string,FeatureFloatInTime>::iterator it = m_feats.begin() ; it != m_feats.end() ; it++)
		{
			try
			{
				it->second.Update(m_lastMatchingObject->GetFeature(it->first), x_alpha);
			}
			catch(FeatureNotFoundException& e)
			{
				// Note: info level only to avoid polluting unit test logs
				LOG_INFO(m_logger, "Exception in UpdateFeatures: " << e.what());
			}
		}
		m_lastSeen = m_currentTimeStamp;
	}
}

/**
* @brief This template needs to be cleaned
*
* @param x_cleaningTimeStamp Time at which the template should be cleaned
*
* @return true if it needs to be cleaned
*/
bool Template::NeedCleaning(TIME_STAMP x_cleaningTimeStamp)
{
	int tmp = m_lastSeen - x_cleaningTimeStamp; // note: this condition would not resist an overflow
	return tmp < 0;
}
