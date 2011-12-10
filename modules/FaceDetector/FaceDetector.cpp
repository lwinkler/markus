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
	assert(m_cascade.load( m_param.filterFile ));
	assert(!m_cascade.empty());
	m_output = cvCreateImage(cvSize(m_param.width, m_param.height), m_param.depth, 3);

	m_outputStreams.push_back(new OutputStream("faceDetector", STREAM_OUTPUT, m_output));
//	m_outputStreams.push_back(new OutputStream("input", STREAM_OUTPUT, m_inputCopy));
}

FaceDetector::~FaceDetector(void)
{
	cvReleaseImage(&m_output);

	//TODO : delete output streams
}

void FaceDetector::PreProcess(const IplImage* img, const double x_timeSinceLastProcessing)
{
//	assert(img->depth == IPL_DEPTH_8U);
//	assert(img->nChannels == 1);
}


void FaceDetector::ThreadProcess(const IplImage* img, const double x_timeSinceLastProcessing)
{
	 cv::CascadeClassifier cascade; 
    double scale = 1; 

	if( !cascade.load( m_param.filterFile ) ) 
	{ 
		printf("Cascade Error: Could not load cascade file, please check the path\n"); 
	} 
	
	m_faces.clear();
	Mat smallImg(img, false);
	equalizeHist( smallImg, smallImg );
	//cout<<"m_cascade.detectMultiScale("<<", "<<m_param.scaleFactor<<" "<<m_param.minNeighbors<<" "<<m_param.minFaceSize<<endl;
	cascade.detectMultiScale(smallImg, m_faces, m_param.scaleFactor, m_param.minNeighbors, CV_HAAR_SCALE_IMAGE, cvSize(m_param.minFaceSide, m_param.minFaceSide));
//	cout<<" found "<<m_faces.size()<<endl;
}

void FaceDetector::PostProcess(const IplImage* img, const double x_timeSinceLastProcessing)
{
	cvConvertImage(img, m_output);
	for(vector<Rect>::const_iterator it = m_faces.begin() ; it != m_faces.end() ; it++)
	{
		Point p1(it->x, it->y);
		Point p2(it->x + it->width, it->y + it->height);
		
		// Draw the rectangle in the input image
		cvRectangle( m_output, p1, p2, CV_RGB(255,0,0), 1, 8, 0 );
        }
}
