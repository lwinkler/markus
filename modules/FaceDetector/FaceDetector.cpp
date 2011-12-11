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

#include "FaceDetector.h"

#include <iostream>
#include <cstdio>
#include <highgui.h>

#include "util.h"

using namespace std;
using namespace cv;


const char * FaceDetector::m_type = "FaceDetector";


FaceDetector::FaceDetector(const std::string& x_name, ConfigReader& x_configReader) 
	 : m_param(x_configReader, x_name), ModuleAsync(x_name, x_configReader)
{
	// Init output images
	assert(m_thread.m_cascade.load( m_param.filterFile ));
	assert(!m_thread.m_cascade.empty());
	m_output = cvCreateImage(cvSize(m_param.width, m_param.height), m_param.depth, 3);

	m_outputStreams.push_back(new OutputStream("faceDetector", STREAM_OUTPUT, m_output));
//	m_outputStreams.push_back(new OutputStream("input", STREAM_OUTPUT, m_inputCopy));
	m_lastInput = cvCreateImage( cvSize(GetWidth(), GetHeight()), GetDepth(), GetNbChannels());
}

FaceDetector::~FaceDetector(void)
{
	cvReleaseImage(&m_output);
	cvReleaseImage(&m_lastInput);

	//TODO : delete output streams
}

// This method launches the thread
void FaceDetector::LaunchThread(const IplImage* img, const double x_timeSinceLastProcessing)
{
	cvCopy(img, m_lastInput);
	Mat smallImg(m_lastInput, false);
	equalizeHist( smallImg, smallImg );
	// Launch a new Thread
	m_faces = m_thread.GetDetectedObjects();
	m_thread.SetData(smallImg, m_param.minNeighbors, m_param.minFaceSide, m_param.scaleFactor);
	m_thread.start();
}

void FaceDetector::NormalProcess(const IplImage* img, const double x_timeSinceLastProcessing)
{
	cvConvertImage(m_lastInput, m_output);
	for(vector<Rect>::const_iterator it = m_faces.begin() ; it != m_faces.end() ; it++)
	{
		Point p1(it->x, it->y);
		Point p2(it->x + it->width, it->y + it->height);
		
		// Draw the rectangle in the input image
		cvRectangle( m_output, p1, p2, CV_RGB(255,0,0), 1, 8, 0 );
        }
}


void DetectionThread::run()
{
	m_cascade.detectMultiScale(m_smallImg, m_faces, m_scaleFactor, m_minNeighbors, CV_HAAR_SCALE_IMAGE, cvSize(m_minFaceSide, m_minFaceSide));
	//QThread::run();
}
