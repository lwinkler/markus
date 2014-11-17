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
#ifndef MK_FEATURE_OPENCV_H
#define MK_FEATURE_OPENCV_H

#include "opencv2/features2d/features2d.hpp"
#include "feature_util.h"
#include "FeatureT.h"

/**
* @brief This file contains all features (templates) that come from OpenCV classes
*/


// Definitions
typedef FeatureT<cv::KeyPoint>	  FeatureKeyPoint;
typedef FeatureT<cv::Point3f>	  FeaturePoint3f;
// typedef FeatureT<cv::Mat>	  FeatureMat;
#endif
