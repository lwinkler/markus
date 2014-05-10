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
#ifdef MARKUS_DEBUG_STREAMS
	m_debug.setTo(0);
#endif
	MatchTemplates();
	CleanTemplates();
	DetectNewTemplates();
	UpdateTemplates();

}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* MatchTemplates : Match our object with the existing object templates */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void TrackerByFeatures::MatchTemplates()
{
	/*for(vector<Object>::iterator it2 = m_objects.begin() ; it2 != m_objects.end(); it2++ )
	{
		it2->isMatched = 0;
	}*/
	m_matched.clear();

	// Try to match each objects with a template
	for(list<Template>::iterator it1 = m_templates.begin() ; it1 != m_templates.end(); it1++ )
	{
		// it1->m_bestMatchingObject = -1;
		it1->m_lastMatchingObject = NULL;
		//int isMatched =
		MatchTemplate(*it1);
	}
	LOG_DEBUG(m_logger, "MatchTemplates : "<<m_templates.size()<<" templates and "<<m_objects.size()<<" objects.");
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/// Match the template with an object (blob)
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/

int TrackerByFeatures::MatchTemplate(Template& x_temp)
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
		return 0;
	if(bestDist <= m_param.maxMatchingDistance)
	{
		if(m_param.symetricMatch)
		{
			const Template * bestTemplate = MatchObject(*bestObject);
			assert(bestTemplate != NULL);
			if(bestTemplate == NULL || bestTemplate->GetNum() != x_temp.GetNum())
				return 0;
		}
		// x_temp.m_bestMatchingObject = bestObject;
		LOG_DEBUG(m_logger, "Template "<<x_temp.GetNum()<<" matched with object "<<bestObject->GetId()<<" dist="<<bestDist);
		bestObject->SetId(x_temp.GetNum()); // Set id of object
		// x_temp.m_matchingObjects.push_back(m_objects[bestObject]);
		x_temp.m_lastMatchingObject = bestObject;
		m_matched[bestObject] = true;
		//bestObject->isMatched = 1;

		return 1;
	}
	else
	{
		return 0;
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
		double dist = it1->CompareWithObject(x_obj, m_featureNames);
		//cout<<"dist ="<<dist;
		if(dist < bestDist)
		{
			bestDist = dist;
			bestTemp = &(*it1);
		}
	}
	return bestTemp;
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
#ifdef MARKUS_DEBUG_STREAMS
		// Draw matching object
		if(it1->m_lastMatchingObject != NULL)
			rectangle(m_debug, it1->m_lastMatchingObject->Rect(), colorFromId(it1->GetNum()));
#endif
		if(it1->m_lastMatchingObject != NULL)
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
			it1->UpdateFeatures(m_param.alpha, m_currentTimeStamp);
			it1->m_lastMatchingObject->SetFeatures(it1->GetFeatures());
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
		if(!m_matched[&(*it2)])
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
					// TODO: Manage cases where x is absent
					// See if the object might have splitted
					try
					{
						double xt = bestTemplate->GetFeature("x").value;
						double yt = bestTemplate->GetFeature("y").value;
						double wt = bestTemplate->GetFeature("width").value;
						double ht = bestTemplate->GetFeature("height").value;
						double xo = it2->GetFeature("x").value;
						double yo = it2->GetFeature("y").value;

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
			//cout<<"Added template "<<t.GetNum()<<endl;
			it2->SetId(template1.GetNum());
			cpt++;
		}
	}
	LOG_DEBUG(m_logger, "DetectNewTemplates : "<<cpt<<" new templates added.");
}

