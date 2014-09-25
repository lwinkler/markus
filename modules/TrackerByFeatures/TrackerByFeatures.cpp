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
	for(map<string,FeatureFloatInTime>::const_iterator it3 = x_temp.GetFeatures().begin() ; it3 != x_temp.GetFeatures().end() ; it3++)
	{
		cout<<"name "<<it3->first<<endl;
		cout<<"name "<<it3->second.SerializeToString()<<endl;
		x_obj.AddFeature(it3->first, it3->second.CreateCopy());
	}
}

TrackerByFeatures::TrackerByFeatures(const ConfigReader& x_configReader) :
	Module(x_configReader),
	m_param(x_configReader)
{
	AddInputStream(0, new StreamObject("input",      m_objects, *this,       "Input objects"));

	AddOutputStream(0, new StreamObject("tracker", m_objects, *this, "Tracked objects"));

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
	cout<<"Process\n";
#ifdef MARKUS_DEBUG_STREAMS
	m_debug.setTo(0);
#endif
	MatchTemplates();
	CleanTemplates();
	DetectNewTemplates();
	UpdateTemplates();
	UpdateObjects();
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* MatchTemplates : Match all templates with our objects */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void TrackerByFeatures::MatchTemplates()
{
	m_matched.clear();

	// Try to match each objects with a template
	for(list<Template>::iterator it1 = m_templates.begin() ; it1 != m_templates.end(); it1++ )
	{
		// it1->m_bestMatchingObject = -1;
		it1->m_lastMatchingObject = NULL;
		MatchTemplate(*it1);
	}

	/*for(vector<Object>::iterator it1 = m_objects.begin() ; it1 != m_objects.end(); it1++ )
	{
		Template* bestTemplate = MatchObject(*it1);
		if(bestTemplate == NULL)
			continue;

		if(m_param.symetricMatch)
		{
			Object* bestObject = MatchTemplate(*bestTemplate);
			// assert(bestTemplate != NULL);
			if(bestObject == NULL || bestObject != reinterpret_cast<Object*>(&*it1))
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
	cout<<"Objects"<<m_objects.size()<<endl;
	for(vector<Object>::iterator it1 = m_objects.begin() ; it1 != m_objects.end(); it1++)
	{
		std::map<Object*, Template*>::iterator it = m_matched.find(&*it1);
		assert(it != m_matched.end());
		updateObjectFromTemplate(*it->second, *it1);
	}
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/// Match the template with an object (blob)
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/

Object* TrackerByFeatures::MatchTemplate(Template& x_temp)
{
	double bestDist = DBL_MAX;
	Object* bestObject = NULL;
	x_temp.m_lastMatchingObject = NULL;

	LOG_DEBUG(m_logger, "Comparing template "<<x_temp.GetNum()<<" with "<<m_objects.size()<<" objects");

	for(vector<Object>::iterator it = m_objects.begin() ; it != m_objects.end() ; it++)
	{
		// Add empty features for distance and speed
		double dist = x_temp.CompareWithObject(*it, m_featureNames);
		if(dist < bestDist)
		{
			bestDist   = dist;
			bestObject = &(*it);
		}
	}
	if(bestObject == NULL)
		return NULL;
	if(bestDist <= m_param.maxMatchingDistance)
	{
		if(m_param.symetricMatch)
		{
			const Template * bestTemplate = MatchObject(*bestObject);
			assert(bestTemplate != NULL);
			if(bestTemplate == NULL || bestTemplate->GetNum() != x_temp.GetNum())
				return NULL;
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
		return NULL;
	}
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/// Match an object with the set of templates
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
const Template * TrackerByFeatures::MatchObject(const Object& x_obj)const
{
	double bestDist = DBL_MAX;
	const Template* bestTemp = NULL;

	//cout<<"Comparing template "<<m_num<<" with "<<x_regs.size()<<" objects"<<endl;

	for(list<Template>::const_iterator it1 = m_templates.begin() ; it1 != m_templates.end(); it1++ )
	{
		// cout<<"Match template "<<it1->GetNum()<<endl;
		double dist = it1->CompareWithObject(x_obj, m_featureNames);
		//cout<<"dist ="<<dist;
		if(dist < bestDist)
		{
			bestDist = dist;
			bestTemp = &(*it1);
		}
	}
	// TODO: Compensate matching distance and alpha param with time between frames
	if(bestDist <= m_param.maxMatchingDistance)
		return bestTemp;
	else return NULL;
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* UpdateTemplates */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void TrackerByFeatures::UpdateTemplates()
{
#ifdef MARKUS_DEBUG_STREAMS
	const double diagonal = sqrt(m_param.width * m_param.width + m_param.height * m_param.height);
#endif

	for(list<Template>::iterator it1= m_templates.begin() ; it1 != m_templates.end(); it1++ )
	{
		// cout<<"Update template "<<it1->GetNum()<<endl;
#ifdef MARKUS_DEBUG_STREAMS
		// Draw matching object
		if(it1->m_lastMatchingObject != NULL)
			rectangle(m_debug, it1->m_lastMatchingObject->Rect(), colorFromId(it1->GetNum()));
#endif
		if(it1->m_lastMatchingObject != NULL)
		{
			// Add two extra features: distance and speed /// TODO
			// const Feature& x = it1->m_lastMatchingObject->GetFeature("x");
			// const Feature& y = it1->m_lastMatchingObject->GetFeature("y");


			// distance
			// it1->m_lastMatchingObject->SetFeatureByName("distance", featureNames, DIST(x.value - x.initial, y.value - y.initial));
			// speed
			// it1->m_lastMatchingObject->SetFeatureByName("speed_x", featureNames, x.value - x.mean);
			// it1->m_lastMatchingObject->SetFeatureByName("speed_y", featureNames, y.value - y.mean);

			// Update the template and copy to the object
			it1->UpdateFeatures(m_param.alpha, m_currentTimeStamp);
			// it1->m_lastMatchingObject->SetFeatures(it1->GetFeatures());
			it1->m_lastMatchingObject = NULL;
		}

#ifdef MARKUS_DEBUG_STREAMS
		// draw template (if position is available)
		try{
			double x = it1->GetFeature("x").mean * diagonal;
			double y = it1->GetFeature("y").mean * diagonal;
			// double w = it1->GetFeature("width").value * diagonal;
			// double h = it1->GetFeature("height").value * diagonal;
			Point p(x, y);
			// Size s(w * m_param.width / 2, h * m_param.height / 2);
			// ellipse(*m_debug, p, s, 0, 0, 360, colorFromId(it1->GetNum()));
			circle(m_debug, p, 4, colorFromId(it1->GetNum()));
		}
		catch(...){}
#endif
	}
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* CleanTemplates */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void TrackerByFeatures::CleanTemplates()
{
	int cptCleaned = 0;
	int cptTotal = 0;
	TIME_STAMP timeStampClean = m_currentTimeStamp - m_param.timeDisappear * 1000;
	for(list<Template>::iterator it1 = m_templates.begin() ; it1 != m_templates.end(); it1++ )
	{
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
	for(vector<Object>::iterator it2 = m_objects.begin() ; it2 != m_objects.end(); it2++ )
	{
		map<Object*, Template*>::iterator it3 = m_matched.find(&(*it2));
		if(it3 == m_matched.end() || it3->second == NULL)
		{
			if(m_templates.size() >= MAX_NB_TEMPLATES)
			{
				LOG_WARN(m_logger, "Too many templates created in TrackerByFeatures::DetectNewTemplates");
				// return; // Note: not a fatal error
			}

			Template template1(*it2);
			// if(bestDist <= m_param.maxMatchingDistance && bestTemplate != NULL)

			// note: We may want to inherit this class and create an AdvancedTracker !
			if(m_param.handleSplit)
			{
				// Detect if the new object is similar to a template
				double bestDist = DBL_MAX;
				const Template * bestTemplate = NULL;

				LOG_DEBUG(m_logger, "New object. Detect if the new object similar to another template ");

				for(list<Template>::iterator it3 = m_templates.begin() ; it3 != m_templates.end() ; it3++)
				{
					// Add empty features for distance and speed
					double dist = it3->CompareWithObject(*it2, m_featureNames);
					if(dist < bestDist)
					{
						bestDist     = dist;
						bestTemplate = &(*it3);
					}
				}

				if(bestTemplate != NULL)
				{
					// See if the object might have splitted
					try
					{
						double xt = bestTemplate->GetFeature("x").value;
						double yt = bestTemplate->GetFeature("y").value;
						double wt = bestTemplate->GetFeature("width").value;
						double ht = bestTemplate->GetFeature("height").value;
						double xo = dynamic_cast<const FeatureFloat&>(it2->GetFeature("x")).value;
						double yo = dynamic_cast<const FeatureFloat&>(it2->GetFeature("y")).value;

						// Condition for object split
						// note: so far the distance to consider a split is the double of w/2 . This is a security margin
						if(abs(xt - xo) <= wt / 2 && abs(yt - yo) <= ht / 2)
						{
							// Copy the template to the object (but not the id)
							template1.SetFeatures(bestTemplate->GetFeatures());
						}
					}
					catch(...){}
				}
			}

			template1.m_lastMatchingObject = &(*it2);
			m_templates.push_back(template1);
			Template* newTemp = &m_templates.back();
			m_matched[&(*it2)] = newTemp;
			//cout<<"Added template "<<t.GetNum()<<endl;
			// updateObjectFromTemplate(*newTemp, *it2);
			LOG_DEBUG(m_logger, "Added new template " << it2->GetId());
			cpt++;
		}
	}
	LOG_DEBUG(m_logger, "DetectNewTemplates : "<<cpt<<" new templates added.");
}

