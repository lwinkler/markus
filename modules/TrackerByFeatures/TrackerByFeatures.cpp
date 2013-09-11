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
#include "StreamDebug.h"
#include "StreamObject.h"

// for debug
#include "util.h"

using namespace cv;

const char * TrackerByFeatures::m_type = "TrackerByFeatures";

#define MAX_NB_TEMPLATES 100

using namespace std;
using namespace cv;



TrackerByFeatures::TrackerByFeatures(const ConfigReader& x_configReader) :
	Module(x_configReader),
	m_param(x_configReader)
{
	m_description = "Track objects by matching a set of features (typically position and size)";
	
	m_inputStreams.push_back(new StreamObject(0, "input", 	m_param.width, m_param.height, m_objects, cvScalar(255, 255, 255), *this,	"Input objects"));

	m_outputObjectStream = new StreamObject(0, "tracker", m_param.width, m_param.height, m_objects, cvScalar(255, 255, 255), *this,	"Tracked objects");
	m_outputStreams.push_back(m_outputObjectStream);

	// TODO : Output template + check if ok in/out same
	
	vector<string> elems;
	split(m_param.features, ',', elems);
	int n = 0;
	for(vector<std::string>::const_iterator it = elems.begin() ; it != elems.end() ; it++)
	{
		cout<<" "<<*it;
		m_outputObjectStream->AddFeatureName(*it);
		m_featureIndices.push_back(n);
		n++;
	}
}

TrackerByFeatures::~TrackerByFeatures(void )
{
}

void TrackerByFeatures::Reset()
{
	Module::Reset();
}

void TrackerByFeatures::ProcessFrame()
{
	MatchTemplates();
	CleanTemplates();
	DetectNewTemplates();
	UpdateTemplates();
	
	// Output the list of templates
	/*m_trackerOutput.clear();
	for(list<Template>::const_iterator it = m_templates.begin()  ; it != m_templates.end() ; it++ )
	{
		Rect rect;
		rect.x = it->m_posX;
		rect.y = it->m_posY;
		rect.width = 10;
		rect.height = 30;//TODO
		Object obj;
		obj.SetRect(rect); // TODO : Objects must be centered !!!!
		m_trackerOutput.push_back(obj);
	}*/

	//track.PrintTrackedRegions();
};



/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/// Match the template with an object (blob)
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/

int TrackerByFeatures::MatchTemplate(Template& x_temp)
{
	double bestDist = 1e99;
	int bestObject = -1;

	//cout<<"Comparing template "<<m_num<<" with "<<x_regs.size()<<" regions"<<endl;
	
	for(unsigned int i = 0 ; i< m_objects.size() ; i++)
	{
		double dist = x_temp.CompareWithObject(m_objects[i], m_featureIndices);
		//cout<<"dist ="<<dist;
		if(dist < bestDist)
		{
			bestDist = dist;
			bestObject = i;
		}
	}
       if(bestObject != -1 && bestDist < m_param.maxMatchingDistance
               && (x_temp.GetNum() == MatchObject(m_objects[bestObject]) || !m_param.symetricMatch)) // Note: the order of this condition is important since MatchObject must be called each time !!
	{
		x_temp.m_bestMatchingObject = bestObject;
#ifdef VERBOSE
//		cout<<"Template "<<x_temp.GetNum()<<" matched with region "<<bestObject<<" dist="<<bestDist<<" pos:("<<m_posX<<","<<m_posY<<")"<<endl;
#endif
		x_temp.m_matchingObjects.push_back(m_objects[bestObject]);
		m_objects[bestObject].m_isMatched = 1;
		
		//CvPoint p = {x_regs[bestObject].m_posX, x_regs[bestObject].m_posY};
		//cvCircle(x_blobsImg, p, 10, TrackerByFeatures::m_colorArray[m_num % TrackerByFeatures::m_colorArraySize]);
		
		return 1;
	}
	else return 0;
	
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* MatchTemplates : Match blobs with former object templates */
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
		it1->m_bestMatchingObject = -1;
		//int isMatched = 
		MatchTemplate(*it1);
	}
#ifdef VERBOSE
	cout<<"MatchTemplates : "<<m_templates.size()<<" templates and "<<m_objects.size()<<" regions."<<endl;
#endif
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* UpdateTemplates */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void TrackerByFeatures::UpdateTemplates()
{
	for(list<Template>::iterator it1= m_templates.begin() ; it1 != m_templates.end(); it1++ )
		it1->UpdateFeatures();
	
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
		if(it1->m_bestMatchingObject == -1)
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
#ifdef VERBOSE
	cout<<"CleanTemplates : "<<cptCleaned<<" templates erased out of "<<cptTotal<<" templates"<<endl;
#endif
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* UpdateTemplates */
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
			m_templates.push_back(t);
			//cout<<"Added template "<<t.GetNum()<<endl;
			cpt++;
		}
	}
#ifdef VERBOSE
	cout<<"DetectNewTemplates : "<<cpt<<" new templates added."<<endl;
#endif
}


int TrackerByFeatures::MatchObject(Object& x_obj)
{

	double bestDist = 1e99;
	const Template* bestTemp = NULL;

	//cout<<"Comparing template "<<m_num<<" with "<<x_regs.size()<<" regions"<<endl;
	
	for(list<Template>::const_iterator it1 = m_templates.begin() ; it1 != m_templates.end(); it1++ )
	{
		double dist = it1->CompareWithObject(x_obj, m_featureIndices);
		//cout<<"dist ="<<dist;
		if(dist < bestDist)
		{
			bestDist = dist;
			bestTemp = &(*it1);
		}
	}
	if(bestTemp != NULL && bestDist < m_param.maxMatchingDistance)
	{
		x_obj.SetId(bestTemp->GetNum()); // Set color of template 
		return bestTemp->GetNum();
	}
	else 
	{
		x_obj.SetId(-1);
		return 0;
	}
}
