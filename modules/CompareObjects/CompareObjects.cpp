/*----------------------------------------------------------------------------------
*
*    MARKUS : a manager for video analysis modules
* 
*    author : Loïc Monney <loic.monney@hefr.ch>
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
#include "CompareObjects.h"

#include "StreamObject.h"
#include "StreamDebug.h"
#include "MkException.h"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

log4cxx::LoggerPtr CompareObjects::m_logger(log4cxx::Logger::getLogger("CompareObjects"));

CompareObjects::CompareObjects(const ConfigReader &x_configReader) :
	Module(x_configReader),
	m_param(x_configReader)
{
	AddInputStream(0, new StreamObject("video1", m_objects1, *this, "Stream of objects 1"));
	AddInputStream(1, new StreamObject("video2", m_objects2, *this, "Stream of objects 2"));
}

CompareObjects::~CompareObjects() {}

void CompareObjects::Reset()
{
	Module::Reset();
	// m_frameCount = 0;
	// m_allErrors = 0;
}


void CompareObjects::ProcessFrame()
{
	if(m_objects1.size() != m_objects2.size())
	{
		LOG_ERROR(m_logger, "Objects vectors have different sizes : "<<m_objects1.size()<<" != "<<m_objects2.size());
		return;
	}
	vector<Object>::const_iterator it2 = m_objects2.begin();

	// Compare the list of objects
	for(vector<Object>::const_iterator it1 = m_objects1.begin() ; it1 != m_objects1.end() ; it1++,it2++)
	{
		// Check that names and ids match
		if(it1->GetName() != it2->GetName())
			LOG_ERROR(m_logger, "Object "<<it1->GetName()<<" do not match with "<<it2->GetName());
		if(it1->GetId() != it2->GetId())
			LOG_ERROR(m_logger, "Object "<<it1->GetName()<<it1->GetId()<<" do not match with "<<it2->GetName()<<it2->GetId());
		

		// Sum the differences for each object
		double sum = 0;
		int cpt = 0;
		for(map<string, FeaturePtr>::const_iterator itfeat = it1->GetFeatures().begin() ; itfeat != it1->GetFeatures().end() ; itfeat++)
		{
			const Feature& feat(it2->GetFeature(itfeat->first));
			double val = itfeat->second->Compare2(feat);

			// If dissimilarity is higher or equal to one, raise an error anyway. 
			// This means that features are too different
			if(val >= 1)
				LOG_ERROR(m_logger, "Feature "<<itfeat->first<<" dissimilarty of object "<<it1->GetName()<<it1->GetId()<<" is too high: "<<val);
			LOG_DEBUG(m_logger, "Feature "<<itfeat->first<<" dissimilarity of object "<<it1->GetName()<<it1->GetId()<<"="<<val);
			sum += val;
			cpt++;
		}
		sum = sqrt(sum) / cpt;

		if(sum > m_param.threshold)
			LOG_ERROR(m_logger, "Object "<<it1->GetName()<<it1->GetId()<<" dissimilarity is higher that threshold: "<<sum<<" > "<<m_param.threshold);
		LOG_DEBUG(m_logger, "Object "<<it1->GetName()<<it1->GetId()<<" dissimilarity="<<sum);
	}
}

