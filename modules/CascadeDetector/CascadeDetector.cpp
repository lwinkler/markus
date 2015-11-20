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

log4cxx::LoggerPtr CascadeDetector::m_logger(log4cxx::Logger::getLogger("CascadeDetector"));

CascadeDetector::CascadeDetector(ParameterStructure& xr_params)
	: Module(xr_params),
	  m_param(dynamic_cast<Parameters&>(xr_params)),
	  m_input(Size(m_param.width, m_param.height), m_param.type)
{
	// Init output images
	if(! m_cascade.load( m_param.filterFile ) || m_cascade.empty())
		throw MkException("Impossible to load cascade filter " + m_param.filterFile, LOC);

	AddInputStream(0, new StreamImage("input", m_input, *this, 		"Video input"));

	AddOutputStream(0, new StreamObject("detected", m_detectedObjects, *this,	"Detected objects"));
#ifdef MARKUS_DEBUG_STREAMS
	m_debug = Mat(Size(m_param.width, m_param.height), CV_8UC3);
	AddDebugStream(0, new StreamDebug("debug", m_debug, *this,		""));
#endif
}

CascadeDetector::~CascadeDetector()
{
}

void CascadeDetector::Reset()
{
	Module::Reset();
}

void CascadeDetector::ProcessFrame()
{
	Mat smallImg(m_input);
	equalizeHist( smallImg, smallImg );

	// Detection
	std::vector<cv::Rect> detected;
	m_cascade.detectMultiScale(smallImg, detected, m_param.scaleFactor, m_param.minNeighbors, CV_HAAR_SCALE_IMAGE, Size(m_param.minSide, m_param.minSide));

	m_detectedObjects.clear();
	const double diagonal = sqrt(m_param.width * m_param.width + m_param.height * m_param.height);
	for(const auto& elem : detected)
	{
		Object obj(m_param.objectLabel, elem);

		obj.AddFeature("x"      , obj.posX   / diagonal);
		obj.AddFeature("y"      , obj.posY   / diagonal);
		obj.AddFeature("width"  , obj.width  / diagonal);
		obj.AddFeature("height" , obj.height / diagonal);

		m_detectedObjects.push_back(obj);
	}

#ifdef MARKUS_DEBUG_STREAMS
	cvtColor(m_input, m_debug, CV_GRAY2RGB);

	for(const auto& obj : m_detectedObjects)
	{
		// Draw the rectangle in the input image
		rectangle(m_debug, obj.GetRect(), Scalar(255, 0, 23)/*colorFromStr(m_param.color)*/, 1, 8, 0 );
	}
#endif
}
