/*----------------------------------------------------------------------------------
 *
 *    MARKUS : a manager for video analysis modules
 * 
 *    author : Florian Rossier <florian.rossier@gmail.com>
 * 
 * 
 *    This file is part of Markus.
 *
 *    Markus is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *    Markus is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with Markus.  If not, see <http://www.gnu.org/licenses/>.
 -------------------------------------------------------------------------------------*/

// TODO Headers !!

#include <iostream>
#include "ExtractKeyPoints.h"
#include "StreamImage.h"
#include "StreamDebug.h"
#include "StreamObject.h"
#include "FeatureKeyPoint.h"
// #include <opencv2/opencv.hpp>
// #include <algorithm>

//for debug
#include "util.h"

using namespace cv;
using namespace std;

log4cxx::LoggerPtr ExtractKeyPoints::m_logger(log4cxx::Logger::getLogger("ExtractKeyPoints"));

ExtractKeyPoints::ExtractKeyPoints(const ConfigReader& x_configReader) :
	Module(x_configReader),
	m_param(x_configReader),
	m_input(Size(m_param.width, m_param.height), m_param.type)
{
	m_detector = NULL;

	AddInputStream(0, new StreamImage("input",  m_input, *this,   "Video input"));
	AddInputStream(1, new StreamObject("input", m_objectsIn, *this,	"Incoming objects"));

	AddOutputStream(0, new StreamObject("output", m_objectsOut, *this,	"List of tracked object with KeyPoint and features"));

#ifdef MARKUS_DEBUG_STREAMS
	m_debug = Mat(Size(m_param.width, m_param.height), CV_8UC3);
	AddDebugStream(0, new StreamDebug("input_with_key_point", m_debug, *this,	"Foreground with KeyPoint"));
#endif
};


ExtractKeyPoints::~ExtractKeyPoints()
{
	CLEAN_DELETE(m_detector);
}

void ExtractKeyPoints::Reset()
{
	Module::Reset();
	CLEAN_DELETE(m_detector);
	m_detector = FeatureDetector::create(m_param.keyPointType);
	if(m_detector == NULL && m_detector->empty())
		throw MkException("Cannot create detector", LOC);

	// If the object input is not connected: extract on whole image
	if(m_objectsIn.size() == 0)
		m_objectsIn.push_back(Object("screen", Rect(0, 0, m_input.cols, m_input.rows)));
}

/**
 */
void ExtractKeyPoints::ProcessFrame()
{
	//compute each object to find point of interest
	m_objectsOut.clear();
	for(vector<Object>::iterator it1 = m_objectsIn.begin() ; it1 != m_objectsIn.end() ; it1++)
	{
		it1->Intersect(m_input);

		//compute point of interest and add it to m_objectsOut
		//strange error (ASSERT : 0<=roi.x&&0<=roi.width&&roi.x+roi.width<=m_input.cols&&0<=roi.y&&0<=roi.height&&roi.y+roi.height<=m_input.rows)
		//this cause unit test fail
		// Mat subImage(m_input, it1->Rect());    
		vector<KeyPoint> pointsOfInterest;

cout<<"ici\n";
		m_detector->detect(m_input, pointsOfInterest);
cout<<"ici2\n";
		//Mat subImage(m_input,it1->Rect());    
		//m_detector->detect(subImage, pointsOfInterest);

		// For each keypoint create an output object
		for(vector<KeyPoint>::const_iterator it2 = pointsOfInterest.begin() ; it2 != pointsOfInterest.end() ; it2++)
		{
			Object obj("keypoint");
			obj.posX = it2->pt.x + it1->posX - it1->width /2 - 5;
			obj.posY = it2->pt.y + it1->posY - it1->height/2 - 5;
			obj.width  = 10;
			obj.height = 10; // TODO: use the scale to set the size of the object
			obj.Intersect(m_input);
			obj.SetId(it1->GetId()); // TODO: Keep this ?
			obj.AddFeature("keypoint", new FeatureKeyPoint(*it2));
			obj.AddFeature("parent", new FeatureFloat(it1->GetId())); // TODO: Feature int
			m_objectsOut.push_back(obj);
		}

#ifdef MARKUS_DEBUG_STREAMS
/*
		// cvtColor(m_input, m_debug, CV_GRAY2RGB);
		m_input.copyTo(m_debug); // TODO : Fix?

		float x_top_left = it1->posX-it1->width/2;
		float y_top_left = it1->posY-it1->height/2;
		for (size_t j = 0; j < pointsOfInterest.size(); j++) {
			KeyPoint elem = pointsOfInterest[j];
			float x_pos = x_top_left+elem.pt.x;
			float y_pos = y_top_left+elem.pt.y;
			Scalar color = Scalar(m_rng.uniform(0, 255), m_rng.uniform(0,255), m_rng.uniform(0,255));
			circle(m_debug, Point(x_pos,y_pos), 3,color);
		}
		*/
#endif        
		//NOTE : a param could decide if we compute descriptor to avoid useless computation 
	}

}

