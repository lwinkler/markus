// +--------------------------------------------------------------------------+
// | File      : Tracker.cpp                                                  |
// | Utility   : Experimental object tracker                                  |
// | Author    : Laurent Winkler                                              |
// | Date      : July 2010                                                    |
// | Remarque  : none                                                         |
// +--------------------------------------------------------------------------+

#include "Tracker.h"

/*#include "cvblobs/BlobResult.h"
#include "cvblobs/BlobExtraction.h"
#include "cvblobs/Blob.h"
#include "cvblobs/BlobLibraryConfiguration.h"
#include "cvblobs/BlobResult.h"*/

#include <iostream>
#include <cstdio>
#include <highgui.h>

using namespace std;

#define POW2(x) (x) * (x)

int Tracker::m_maxMatchingDistance = 100;
int Tracker::m_maxNbFramesDisappearance = 10;

int Template::m_counter = 0;
//int Region::m_counter = 0;

vector <string> Feature::m_names;

int Tracker::colorArraySize=54;
CvScalar Tracker::colorArray[] =
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


Feature::Feature(/*const string& x_name, */double x_value)
{
	//sprintf(m_name,"%s", x_name);
	//printf("name %s %s\n",x_name, m_name);
	m_value = x_value;
	m_variance = 0.1; // TODO : Default variance
}

Feature::Feature(const Feature& f)
{
	//strcpy(m_name, f.GetName());
	m_value=f.GetValue();
	m_variance = f.GetVariance();
};

Feature&  Feature::operator = (const Feature& f)
{
	//strcpy(m_name, f.GetName());
	m_value=f.GetValue();
	m_variance = f.GetVariance();

	return *this;
};

Feature::~Feature(){}

Region::Region(int x_num)
{
	m_num = x_num;//m_counter;
	//m_counter++;
};

Region::Region(const Region& r)
{
	m_num = r.GetNum();
	m_feats = r.GetFeatures();
	m_posX = r.m_posX;
	m_posY = r.m_posY;
}

Region& Region::operator = (const Region& r)
{
	m_num = r.GetNum();
	m_feats = r.GetFeatures();
	m_posX = r.m_posX;
	m_posY = r.m_posY;

	return *this;
}

Region::~Region(){};

int Region::Match(const std::list<Template>& x_temp)
{

	double bestDist = 1e99;
	const Template* bestTemp = NULL;

	//cout<<"Comparing template "<<m_num<<" with "<<x_regs.size()<<" regions"<<endl;
	
	for(list<Template>::const_iterator it1 = x_temp.begin() ; it1 != x_temp.end(); it1++ )
	{
		double dist = it1->CompareWithRegion(*this);
		//cout<<"dist ="<<dist;
		if(dist < bestDist)
		{
			bestDist = dist;
			bestTemp = &(*it1);
		}
	}
	if(bestTemp != NULL && bestDist < Tracker::m_maxMatchingDistance)
		return bestTemp->GetNum();
	else 
		return 0;

}


Tracker::Tracker(int width, int height, int depth, int channels)
{

	m_blobsImg = cvCreateImage(cvSize(width, height), depth, channels);
	
}

Template::Template()
{
	m_num = m_counter;
	m_bestMatchingRegion = -1;
	m_counterClean = Tracker::m_maxNbFramesDisappearance;
	m_posX = 0;
	m_posY = 0;

	m_counter++;
}

Template::Template(const Template& t)
{
	m_num = t.GetNum();
	m_matchingRegions = t.GetMatchingRegions();
	m_feats = t.GetFeatures();
	m_posX = t.m_posX;
	m_posY = t.m_posY;

	m_bestMatchingRegion = -1;
	m_counterClean = Tracker::m_maxNbFramesDisappearance;

}

Template::Template(const Region& x_reg)
{
	m_num = m_counter;
	m_counter++;
	m_feats = x_reg.GetFeatures();
	m_posX = x_reg.m_posX;
	m_posY = x_reg.m_posY;
	m_bestMatchingRegion = -1;
	m_counterClean = Tracker::m_maxNbFramesDisappearance;

	//cout<<"Region "<<x_reg.GetNum()<<" is used to create template "<<m_num<<" with "<<x_reg.GetFeatures().size()<<" features"<<endl;
}

Template& Template::operator = (const Template& t)
{
	m_num = t.GetNum();
	m_matchingRegions = t.GetMatchingRegions();
	m_feats = t.GetFeatures();
	m_posX = t.m_posX;
	m_posY = t.m_posY;
	
	m_bestMatchingRegion = -1;
	m_counterClean = Tracker::m_maxNbFramesDisappearance;

	return *this;
}

Template::~Template()
{
	
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Get a value from a feature vector */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
double Feature::GetFeatureValue(const std::vector<Feature>& x_vect, const char* x_name)
{
	int cpt = 0;
	
	for ( vector<Feature>::const_iterator it1= x_vect.begin() ; it1 != x_vect.end(); it1++ )
	{
		if(! Feature::m_names.at(cpt).compare(x_name))// !strcmp((const char*) Feature::m_names.at(cpt).compare(x_name)/* it1->m_name*/, x_name))
			return it1->m_value;
		cpt++;
	}
	throw("GetFeatureValue : cannot find feature " + string(x_name));

}


/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Compare a candidate region with the template */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/

double Template::CompareWithRegion(const Region& x_reg) const
{
	double sum = 0;
	//cout<<"m_feats.size() ="<<m_feats.size()<<endl;
	assert(m_feats.size() == x_reg.GetFeatures().size());
	
	for ( unsigned int i=0 ; i < m_feats.size() ; i++)
	{
		//cout<<"m_feats[i].GetValue()"<<m_feats[i].GetValue()<<endl;
		//cout<<"x_reg.GetFeatures()[i].GetValue()"<<x_reg.GetFeatures()[i].GetValue()<<endl;
		
		//cout<<"temp val ="<<m_feats[i].GetValue()<<" region val="<<x_reg.GetFeatures()[i].GetValue()<<" temp var="<<m_feats[i].GetVariance()<<endl;
		sum += POW2(m_feats[i].GetValue() - x_reg.GetFeatures()[i].GetValue()) 
			/ POW2(m_feats[i].GetVariance());
	}
	return sqrt(sum) / m_feats.size();
}


/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Match the template with a region (blob)*/
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/

int Template::Match(const std::list<Template>& x_temp, vector<Region>& x_regs, IplImage* x_blobsImg)
{

	double bestDist = 1e99;
	int bestRegion = -1;

	//cout<<"Comparing template "<<m_num<<" with "<<x_regs.size()<<" regions"<<endl;
	
	for(unsigned int i = 0 ; i< x_regs.size() ; i++)
	{
		double dist = CompareWithRegion(x_regs[i]);
		//cout<<"dist ="<<dist;
		if(dist < bestDist)
		{
			bestDist = dist;
			bestRegion = i;
		}
	}
	if(bestRegion != -1 && bestDist < Tracker::m_maxMatchingDistance
		&& (m_num == x_regs[bestRegion].Match(x_temp))) // Symetric match
	{
		m_bestMatchingRegion = bestRegion;
		//cout<<"Template "<<GetNum()<<" matched with region "<<bestRegion<<" dist="<<bestDist<<" pos:("<<m_posX<<","<<m_posY<<")"<<endl;
		m_matchingRegions.push_back(x_regs[bestRegion]);
		x_regs[bestRegion].m_isMatched = 1;
		CvPoint p = {x_regs[bestRegion].m_posX, x_regs[bestRegion].m_posY};
		
		cvCircle(x_blobsImg, p, 10, Tracker::colorArray[m_num % Tracker::colorArraySize]);
		
		return 1;
	}
	else return 0;
	
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Update the template's features based on the latest 50 matching regions */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/

void Template::UpdateFeatures()
{
	for ( unsigned int i=0 ; i < m_feats.size() ; i++)
	{
		double mean=0;
		double sqstdev=0;
		int size = m_matchingRegions.size();
		if (size <= 0) return;
		
		//cout<<"Updating template "<<m_num<<" with "<<m_matchingRegions.size()<<" matching regions."<<endl;
		
		for ( list<Region>::iterator it1= m_matchingRegions.begin() ; it1 != m_matchingRegions.end(); it1++ )
			mean += it1->GetFeatures().at(i).GetValue();
		mean /= size;
		
		for ( list<Region>::iterator it1= m_matchingRegions.begin() ; it1 != m_matchingRegions.end(); it1++ )
			sqstdev += (it1->GetFeatures().at(i).GetValue() - mean) * (it1->GetFeatures().at(i).GetValue() - mean);
		sqstdev /= size;

		//cout<<"UpdateFeatures : "<<m_feats[i].GetValue()<<"->"<<mean<<endl;
		//cout<<"UpdateFeatures : "<<m_feats[i].GetVariance()<<"->"<<sqrt(sqstdev)<<endl;
		m_feats[i].SetValue(mean);
		m_feats[i].SetVariance(sqstdev<0.01 ? 0.01 : sqrt(sqstdev)); /// !!!! TODO ? Faster if left squared
	}
	
}

void trackerBarCallback1(int pos) 
{
	Tracker::SetMaxMatchingDistance( pos);
	cout<<"Max matching distance set to "<<Tracker::GetMaxMatchingDistance()<<endl;
}

void trackerBarCallback2(int pos) 
{
	Tracker::SetMaxNbFramesDisappearance( pos);
	cout<<"Max frames before object disappearance set to "<<Tracker::GetMaxNbFramesDisappearance()<<endl;
}

void Tracker::CreateParamWindow()
{
	cvNamedWindow("Parameters", CV_WINDOW_AUTOSIZE); 
	cvMoveWindow("Parameters", 0, 0);

	cvCreateTrackbar( "Max matching distance ", "Parameters", &m_maxMatchingDistance, 100, trackerBarCallback1 );
	cvCreateTrackbar( "Frames before disappearance ", "Parameters", &m_maxNbFramesDisappearance, 100, trackerBarCallback2 );
//	cvCreateTrackbar( "Foreground filter size ", "Parameters", &m_foregroundFilterSize, 7, trackbarCallback3 );

}

Tracker::~Tracker(void)
{
	cvReleaseImage(&m_blobsImg);
}


/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* ExtractBlob */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void Tracker::ExtractBlobs(IplImage* x_img)
{
	// object that will contain blobs of inputImage
	CBlobResult blobs;
	cvSet(m_blobsImg, cvScalar(0,0,0));
	m_regions.clear();
	
	// Extract the blobs using a threshold of 100 in the image
	blobs = CBlobResult( x_img, NULL, 100/*m_foreground_thres* 255*/, true);
	
	// create a file with some of the extracted features
	blobs.PrintBlobs( (char*) "blobs.txt" );
	
	int i;
	CBlob *currentBlob;
	// exclude the ones smaller than value
	blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_OUTSIDE, 25, x_img->imageSize / 4);

	for (i = 0; i < blobs.GetNumBlobs(); i++ )
	{
			currentBlob = blobs.GetBlob(i);
			
			double posx = currentBlob->SumX();
			double posy = currentBlob->SumY();
		if(currentBlob->Parent() && !currentBlob->Exterior() && posx > 0 && posx < m_blobsImg->width && posy > 0 && posy < m_blobsImg->height) // TODO : fix this for real
		{
			currentBlob->FillBlob( m_blobsImg, colorArray[i % colorArraySize]);
			
			Region reg(i);
			reg.m_posX = currentBlob->SumX();
			reg.m_posY = currentBlob->SumY();
			
			// Add here all features that are to be added in the templates/region
			static int do_once = 1;
			if(do_once)
			{
				// Write names once
				Feature::m_names.push_back("area");
				Feature::m_names.push_back("perimeter");
				Feature::m_names.push_back("position x");
				Feature::m_names.push_back("position y");
				do_once = 0;
			}
			
			reg.AddFeature(currentBlob->Area());
	 		reg.AddFeature(currentBlob->Perimeter());
	
	// 		//reg.AddFeature("mean", GetSTLResult(currentBlob, CBlobGetMean()));
	// 		reg.AddFeature("compactness", GetSTLResult(currentBlob, CBlobGetCompactness()));
	// 		reg.AddFeature("length", GetSTLResult(currentBlob, CBlobGetLength()));
	// 
	// 		reg.AddFeature("convex hull perimeter", GetSTLResult(currentBlob, CBlobGetHullPerimeter()));
	
			reg.AddFeature(currentBlob->SumX());
			reg.AddFeature(currentBlob->SumY());
			
			//cout<<"Blob extracted position "<<currentBlob->SumX()<<","<<currentBlob->SumY()<<endl;;
			m_regions.push_back(reg);
		}
		//else throw("ERROR");
	}
}


double Tracker::GetSTLResult( CBlob* blob, funcio_calculBlob *evaluador ) const
{
	return (*evaluador)(*blob);
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* MatchTemplates : Match blobs with former object templates */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void Tracker::MatchTemplates()
{
	for(vector<Region>::iterator it2 = m_regions.begin() ; it2 != m_regions.end(); it2++ )
	{
		it2->m_isMatched = 0;
	}

	// Try to match each region with a template
	for(list<Template>::iterator it1 = m_templates.begin() ; it1 != m_templates.end(); it1++ )
	{
		it1->m_bestMatchingRegion = -1;
		//int isMatched = 
		it1->Match(m_templates, m_regions, m_blobsImg);
		
	}
#ifdef VERBOSE
	cout<<"MatchTemplates : "<<m_templates.size()<<" templates and "<<m_regions.size()<<" regions."<<endl;
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
		if(it1->m_bestMatchingRegion == -1)
		{
			it1->m_counterClean--;
			if(it1->m_counterClean <= 0)
			{
				it1 = m_templates.erase(it1);
				cptCleaned++;
			}
		}
		else it1->m_counterClean = Tracker::m_maxNbFramesDisappearance;
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
	for(vector<Region>::iterator it2 = m_regions.begin() ; it2 != m_regions.end(); it2++ )
	{
		if(!it2->m_isMatched && m_templates.size() < 100)
		{
			Template t(*it2);
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
/* PrintRegions */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void Tracker::PrintRegions() const
{
	//cout<<"print region name="<<m_regions.at(0).GetFeatures().at(0).m_name<<endl;
	for ( vector<Region>::const_iterator it1= m_regions.begin() ; it1 < m_regions.end(); it1++ )
	{
		int cpt=0;
		cout<<"Region "<<(int)it1->GetNum()<<" : ";
		for ( vector<Feature>::const_iterator it2=it1->GetFeatures().begin() ; it2 < it1->GetFeatures().end(); it2++ )
		{
			cout<<" "<<it2->m_names.at(cpt)<<"="<<it2->GetValue()<<"|";
			cpt++;
		}
		cout<<endl;
	}
	
	
}
