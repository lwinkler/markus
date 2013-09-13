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

#include "CascadeDetector.h"
#include "StreamImage.h"
#include "StreamObject.h"
#include "StreamDebug.h"
#include "Timer.h"

#include <iostream>
#include <cstdio>
#include <opencv2/highgui/highgui.hpp>


#include "util.h"

using namespace std;
using namespace cv;


const char * CascadeDetector::m_type = "CascadeDetector";


CascadeDetector::CascadeDetector(const ConfigReader& x_configReader) 
	 : ModuleAsync(x_configReader), m_param(x_configReader)
{
	// Init output images
	if(! m_thread.m_cascade.load( m_param.filterFile ) || m_thread.m_cascade.empty())
		throw("Impossible to load cascade filter " + m_param.filterFile);
	m_description = "Detect objects from a video stream using a cascade filter (c.f. Haar patterns).";
	m_input = new Mat(cvSize(m_param.width, m_param.height), CV_8UC1);
	m_lastInput = new Mat( cvSize(m_param.width, m_param.height), m_param.type);
	m_debug = new Mat(cvSize(m_param.width, m_param.height), CV_8UC3);

	m_inputStreams.push_back(new StreamImage(0, "input", m_input, *this, 		"Video input")); 

	m_outputStreams.push_back(new StreamObject(0, "detected", m_param.width, m_param.height, 
				m_detectedObjects, colorFromStr(m_param.color), *this,	"Detected objects"));
	m_outputStreams.push_back(new StreamDebug(1, "debug", m_debug, *this,		""));
}

CascadeDetector::~CascadeDetector()
{
	delete(m_debug);
	delete(m_lastInput);
}

void CascadeDetector::Reset()
{
	ModuleAsync::Reset();
}

// This method launches the thread
void CascadeDetector::LaunchThread()
{
	assert(m_input->type() == CV_8UC1); // TODO add restriction to param
	//	throw("For cascade detection, input type must be CV_8UC1 and not " + ParameterImageType::ImageTypeInt2Str(img->type()));
	m_input->copyTo(*m_lastInput);
	Mat smallImg(*m_lastInput);
	equalizeHist( smallImg, smallImg );
	
	// Launch a new Thread
	m_thread.SetData(smallImg, m_param.minNeighbors, m_param.minSide, m_param.scaleFactor);
	m_thread.start();
	// m_thread.run(); // Use run instead of start for synchronous use
}

void CascadeDetector::NormalProcess()
{
	cvtColor(*m_lastInput, *m_debug, CV_GRAY2RGB);

	for(vector<Object>::const_iterator it = m_detectedObjects.begin() ; it != m_detectedObjects.end() ; it++)
	{
		Point p1(it->m_posX - it->m_width / 2, it->m_posY - it->m_width / 2);
		Point p2(it->m_posX + it->m_width / 2, it->m_posY + it->m_width / 2);
		
		// Draw the rectangle in the input image
		rectangle( *m_debug, p1, p2, colorFromStr(m_param.color), 1, 8, 0 );
        }
}

void CascadeDetector::CopyResults()
{
	// TODO : See if we can find faster by avoiding copies
	m_detectedObjects.clear();
	for(std::vector<Rect>::const_iterator it = m_thread.GetDetectedObjects().begin() ; it != m_thread.GetDetectedObjects().end() ; it++)
	{
		Object obj(m_param.objectLabel);
		//obj.SetRect(*it);
		obj.m_posX = it->x + it->width / 2;
		obj.m_posY = it->y + it->height / 2;
		obj.m_width = it->width;
		obj.m_height = it->height;

		obj.AddFeature("x", obj.m_posX);
		obj.AddFeature("y", obj.m_posY);
		obj.AddFeature("width", obj.m_width);
		obj.AddFeature("height", obj.m_height);

		m_detectedObjects.push_back(obj);
	}
	m_timerThread += m_thread.m_timerThread;
	m_countFramesThread++;
}

void DetectionThread::run()
{
	Timer ti;
	m_detected.clear();
	//cout<<"m_smallImg"<<&m_smallImg<<" m_detectedObjects"<<&m_detectedObjects<<" m_scaleFactor"<<m_scaleFactor<<" m_minNeighbors"<<m_minNeighbors<<endl;
	m_cascade.detectMultiScale(m_smallImg, m_detected, m_scaleFactor, m_minNeighbors, CV_HAAR_SCALE_IMAGE, cvSize(m_minSide, m_minSide));
	m_timerThread = ti.GetMSecLong();
}
