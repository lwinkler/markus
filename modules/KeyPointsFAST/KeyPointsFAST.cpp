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
#include "KeyPointsFAST.h"
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

log4cxx::LoggerPtr KeyPointsFAST::m_logger(log4cxx::Logger::getLogger("KeyPointsFAST"));

KeyPointsFAST::KeyPointsFAST(const ConfigReader& x_configReader) :
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


KeyPointsFAST::~KeyPointsFAST()
{
	CLEAN_DELETE(m_detector);
}

void KeyPointsFAST::Reset()
{
	// TODO remove
	vector<string> algos;
	Algorithm::getList(algos);

	for(vector<string>::iterator it = algos.begin() ; it != algos.end() ; it++)
		cout<<*it<<endl;


	Module::Reset();
	CLEAN_DELETE(m_detector);
        m_detector = new FastFeatureDetector(m_param.threshold);
        // m_detector = Algorithm::create<Feature2D>("Feature2D.FAST");
	if(m_detector == NULL && m_detector->empty())
		throw MkException("Cannot create detector", LOC);

	// If the object input is not connected: extract on whole image
        if(!m_inputStreams.at(1)->IsConnected())
	{
		LOG_INFO(m_logger, "Object input not connected, use the whole image");
        	m_objectsIn.push_back(Object("screen", Rect(0, 0, m_input.cols, m_input.rows)));
	}
}

/**
 */
void KeyPointsFAST::ProcessFrame()
{
#ifdef MARKUS_DEBUG_STREAMS
		m_input.copyTo(m_debug);
#endif
	//compute each object to find point of interest
	m_objectsOut.clear();
	for(vector<Object>::iterator it1 = m_objectsIn.begin() ; it1 != m_objectsIn.end() ; it1++)
	{
		it1->Intersect(m_input);

		//compute point of interest and add it to m_objectsOut
		//strange error (ASSERT : 0<=roi.x&&0<=roi.width&&roi.x+roi.width<=m_input.cols&&0<=roi.y&&0<=roi.height&&roi.y+roi.height<=m_input.rows)
		//this cause unit test fail
		Mat subImage(m_input, it1->Rect());    
		vector<KeyPoint> pointsOfInterest;

		m_detector->detect(subImage, pointsOfInterest);

		//Mat subImage(m_input,it1->Rect());    
		//m_detector->detect(subImage, pointsOfInterest);

		// For each keypoint create an output object
		for(vector<KeyPoint>::const_iterator it2 = pointsOfInterest.begin() ; it2 != pointsOfInterest.end() ; it2++)
		{
			Object obj("keypoint");
			obj.posX = it2->pt.x + it1->posX - it1->width /2 - 5;
			obj.posY = it2->pt.y + it1->posY - it1->height/2 - 5;
			obj.width  = it2->size;
			obj.height = it2->size; // TODO: use the scale to set the size of the object
			obj.Intersect(m_input);
			obj.SetId(it1->GetId()); // TODO: Keep this ?
			obj.AddFeature("keypoint", new FeatureKeyPoint(*it2));
			obj.AddFeature("parent", new FeatureFloat(it1->GetId())); // TODO: Feature int
			m_objectsOut.push_back(obj);
		}

#ifdef MARKUS_DEBUG_STREAMS
		// Scalar color = Scalar(22, 88, 255);
		// drawKeypoints(m_debug, pointsOfInterest, m_debug, color);

		for(vector<KeyPoint>::const_iterator it2 = pointsOfInterest.begin() ; it2 != pointsOfInterest.end() ; it2++)
		{
			Scalar color = Scalar(22, 88, 255 * it2->response);
			circle(m_debug, it2->pt, 3, Scalar(0, 55, 0));
			line(m_debug, it2->pt, Point(
				it2->pt.x + it2->octave * cos(it2->angle / 360 * 2 * M_PI), 
				it2->pt.y - it2->octave * sin(it2->angle / 360 * 2 * M_PI)
			), color);
		}
#endif        
		//NOTE : a param could decide if we compute descriptor to avoid useless computation 
	}

}

