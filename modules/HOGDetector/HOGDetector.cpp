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

// #include <opencv2/highgui/highgui.hpp>
// #include "util.h"

namespace mk {
using namespace std;
using namespace cv;

log4cxx::LoggerPtr HOGDetector::m_logger(log4cxx::Logger::getLogger("HOGDetector"));

HOGDetector::HOGDetector(ParameterStructure& xr_params)
	: Module(xr_params), m_param(dynamic_cast<Parameters&>(xr_params)),
	  m_input(Size(m_param.width, m_param.height), m_param.type)
{
	AddInputStream(0, new StreamImage("image", m_input, *this, 		"Video input"));

	AddOutputStream(0, new StreamObject("detected", m_detectedObjects, /*colorFromStr(m_param.color),*/ *this,	"Detected objects"));
#ifdef MARKUS_DEBUG_STREAMS
	m_debug = Mat(Size(m_param.width, m_param.height), CV_8UC3);
	AddDebugStream(0, new StreamDebug("debug", m_debug, *this,		"Debug image"));
#endif
}

HOGDetector::~HOGDetector()
{
}

void HOGDetector::Reset()
{
	Module::Reset();
	m_hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
}

// This method launches the thread
void HOGDetector::ProcessFrame()
{
	Mat smallImg(m_input);
	// equalizeHist( smallImg, smallImg );

	std::vector<cv::Rect> detected;

	m_hog.detectMultiScale(smallImg, detected, 0, Size(8,8), Size(32,32), m_param.scaleFactor, 2);

	const double diagonal = sqrt(m_param.width * m_param.width + m_param.height * m_param.height);
	m_detectedObjects.clear();
	for(const auto & elem : detected)
	{
		Object obj(m_param.objectLabel, elem);

		obj.AddFeature("x"      , new FeatureFloat(obj.posX   / diagonal));
		obj.AddFeature("y"      , new FeatureFloat(obj.posY   / diagonal));
		obj.AddFeature("width"  , new FeatureFloat(obj.width  / diagonal));
		obj.AddFeature("height" , new FeatureFloat(obj.height / diagonal));

		m_detectedObjects.push_back(obj);
	}

#ifdef MARKUS_DEBUG_STREAMS
	m_input.copyTo(m_debug);

	for(const auto& elem : m_detectedObjects)
	{
		// Draw the rectangle in the input image
		rectangle(m_debug, elem.GetRect(), Scalar(255, 0, 33), 1, 8, 0 );
	}
#endif
}

} // namespace mk
