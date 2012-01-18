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

#include "Tracker.h"
#include "Template.h"
#include <iostream>

using namespace std;
using namespace cv;

//int Object::m_counter = 0;

vector <string> Feature::m_names;

int Tracker::m_colorArraySize=54;
CvScalar Tracker::m_colorArray[] =
{
	CV_RGB(128,128,128), // gray//CV_RGB(255,255,255), // white
	CV_RGB(255,0,0), // red
	CV_RGB(0,255,0), // green
	CV_RGB(0,0,255), // blue
	CV_RGB(255,255,0), // yellow
	CV_RGB(0,150,255), // blue
	CV_RGB(130,77,191), // purple
	CV_RGB(255,100,0), // yellow
	CV_RGB(185,135,95), // brown
	CV_RGB(160, 32, 240),
	CV_RGB(255, 165, 0),
	CV_RGB(132, 112, 255),
	CV_RGB(0, 250, 154),
	CV_RGB(255, 192, 203),
	CV_RGB(0, 255, 255),
	CV_RGB(185, 185, 185),
	CV_RGB(216, 191, 216),
	CV_RGB(255, 105, 180),
	CV_RGB(0, 255, 255),
	CV_RGB(240, 255, 240),
	CV_RGB(173, 216, 230),
	CV_RGB(127, 255, 212),
	CV_RGB(100, 149, 237),
	CV_RGB(255, 165, 0),
	CV_RGB(255, 255, 0),
	CV_RGB(210, 180, 140),
	CV_RGB(211, 211, 211),
	CV_RGB(222, 184, 135),
	CV_RGB(205, 133, 63),
	CV_RGB(139, 69, 19),
	CV_RGB(165, 42, 42),
	CV_RGB(84, 134, 11),
	CV_RGB(210, 105, 30),
	CV_RGB(255, 127, 80),
	CV_RGB(255, 0, 255),
	CV_RGB(70, 130, 180),
	CV_RGB(95, 158, 160),
	CV_RGB(199, 21, 133),
	CV_RGB(255, 182, 193),
	CV_RGB(255, 140, 0),
	CV_RGB(240, 255, 255),
	CV_RGB(152, 251, 152),
	CV_RGB(143, 188, 143),
	CV_RGB(240, 230, 140),
	CV_RGB(240, 128, 128),
	CV_RGB(0, 191, 255),
	CV_RGB(250, 128, 114),
	CV_RGB(189, 183, 107),
	CV_RGB(255, 218, 185),
	CV_RGB(60, 179, 113),
	CV_RGB(178, 34, 34),
	CV_RGB(30, 144, 255),
	CV_RGB(255, 140, 0),
	CV_RGB(175, 238, 238)
};



Tracker::Tracker(const TrackerParameter& x_param, int width, int height, int type) :
	m_param(x_param)
{

	
}

void Tracker::Reset()
{
	
}



/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Match the template with a region (blob)*/
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/

int Tracker::MatchTemplate(Template& x_temp, vector<Object>& x_regs, /*cv::Mat* x_blobsImg, */double x_maxMatchingDistance)
{
	double bestDist = 1e99;
	int bestObject = -1;

	//cout<<"Comparing template "<<m_num<<" with "<<x_regs.size()<<" regions"<<endl;
	
	for(unsigned int i = 0 ; i< x_regs.size() ; i++)
	{
		double dist = x_temp.CompareWithObject(x_regs[i]);
		//cout<<"dist ="<<dist;
		if(dist < bestDist)
		{
			bestDist = dist;
			bestObject = i;
		}
	}
       if(bestObject != -1 && bestDist < x_maxMatchingDistance
               && (x_temp.GetNum() == MatchObject(x_regs[bestObject], x_maxMatchingDistance))) // Symetric match
	{
		x_temp.m_bestMatchingObject = bestObject;
		//cout<<"Template "<<GetNum()<<" matched with region "<<bestObject<<" dist="<<bestDist<<" pos:("<<m_posX<<","<<m_posY<<")"<<endl;
		x_temp.m_matchingObjects.push_back(x_regs[bestObject]);
		x_regs[bestObject].m_isMatched = 1;
		
		//CvPoint p = {x_regs[bestObject].m_posX, x_regs[bestObject].m_posY};
		//cvCircle(x_blobsImg, p, 10, Tracker::m_colorArray[m_num % Tracker::m_colorArraySize]);
		
		return 1;
	}
	else return 0;
	
}




Tracker::~Tracker(void)
{
}




/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* MatchTemplates : Match blobs with former object templates */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void Tracker::MatchTemplates()
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
		MatchTemplate(*it1, m_objects, /*m_blobsImg,*/ m_param.maxMatchingDistance);
	}
#ifdef VERBOSE
	cout<<"MatchTemplates : "<<m_templates.size()<<" templates and "<<m_objects.size()<<" regions."<<endl;
#endif
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* UpdateTemplates */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void Tracker::UpdateTemplates()
{
	for(list<Template>::iterator it1= m_templates.begin() ; it1 != m_templates.end(); it1++ )
		it1->UpdateFeatures();
	
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* CleanTemplates */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void Tracker::CleanTemplates()
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
void Tracker::DetectNewTemplates()
{
	// If region not matched, add a template
	int cpt = 0;
	for(vector<Object>::iterator it2 = m_objects.begin() ; it2 != m_objects.end(); it2++ )
	{
		if(!it2->m_isMatched && m_templates.size() < 100)
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

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* PrintObjects */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void Tracker::PrintObjects() const
{
	//cout<<"print region name="<<m_objects.at(0).GetFeatures().at(0).m_name<<endl;
	for ( vector<Object>::const_iterator it1= m_objects.begin() ; it1 < m_objects.end(); it1++ )
	{
		int cpt=0;
		cout<<"Object "/*<<(int)it1->GetNum()*/<<" : ";
		for ( vector<Feature>::const_iterator it2=it1->GetFeatures().begin() ; it2 < it1->GetFeatures().end(); it2++ )
		{
			cout<<" "<<it2->m_names.at(cpt)<<"="<<it2->GetValue()<<"|";
			cpt++;
		}
		cout<<endl;
	}
	
	
}


int Tracker::MatchObject(Object& x_obj, double x_maxMatchingDistance)
{

	double bestDist = 1e99;
	const Template* bestTemp = NULL;

	//cout<<"Comparing template "<<m_num<<" with "<<x_regs.size()<<" regions"<<endl;
	
	for(list<Template>::const_iterator it1 = m_templates.begin() ; it1 != m_templates.end(); it1++ )
	{
		double dist = it1->CompareWithObject(x_obj);
		//cout<<"dist ="<<dist;
		if(dist < bestDist)
		{
			bestDist = dist;
			bestTemp = &(*it1);
		}
	}
	if(bestTemp != NULL && bestDist < x_maxMatchingDistance)
		return bestTemp->GetNum();
	else 
		return 0;

}