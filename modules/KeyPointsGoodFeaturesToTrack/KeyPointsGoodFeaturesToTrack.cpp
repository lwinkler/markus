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

#include "KeyPointsGoodFeaturesToTrack.h"
#include "StreamImage.h"
#include "StreamDebug.h"
#include "StreamObject.h"
#include "FeatureOpenCv.h"

//for debug
#include "util.h"

using namespace cv;
using namespace std;

log4cxx::LoggerPtr KeyPointsGoodFeaturesToTrack::m_logger(log4cxx::Logger::getLogger("KeyPointsGoodFeaturesToTrack"));

KeyPointsGoodFeaturesToTrack::KeyPointsGoodFeaturesToTrack(const ConfigReader& x_configReader) :
	ModuleKeyPoints(x_configReader),
	m_param(x_configReader)
{
};


KeyPointsGoodFeaturesToTrack::~KeyPointsGoodFeaturesToTrack()
{
}

void KeyPointsGoodFeaturesToTrack::Reset()
{
	ModuleKeyPoints::Reset();
	CLEAN_DELETE(mp_detector);
	mp_detector = new GoodFeaturesToTrackDetector(
		m_param.maxCorners,
		m_param.qualityLevel,
		m_param.minDistance,
		m_param.blockSize,
		m_param.useHarrisDetector,
		m_param.k);

	if(mp_detector == nullptr && mp_detector->empty())
		throw MkException("Cannot create detector", LOC);
}

