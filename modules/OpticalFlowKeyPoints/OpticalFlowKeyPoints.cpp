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

#include "OpticalFlowKeyPoints.h"
#include "StreamImage.h"
#include "StreamDebug.h"
#include "StreamObject.h"
#include "FeatureOpenCv.h"

//for debug
#include "util.h"

using namespace cv;
using namespace std;

// Note: This has been partly inspired from lkdemo.cpp in OpenCV samples

log4cxx::LoggerPtr OpticalFlowKeyPoints::m_logger(log4cxx::Logger::getLogger("OpticalFlowKeyPoints"));

OpticalFlowKeyPoints::OpticalFlowKeyPoints(const ConfigReader& x_configReader) :
	Module(x_configReader),
	m_param(x_configReader),
	m_input(Size(m_param.width, m_param.height), m_param.type)
{
	AddInputStream(0, new StreamImage("image",       m_input,        *this,   "Video input"));
	AddInputStream(1, new StreamObject("keypoints",  m_keyPointsIn,  *this,   "List of key points"));

	AddOutputStream(0, new StreamObject("keypoints", m_keyPointsIn,  *this,   "List of key points with optical flow")); // TODO: Add optical flow or matching keypoint to the output

#ifdef MARKUS_DEBUG_STREAMS
	m_debug = Mat(Size(m_param.width, m_param.height), CV_8UC3);
	AddDebugStream(0, new StreamDebug("flow", m_debug, *this, "Visualization of the optical flow"));
#endif
};


OpticalFlowKeyPoints::~OpticalFlowKeyPoints()
{
}

void OpticalFlowKeyPoints::Reset()
{
	Module::Reset();
	m_lastPoints.clear();
	m_lastImg = Mat();
}

void OpticalFlowKeyPoints::ProcessFrame()
{
	// static const TermCriteria termcrit(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 20, 0.03);
	static const Size subPixWinSize(10,10);

	if(m_keyPointsIn.size() == 0)
		return;
		
	// Convert all keypoints to a vector of Point2f
	vector<Point2f> pointsIn;
	for(vector<Object>::iterator itkp = m_keyPointsIn.begin() ; itkp != m_keyPointsIn.end() ; ++itkp)
	{
		// KeyPoint keypoint = dynamic_cast<const FeatureKeyPoint&>(itkp->GetFeature("keypoint")).keypoint;
		// pointsIn.push_back(keypoint.pt);
		pointsIn.push_back(Point2f(itkp->posX, itkp->posY)); // TODO Add to object class ?
	}

	// If just initialized
	if(m_lastImg.empty())
	{
		m_input.copyTo(m_lastImg);
		m_lastPoints = pointsIn;
	}
	else
	{
		vector<uchar> status;
		vector<float> err;
		// cornerSubPix(m_input, pointsIn, subPixWinSize, Size(-1,-1), termcrit);
		vector<Point2f> pointsOut;

		// match points of the old point list with the current image
		// the matching keypoints correspond with the old points !
		// calcOpticalFlowPyrLK(m_lastImg, m_input, m_lastPoints, pointsOut, status, err, Size(m_param.winSide, m_param.winSide), m_param.maxLevel);
		
		// match points of the current point list with the old image
		// in some way we compute optical flow in reverse: we inverted previous and next lists of points compared to standard usage
		calcOpticalFlowPyrLK(m_input, m_lastImg, pointsIn, pointsOut, status, err, Size(m_param.winSide, m_param.winSide), m_param.maxLevel);

#ifdef MARKUS_DEBUG_STREAMS
		//draw a line between this to point to show the OF of these points
		adjustChannels(m_input, m_debug);
		int cpt = 0;
		for(size_t i = 0 ; i < status.size() ; i++)
		{
			if(status[i] == 1)
			{
				//point have been found draw it in green
				line(m_debug, pointsIn[i], pointsOut[i], Scalar(12, 233, 0));
				ellipse(m_debug, RotatedRect(pointsIn[i], Size(3, 3), 0), Scalar(33, 233, 0));
				cpt++;
			}
		}
		LOG_DEBUG(m_logger, "Computed optical flow for "<<cpt<<" keypoints");
#endif
	}

	// swap(m_input, m_lastImg);
	// swap(m_lastPoints, pointsIn);
	m_input.copyTo(m_lastImg);
	m_lastPoints = pointsIn;
}
