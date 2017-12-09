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

namespace mk {
using namespace cv;
using namespace std;

log4cxx::LoggerPtr CompareObjects::m_logger(log4cxx::Logger::getLogger("CompareObjects"));

CompareObjects::CompareObjects(ParameterStructure &xr_params) :
	Module(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params))
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
	vector<Object>::const_iterator itObj2 = m_objects2.begin();

	// Compare the list of objects
	for(const auto& obj1 : m_objects1)
	{
		// Check that names and ids match
		if(obj1.GetName() != itObj2->GetName())
			LOG_ERROR(m_logger, "Object "<<obj1.GetName()<<" does not match with "<<itObj2->GetName());
		if(obj1.GetId() != itObj2->GetId())
			LOG_ERROR(m_logger, "Object "<<obj1.GetName()<<obj1.GetId()<<" does not match with "<<itObj2->GetName()<<itObj2->GetId());


		// Sum the differences for each object
		double sum = 0;
		int cpt = 0;
		//cout<<"obj ";obj1.Serialize(cout, "");
		for(const auto& elemFeat : obj1.GetFeatures())
		{
			try
			{
				const Feature& feat(itObj2->GetFeature(elemFeat.first));
				// cout<<"compare feat "<<elemFeat.first<<": "<<*elemFeat.second<<" to "<<feat<<endl;
				double val = elemFeat.second->CompareSquared(feat);

				// If dissimilarity is higher or equal to one, raise an error anyway.
				// This means that features are too different
				if(val >= 1)
					LOG_ERROR(m_logger, "Feature "<<elemFeat.first<<" dissimilarty of object "<<obj1.GetName()<<obj1.GetId()<<" is too high: "<<val);
				LOG_DEBUG(m_logger, "Feature "<<elemFeat.first<<" dissimilarity of object "<<obj1.GetName()<<obj1.GetId()<<"="<<val);
				sum += val;
				cpt++;
			}
			catch(exception& e)
			{
				LOG_ERROR(m_logger, "Error at feature comparison: " << e.what());
			}
		}
		sum = sqrt(sum) / cpt;

		if(sum > m_param.threshold)
			LOG_ERROR(m_logger, "Object "<<obj1.GetName()<<obj1.GetId()<<" dissimilarity is higher that threshold: "<<sum<<" > "<<m_param.threshold);
		LOG_DEBUG(m_logger, "Object "<<obj1.GetName()<<obj1.GetId()<<" dissimilarity="<<sum);

		++itObj2;
	}
}


} // namespace mk
