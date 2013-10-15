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

// for debug
#include "util.h"

using namespace cv;

const char * TrackerByFeatures::m_type = "TrackerByFeatures";

#define MAX_NB_TEMPLATES 100
#define DIST(x, y) sqrt((x) * (x) + (y) * (y))

using namespace std;
using namespace cv;



TrackerByFeatures::TrackerByFeatures(const ConfigReader& x_configReader) :
	Module(x_configReader),
	m_param(x_configReader)
{
	m_description = "Track objects by matching a set of features (typically x,y,width and height)";

	m_inputObjectStream = new StreamObject(0, "input", 	m_param.width, m_param.height, m_objects, cvScalar(255, 255, 255), *this,	"Input objects");
	m_inputStreams.push_back(m_inputObjectStream);

	m_outputObjectStream = new StreamObject(0, "tracker", m_param.width, m_param.height, m_objects, cvScalar(255, 255, 255), *this,	"Tracked objects");
	m_outputStreams.push_back(m_outputObjectStream);

	// TODO : Output template + check if ok in/out same

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
	for(vector<Object>::iterator it2 = m_objects.begin() ; it2 != m_objects.end(); it2++ )
	{
		it2->m_isMatched = 0;
	}

	// Try to match each region with a template
	for(list<Template>::iterator it1 = m_templates.begin() ; it1 != m_templates.end(); it1++ )
	{
		// it1->m_bestMatchingObject = -1;
		it1->m_lastMatchingObject = NULL;
		//int isMatched =
		MatchTemplate(*it1);
	}
	LOG_DEBUG("MatchTemplates : "<<m_templates.size()<<" templates and "<<m_objects.size()<<" regions.");
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/// Match the template with an object (blob)
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/

int TrackerByFeatures::MatchTemplate(Template& x_temp)
{
	double bestDist = DBL_MAX;
	Object* bestObject = NULL;

	LOG_DEBUG("Comparing template "<<x_temp.GetNum()<<" with "<<m_objects.size()<<" objects");

	for(vector<Object>::iterator it = m_objects.begin() ; it != m_objects.end() ; it++)
	{
		// it->SetId(-1); // See if we move this TODO
		// Add empty features for distance and speed
		double dist = x_temp.CompareWithObject(*it, m_featureNames);
		//cout<<"dist ="<<dist;
		if(dist < bestDist)
		{
			bestDist   = dist;
			bestObject = &(*it);
		}
	}
	if(bestObject == NULL)
		return 0;
	const Template * bestTemplate = MatchObject(*bestObject);
	if(bestDist < m_param.maxMatchingDistance
			&& (bestTemplate == &x_temp || !m_param.symetricMatch))
	{
		bestObject->SetId(bestTemplate->GetNum()); // Set id of object
		// x_temp.m_bestMatchingObject = bestObject;
		LOG_DEBUG("Template "<<x_temp.GetNum()<<" matched with object "<<bestObject->GetId()<<" dist="<<bestDist);
		// x_temp.m_matchingObjects.push_back(m_objects[bestObject]);
		x_temp.m_lastMatchingObject = bestObject;
		bestObject->m_isMatched = 1;

		//CvPoint p = {x_regs[bestObject].m_posX, x_regs[bestObject].m_posY};
		//cvCircle(x_blobsImg, p, 10, TrackerByFeatures::m_colorArray[m_num % TrackerByFeatures::m_colorArraySize]);

		return 1;
	}
	else return 0;

}


/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* UpdateTemplates */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void TrackerByFeatures::UpdateTemplates()
{
	for(list<Template>::iterator it1= m_templates.begin() ; it1 != m_templates.end(); it1++ )
	{
		if(it1->m_lastMatchingObject != NULL)
		{
			// Add two extra features: distance and speed
			// const Feature& x = it1->m_lastMatchingObject->GetFeature("x");
			// const Feature& y = it1->m_lastMatchingObject->GetFeature("y");


			// distance
			// it1->m_lastMatchingObject->SetFeatureByName("distance", featureNames, DIST(x.value - x.initial, y.value - y.initial));
			// speed
			// it1->m_lastMatchingObject->SetFeatureByName("speed_x", featureNames, x.value - x.mean); // TODO : This is experimental
			// it1->m_lastMatchingObject->SetFeatureByName("speed_y", featureNames, y.value - y.mean);

			// Update the template and copy to the object
			it1->UpdateFeatures(m_param.alpha);
			it1->m_lastMatchingObject->SetFeatures(it1->GetFeatures());
			it1->m_lastMatchingObject = NULL;
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
	for(list<Template>::iterator it1 = m_templates.begin() ; it1 != m_templates.end(); it1++ )
	{
		//cout<<"it1->m_isMatched"<<it1->m_isMatched<<endl;
		//cout<<"it1->m_counterClean"<<it1->m_counterClean<<endl;
		if(it1->m_lastMatchingObject == NULL)
		{
			it1->m_counterClean--;
			if(it1->m_counterClean <= 0)
			{
				it1 = m_templates.erase(it1);
				cptCleaned++;
			}
		}
		else it1->m_counterClean = m_param.maxNbFramesDisappearance;
		cptTotal++;
	}
	LOG_DEBUG("CleanTemplates : "<<cptCleaned<<" templates erased out of "<<cptTotal<<" templates");
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* DetectNewTemplates */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void TrackerByFeatures::DetectNewTemplates()
{
	// If region not matched, add a template
	int cpt = 0;
	for(vector<Object>::iterator it2 = m_objects.begin() ; it2 != m_objects.end(); it2++ )
	{
		if(!it2->m_isMatched && m_templates.size() < MAX_NB_TEMPLATES)
		{
			Template t(*it2, m_param.maxNbFramesDisappearance);
			/*if()
			{
				
			}*/
			m_templates.push_back(t);
			//cout<<"Added template "<<t.GetNum()<<endl;
			it2->SetId(t.GetNum());
			cpt++;
		}
	}
	LOG_DEBUG("DetectNewTemplates : "<<cpt<<" new templates added.");
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/// Match an object with the set of templates
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
const Template * TrackerByFeatures::MatchObject(const Object& x_obj)const
{
	double bestDist = DBL_MAX;
	const Template* bestTemp = NULL;

	//cout<<"Comparing template "<<m_num<<" with "<<x_regs.size()<<" regions"<<endl;

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
