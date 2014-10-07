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

#include "KeyPointsDense.h"
#include "StreamImage.h"
#include "StreamDebug.h"
#include "StreamObject.h"
#include "FeatureKeyPoint.h"

//for debug
#include "util.h"

using namespace cv;
using namespace std;

log4cxx::LoggerPtr KeyPointsDense::m_logger(log4cxx::Logger::getLogger("KeyPointsDense"));

KeyPointsDense::KeyPointsDense(const ConfigReader& x_configReader) :
	ModuleKeyPoints(x_configReader),
	m_param(x_configReader)
{
};


KeyPointsDense::~KeyPointsDense()
{
}

void KeyPointsDense::Reset()
{
	ModuleKeyPoints::Reset();
	CLEAN_DELETE(mp_detector);
        mp_detector = new DenseFeatureDetector(
		m_param.initFeatureScale,
		m_param.featureScaleLevels,
		m_param.featureScaleMul,
		m_param.initXyStep,
		m_param.initImgBound,
		m_param.varyXyStepWithScale,
		m_param.varyImgBoundWithScale
        );
	if(mp_detector == NULL && mp_detector->empty())
		throw MkException("Cannot create detector", LOC);
}
