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
#include "StreamRect.h"
#include "StreamDebug.h"

#include <iostream>
#include <cstdio>
#include <highgui.h>

#include "util.h"

using namespace std;
using namespace cv;


const char * CascadeDetector::m_type = "CascadeDetector";


CascadeDetector::CascadeDetector(const ConfigReader& x_configReader) 
	 : ModuleAsync(x_configReader), m_param(x_configReader)
{
	// Init output images
	assert(m_thread.m_cascade.load( m_param.filterFile ));
	assert(!m_thread.m_cascade.empty());
	m_description = "Detect objects from a video stream using a cascade filter (c.f. Haar patterns).";
	m_debug = new Mat(cvSize(m_param.width, m_param.height), CV_8UC3);
	m_input = new Mat(cvSize(m_param.width, m_param.height), CV_8UC3);
	//m_output = new Mat(cvSize(m_param.width, m_param.height), CV_8UC3);

	m_inputStreams.push_back(new StreamImage(0, "input", m_input, *this, 		"Video input")); 

	m_outputStreams.push_back(new StreamRect(0, "detected", m_param.width, m_param.height, 
				m_detectedObjects, ColorFromStr(m_param.color), *this,	"Detected objects"));
	m_outputStreams.push_back(new StreamDebug(1, "debug", m_debug, *this,		""));
//	m_outputStreams.push_back(new StreamImage("input", m_inputCopy));
	m_lastInput = new Mat( cvSize(GetInputWidth(), GetInputHeight()), GetInputType());
}

CascadeDetector::~CascadeDetector(void)
{
	delete(m_debug);
	delete(m_lastInput);

	//TODO : delete output streams
}

// This method launches the thread
void CascadeDetector::LaunchThread(const Mat* img, const double x_timeSinceLastProcessing)
{
	assert(img->type() == CV_8UC1); // TODO add restriction to param
	img->copyTo(*m_lastInput);
	Mat smallImg(*m_lastInput);
	equalizeHist( smallImg, smallImg );
		
	// Launch a new Thread
	m_thread.SetData(smallImg, m_param.minNeighbors, m_param.minFaceSide, m_param.scaleFactor);
	m_thread.start();
}

void CascadeDetector::NormalProcess(const Mat* img, const double x_timeSinceLastProcessing)
{
	cvtColor(*m_lastInput, *m_debug, CV_GRAY2RGB);
	//Stream& os(*m_outputStreams[0]);
	//os.ClearRect();
	for(vector<Rect>::const_iterator it = m_detectedObjects.begin() ; it != m_detectedObjects.end() ; it++)
	{
		// TODO : See if we move this to execute it once only
		Point p1(it->x, it->y);
		Point p2(it->x + it->width, it->y + it->height);
		
		// Draw the rectangle in the input image
		rectangle( *m_debug, p1, p2, CV_RGB(255,0,0), 1, 8, 0 );
		
		// Add rectangle to output streams
		//os.AddRect(cv::Rect(p1, p2));
        }
}

void CascadeDetector::CopyResults()
{
	m_detectedObjects = m_thread.GetDetectedObjects();
}

void DetectionThread::run()
{
	m_cascade.detectMultiScale(m_smallImg, m_detectedObjects, m_scaleFactor, m_minNeighbors, CV_HAAR_SCALE_IMAGE, cvSize(m_minFaceSide, m_minFaceSide));
	//QThread::run();
}
