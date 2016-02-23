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

#include "TrackerByFeatures.h"
#include "StreamObject.h"
#include "StreamDebug.h"
#include "util.h"
#include "FeatureVector.h"

// for debug
// #include "util.h"
#define MAX_NB_TEMPLATES 256

using namespace std;
using namespace cv;

log4cxx::LoggerPtr TrackerByFeatures::m_logger(log4cxx::Logger::getLogger("TrackerByFeatures"));

/**
* @brief Update an object with the information contained in the template (features and id)
*
* @param x_temp Template
* @param x_obj  Object
*/
void updateObjectFromTemplate(const Template& x_temp, Object& x_obj)
{
	x_obj.SetId(x_temp.GetNum());

	// Copy features
	for(const auto & elem : x_temp.GetFeatures())
	{
		// cout<<"name "<<it3->first<<endl;
		// cout<<"name "<<it3->second.SerializeToString()<<endl;
		x_obj.AddFeature(elem.first, elem.second.CreateCopy());
	}
}

TrackerByFeatures::TrackerByFeatures(ParameterStructure& xr_params) :
	Module(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params))
{
	// Features required in input
	vector<string> feats;
	split(m_param.features, ',', feats);
	stringstream ss;
	ss << "{\"features\":{" << join(feats, ',', "\"%s\":{\"type\":\"FeatureFloat\"}") << "}}";

	AddInputStream(0, new StreamObject("input",      m_objects, *this, "Input objects", ss.str()));

	AddOutputStream(0, new StreamObject("tracker",   m_objects, *this, "Tracked objects"));

#ifdef MARKUS_DEBUG_STREAMS
	m_debug = Mat(Size(m_param.width, m_param.height), CV_8UC3);
	AddDebugStream(0, new StreamDebug("debug", m_debug, *this,	"Debug"));
#endif
}

TrackerByFeatures::~TrackerByFeatures(void )
{
}

void TrackerByFeatures::Reset()
{
	Module::Reset();
	split(m_param.features, ',', m_featureNames);
	m_templates.clear();
	m_objects.clear();
}

void TrackerByFeatures::ProcessFrame()
{
#ifdef MARKUS_DEBUG_STREAMS
	m_debug.setTo(0);
#endif
	MatchTemplates();
	CheckMergeSplit();    // before updating templates
	UpdateTemplates();
	DetectNewTemplates();
	UpdateObjects();      // after detecting new templates
	CleanTemplates();
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* MatchTemplates : Match all templates with our objects */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void TrackerByFeatures::MatchTemplates()
{
	m_matched.clear();

	// Try to match each objects with a template
	for(auto & elem : m_templates)
	{
		// it1->m_bestMatchingObject = -1;
		elem.m_lastMatchingObject = nullptr;
		MatchTemplate(elem);
	}

	/*for(vector<Object>::iterator it1 = m_objects.begin() ; it1 != m_objects.end(); it1++ )
	{
		Template* bestTemplate = MatchObject(*it1);
		if(bestTemplate == nullptr)
			continue;

		if(m_param.symetricMatch)
		{
			Object* bestObject = MatchTemplate(*bestTemplate);
			// assert(bestTemplate != nullptr);
			if(bestObject == nullptr || bestObject != reinterpret_cast<Object*>(&*it1))
				continue;
		}

		LOG_DEBUG(m_logger, "Template "<<bestTemplate->GetNum()<<" matched with object "<<it1->GetId());

		bestTemplate->m_lastMatchingObject = &*it1;
		m_matched[&*it1] = bestTemplate;
	}*/
	LOG_DEBUG(m_logger, "MatchTemplates : "<<m_templates.size()<<" templates and "<<m_objects.size()<<" objects.");
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/// Update the objects
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/

void TrackerByFeatures::UpdateObjects()
{
	for(auto & elem : m_objects)
	{
		auto it = m_matched.find(&elem);
		assert(it != m_matched.end());
		updateObjectFromTemplate(*it->second, elem);
#ifdef MARKUS_DEBUG_STREAMS
		// Draw matching object
		rectangle(m_debug, elem.GetRect(), colorFromId(elem.GetId()));
#endif
	}
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/// Match the template with an object (blob)
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/

Object* TrackerByFeatures::MatchTemplate(Template& x_temp)
{
	double bestDist = DBL_MAX;
	Object* bestObject = nullptr;
	x_temp.m_lastMatchingObject = nullptr;

	LOG_DEBUG(m_logger, "Comparing template "<<x_temp.GetNum()<<" with "<<m_objects.size()<<" objects");

	for(auto & elem : m_objects)
	{
		// Add empty features for distance and speed
		double dist = x_temp.CompareWithObject(elem, m_featureNames);
		if(dist < bestDist)
		{
			bestDist   = dist;
			bestObject = &(elem);
		}
	}
	if(bestObject == nullptr)
		return nullptr;
	if(bestDist <= m_param.maxMatchingDistance)
	{
		if(m_param.symetricMatch)
		{
			const Template * bestTemplate = MatchObject(*bestObject);
			assert(bestTemplate != nullptr);
			if(bestTemplate->GetNum() != x_temp.GetNum())
				return nullptr;
		}
		// x_temp.m_bestMatchingObject = bestObject;
		LOG_DEBUG(m_logger, "Template "<<x_temp.GetNum()<<" matched with object "<<bestObject->GetId()<<" dist="<<bestDist);
		// bestObject->SetId(x_temp.GetNum()); // Set id of object
		// x_temp.m_matchingObjects.push_back(m_objects[bestObject]);
		x_temp.m_lastMatchingObject = bestObject;
		m_matched[bestObject] = &x_temp;
		//bestObject->isMatched = 1;
		// updateObjectFromTemplate(x_temp, *bestObject);

		return bestObject;
	}
	else
	{
		return nullptr;
	}
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/// Match an object with the set of templates
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
const Template * TrackerByFeatures::MatchObject(const Object& x_obj)const
{
	double bestDist = DBL_MAX;
	const Template* bestTemp = nullptr;
	
	//cout<<"Comparing template "<<m_num<<" with "<<x_regs.size()<<" objects"<<endl;

	for(const auto& temp : m_templates)
	{
		// cout<<"Match template "<<temp.GetNum()<<endl;
		double dist = temp.CompareWithObject(x_obj, m_featureNames);
		//cout<<"dist ="<<dist;
		if(dist < bestDist)
		{
			bestDist = dist;
			bestTemp = &temp;
		}
	}
	// TODO: Compensate matching distance and alpha param with time between frames
	if(bestDist <= m_param.maxMatchingDistance)
		return bestTemp;
	else return nullptr;
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* UpdateTemplates */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void TrackerByFeatures::UpdateTemplates()
{
#ifdef MARKUS_DEBUG_STREAMS
#endif

	for(auto & elem : m_templates)
	{
		// cout<<"Update template "<<it1->GetNum()<<endl;
		if(elem.m_lastMatchingObject != nullptr)
		{
			// Add two extra features: distance and speed
			// const Feature& x = it1->m_lastMatchingObject->GetFeature("x");
			// const Feature& y = it1->m_lastMatchingObject->GetFeature("y");


			// distance
			// it1->m_lastMatchingObject->SetFeatureByName("distance", featureNames, DIST(x.value - x.initial, y.value - y.initial));
			// speed
			// it1->m_lastMatchingObject->SetFeatureByName("speed_x", featureNames, x.value - x.mean);
			// it1->m_lastMatchingObject->SetFeatureByName("speed_y", featureNames, y.value - y.mean);

			// Update the template and copy to the object
			elem.UpdateFeatures(m_param.alpha, m_currentTimeStamp);
			// it1->m_lastMatchingObject->SetFeatures(it1->GetFeatures());
			elem.m_lastMatchingObject = nullptr;
		}
	}
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* CleanTemplates */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void TrackerByFeatures::CleanTemplates()
{
	int cptCleaned = 0;
	int cptTotal = 0;
#ifdef MARKUS_DEBUG_STREAMS
	const double diagonal = sqrt(m_param.width * m_param.width + m_param.height * m_param.height);
#endif
	TIME_STAMP timeStampClean = m_currentTimeStamp - m_param.timeDisappear * 1000;

	for(auto it1 = m_templates.begin() ; it1 != m_templates.end(); ++it1)
	{
#ifdef MARKUS_DEBUG_STREAMS
		// draw template (if position is available)
		try
		{
			double x = it1->GetFeature("x").mean * diagonal;
			double y = it1->GetFeature("y").mean * diagonal;
			// double w = it1->GetFeature("width").value * diagonal;
			// double h = it1->GetFeature("height").value * diagonal;
			Point p(x, y);
			// Size s(w * m_param.width / 2, h * m_param.height / 2);
			// ellipse(*m_debug, p, s, 0, 0, 360, colorFromId(it1->GetNum()));
			circle(m_debug, p, 4, colorFromId(it1->GetNum()));
		}
		catch(...) {}
#endif
		if(it1->NeedCleaning(timeStampClean))
		{
			it1 = m_templates.erase(it1);
			cptCleaned++;
		}
		cptTotal++;
	}
	LOG_DEBUG(m_logger, "CleanTemplates : "<<cptCleaned<<" templates erased out of "<<cptTotal<<" templates");
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* DetectNewTemplates */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void TrackerByFeatures::DetectNewTemplates()
{
	// If objects not matched, add a template
	int cpt = 0;
	for(auto& obj : m_objects)
	{
		auto obj_matched = m_matched.find(&obj);
		if(obj_matched == m_matched.end() || obj_matched->second == nullptr)
		{
			if(m_templates.size() >= MAX_NB_TEMPLATES)
			{
				LOG_WARN(m_logger, "Too many templates created in TrackerByFeatures::DetectNewTemplates");
				// return; // Note: not a fatal error
			}

			// Create new template
			Template template1(obj, m_currentTimeStamp);
			// if(bestDist <= m_param.maxMatchingDistance && bestTemplate != nullptr)

			// note: We may want to inherit this class and create an AdvancedTracker !
			if(m_param.handleSplit)
			{
				// Detect if the new object is similar to a template
				double bestDist = DBL_MAX;
				// const Template * bestTemplate = nullptr;

				LOG_DEBUG(m_logger, "New object. Detect if the new object similar to another template ");

				for(const auto& temp : m_templates)
				{
					// Add empty features for distance and speed
					double dist = temp.CompareWithObject(obj, m_featureNames);
					if(dist < bestDist)
					{
						bestDist     = dist;
						// bestTemplate = &temp;
					}
				}
			}

			template1.m_lastMatchingObject = &obj;
			m_templates.push_back(template1);
			Template* newTemp = &m_templates.back();
			m_matched[&obj] = newTemp;
			//cout<<"Added template "<<t.GetNum()<<endl;
			// updateObjectFromTemplate(*newTemp, obj);
			LOG_DEBUG(m_logger, "Added new template " << obj.GetId());
			cpt++;
		}
	}
	LOG_DEBUG(m_logger, "DetectNewTemplates : "<<cpt<<" new templates added.");
}


/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Check if the objects have merged of split from previous templates */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void TrackerByFeatures::CheckMergeSplit()
{
	if(!m_param.handleSplit)
		return;

	vector<const Template*> templates;

	// Push all templates that were present on the current or the last frame 
	for(const auto& temp : m_templates)
	{
		if(temp.m_lastSeen >= m_lastTimeStamp)
			templates.push_back(&temp);
	}
	
	for(auto& obj : m_objects)
	{
		vector<int> merged;
		vector<int> split;
		for(const auto& ptemp : templates)
		{
			if(ptemp == m_matched[&obj])
				continue;

			try
			{
				double xt = ptemp->GetFeature("x").value;
				double yt = ptemp->GetFeature("y").value;
				double wt = ptemp->GetFeature("width").value;
				double ht = ptemp->GetFeature("height").value;
				double xo = dynamic_cast<const FeatureFloat&>(obj.GetFeature("x")).value;
				double yo = dynamic_cast<const FeatureFloat&>(obj.GetFeature("y")).value;
				double wo = dynamic_cast<const FeatureFloat&>(obj.GetFeature("width")).value;
				double ho = dynamic_cast<const FeatureFloat&>(obj.GetFeature("height")).value;

				if(wt*wt + ht*ht > wo*wo + ho*ho)
				{
					// Condition for object split
					if(abs(xt - xo) <= wt / 2 && abs(yt - yo) <= ht / 2)
					{
						split.push_back(ptemp->GetNum());
						// LOG_DEBUG(m_logger,"Objects split from "<<ptemp->GetNum());
					}
				}
				else
				{
					// Condition for object merge
					// note: so far the distance to consider a split is the double of w/2 . This is a security margin
					if(abs(xo - xt) <= wo / 2 && abs(yo - yt) <= ho / 2)
					{
						merged.push_back(ptemp->GetNum());
						// LOG_DEBUG(m_logger,"Objects merged with "<<ptemp->GetNum());
					}
				}
			}
			catch(exception& e) 
			{
				LOG_WARN(m_logger, "Exception in split/merge: "<< e.what());
			}
		}

		if(!merged.empty())
		{
			obj.AddFeature("merge", new FeatureVectorInt(merged));
			LOG_DEBUG(m_logger, "Object merged with "<< merged.size() << " templates");
		}
		if(!split.empty())
		{
			obj.AddFeature("merge", new FeatureVectorInt(split));
			LOG_DEBUG(m_logger, "Object split from "<< split.size() << " templates");
		}
	}
}
