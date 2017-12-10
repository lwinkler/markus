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

#include "KeyPointsOrb.h"
#include "StreamImage.h"
#include "StreamDebug.h"
#include "StreamObject.h"
#include "FeatureOpenCv.h"
#include "opencv2/features2d/features2d.hpp"

//for debug
#include "util.h"

namespace mk {
using namespace cv;
using namespace std;

log4cxx::LoggerPtr KeyPointsOrb::m_logger(log4cxx::Logger::getLogger("KeyPointsOrb"));

KeyPointsOrb::KeyPointsOrb(ParameterStructure& xr_params) :
	ModuleKeyPoints(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params))
{
	m_isUnitTestingEnabled = false; // TODO: error with roi in openCV
};


KeyPointsOrb::~KeyPointsOrb()
{
}

void KeyPointsOrb::Reset()
{
	ModuleKeyPoints::Reset();
	mp_detector = ORB::create(
		m_param.nbFeatures,
		m_param.scaleFactor,
		m_param.nbLevels,
		m_param.edgeThreshold,
		m_param.firstLevel,
		m_param.wta_k,
		ORB::HARRIS_SCORE,
		m_param.edgeThreshold // Note: the 2 params should be roughly equivalent according to the doc
	);
	if(mp_detector.empty())
		throw MkException("Cannot create detector", LOC);
}

} // namespace mk
