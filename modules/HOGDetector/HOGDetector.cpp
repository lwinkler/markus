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

#include "HOGDetector.h"
#include "StreamImage.h"
#include "StreamObject.h"
#include "StreamDebug.h"
#include "Timer.h"

// #include <iostream>
// #include <cstdio>
#include <opencv2/highgui/highgui.hpp>


#include "util.h"

using namespace std;
using namespace cv;


const char * HOGDetector::m_type = "HOGDetector";


HOGDetector::HOGDetector(const ConfigReader& x_configReader) 
	 : ModuleAsync(x_configReader), m_param(x_configReader)
{
	// Init output images

	m_description = "Detect objects from a video stream using a HOG descriptor";
	m_input = new Mat(cvSize(m_param.width, m_param.height), m_param.type);
	m_lastInput = new Mat( cvSize(m_param.width, m_param.height), m_param.type);

	m_inputStreams.push_back(new StreamImage(0, "input", m_input, *this, 		"Video input")); 

	m_outputStreams.push_back(new StreamObject(0, "detected", m_param.width, m_param.height, 
				m_detectedObjects, colorFromStr(m_param.color), *this,	"Detected objects"));
#ifdef MARKUS_DEBUG_STREAMS
	m_debug = new Mat(cvSize(m_param.width, m_param.height), CV_8UC3);
	m_debugStreams.push_back(new StreamDebug(1, "debug", m_debug, *this,		""));
#endif
}

HOGDetector::~HOGDetector()
{
	delete(m_input);
	delete(m_lastInput);
#ifdef MARKUS_DEBUG_STREAMS
	delete(m_debug);
#endif
}

void HOGDetector::Reset()
{
	ModuleAsync::Reset();
	m_thread.m_hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
}

// This method launches the thread
void HOGDetector::LaunchThread()
{
	// assert(m_input->type() == CV_8UC1);
	//	throw("For cascade detection, input type must be CV_8UC1 and not " + ParameterImageType::ImageTypeInt2Str(img->type()));
	m_input->copyTo(*m_lastInput);
	Mat smallImg(*m_lastInput);
	// equalizeHist( smallImg, smallImg );
	
	// Launch a new Thread
	m_thread.SetData(smallImg, m_param.scaleFactor);
	m_thread.start();
	// m_thread.run(); // Use run instead of start for synchronous use
}

void HOGDetector::NormalProcess()
{
#ifdef MARKUS_DEBUG_STREAMS
	m_lastInput->copyTo(*m_debug);

	for(vector<Object>::const_iterator it = m_detectedObjects.begin() ; it != m_detectedObjects.end() ; it++)
	{
		// Draw the rectangle in the input image
		rectangle( *m_debug, it->Rect(), colorFromStr(m_param.color), 1, 8, 0 );
        }
#endif
}

/// Copy the results from the thread to the module
void HOGDetector::CopyResults()
{
	// TODO : See if we can find faster by avoiding copies
	m_detectedObjects.clear();
	for(std::vector<Rect>::const_iterator it = m_thread.GetDetectedObjects().begin() ; it != m_thread.GetDetectedObjects().end() ; it++)
	{
		Object obj(m_param.objectLabel, it->Rect());

		obj.AddFeature("x"      , obj.m_posX   / m_param.width);
		obj.AddFeature("y"      , obj.m_posY   / m_param.height);
		obj.AddFeature("width"  , obj.m_width  / m_param.width);
		obj.AddFeature("height" , obj.m_height / m_param.height);

		m_detectedObjects.push_back(obj);
	}
	m_timerThread += m_thread.m_timerThread;
	m_countFramesThread++;
}

/// Run the detection process inside the thread
void HOGDetectionThread::run()
{
	Timer ti;
	m_detected.clear();
	//if(m_scaleFactor == 1.00)
		//m_hog.detect(m_smallImg, m_detected) // , 0, Size(8,8), Size(32,32));
	//else
	m_hog.detectMultiScale(m_smallImg, m_detected, 0, Size(8,8), Size(32,32), m_scaleFactor, 2);

	m_timerThread = ti.GetMSecLong();
}


