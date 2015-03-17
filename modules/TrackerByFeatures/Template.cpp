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
log4cxx::LoggerPtr Template::m_logger(log4cxx::Logger::getLogger("Template"));

void copyFeaturesToTemplate(const map<string, FeaturePtr>& x_source, map<string, FeatureFloatInTime>& xr_dest)
{
	xr_dest.clear();
	for(const auto & elem : x_source)
	{
		const FeatureFloat* const ff = dynamic_cast<const FeatureFloat* const>(&*elem.second);
		// Skip all other features
		if(ff == nullptr)
			continue;
		xr_dest.insert(std::make_pair(elem.first, FeatureFloatInTime(*ff)));
	}
}

Template::Template()
{
	m_num = m_counter;
	m_lastMatchingObject = nullptr;
	m_lastSeen = TIME_STAMP_MIN;

	m_counter++;
}

Template::Template(const Template& t)
{
	m_num = t.GetNum();
	m_lastMatchingObject = t.m_lastMatchingObject;
	m_feats = t.GetFeatures();
	m_lastSeen = t.m_lastSeen;
}

Template::Template(const Object& x_obj, TIME_STAMP x_currentTimeStamp)
{
	m_num = m_counter;
	m_counter++;
	copyFeaturesToTemplate(x_obj.GetFeatures(), m_feats);
	m_lastMatchingObject = nullptr; // &x_obj;
	m_lastSeen = x_currentTimeStamp;

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
	if(m_feats.empty())
		throw MkException("Feature vector of Template cannot be empty", LOC);

	for (const auto & x_feature : x_features)
	{
		/*
		const FeatureFloatInTime& f1(GetFeature(*it));
		const FeatureFloat&       f2(dynamic_cast<const FeatureFloat &>(x_obj.GetFeature(*it)));
		sum += POW2(f1.value - f2.value)
			   / POW2(f1.sqVariance);
			   */
		sum += GetFeature(x_feature).CompareSquared(x_obj.GetFeature(x_feature));
	}
	// cout<<sqrt(sum) / x_features.size()<<endl;
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
	if(m_lastMatchingObject != nullptr)
	{
		for (auto it = m_feats.begin() ; it != m_feats.end() ; ++it)
		{
			try
			{
				it->second.Update(m_lastMatchingObject->GetFeature(it->first), x_alpha);
				LOG_DEBUG(m_logger, "Update feature "<<it->first<<" of template  mean: "<<it->second.mean<<", init: "<<it->second.initial<<", current value: "<<it->second.value);
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
