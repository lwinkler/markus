/*----------------------------------------------------------------------------------
 *
 *    MARKUS : a manager for video analysis modules
 *
 *    author : Laurent Winkler and Florian Rossier <florian.rossier@gmail.com>
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

#include "KeyPointsFast.h"
#include "StreamImage.h"
#include "StreamDebug.h"
#include "StreamObject.h"
#include "FeatureOpenCv.h"
#include "opencv2/features2d/features2d.hpp"

//for debug
#include "util.h"

using namespace cv;
using namespace std;

log4cxx::LoggerPtr KeyPointsFast::m_logger(log4cxx::Logger::getLogger("KeyPointsFast"));

KeyPointsFast::KeyPointsFast(ParameterStructure& xr_params) :
	ModuleKeyPoints(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params))
{
};


KeyPointsFast::~KeyPointsFast()
{
}

void KeyPointsFast::Reset()
{
	ModuleKeyPoints::Reset();
	CLEAN_DELETE(mp_detector);
	mp_detector = cv::FastFeatureDetector::create(m_param.threshold, m_param.nonMaxSuppression);
	// mp_detector = Algorithm::create<Feature2D>("Feature2D.Fast");
	if(mp_detector == nullptr && mp_detector->empty())
		throw MkException("Cannot create detector", LOC);
}

