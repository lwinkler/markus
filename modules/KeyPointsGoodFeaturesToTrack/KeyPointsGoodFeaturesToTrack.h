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

#ifndef KEYPOINTS_GOODFEATURESTOTRACK_H
#define KEYPOINTS_GOODFEATURESTOTRACK_H

#include "ModuleKeyPoints.h"
#include "StreamObject.h"



/**
* @brief Extract different types of keypoints
*/
class KeyPointsGoodFeaturesToTrack : public ModuleKeyPoints
{
public:
	class Parameters : public ModuleKeyPoints::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) : ModuleKeyPoints::Parameters(x_name)
		{
			AddParameter(new ParameterInt("maxCorners", 1000, 1, INT_MAX, &maxCorners, "Maximum number of corners to return. If there are more corners than are found, the strongest of them is returned."));
			AddParameter(new ParameterDouble("qualityLevel", 0.01, 0.01, 1, &qualityLevel,"Parameter characterizing the minimal accepted quality of image corners."));
			AddParameter(new ParameterDouble("minDist", 1, 1, MAX_WIDTH, &minDistance,"Minimum possible Euclidean distance between the returned corners"));
			AddParameter(new ParameterInt("blockSize", 3, 1, MAX_WIDTH, &blockSize,"Size of an average block for computing a derivative covariation matrix over each pixel neighborhood"));
			AddParameter(new ParameterBool("useHarrisDetector", 0, 0, 1, &useHarrisDetector,"Parameter indicating whether to use a Harris detector"));
			AddParameter(new ParameterDouble("k", 0.04, 0, 1, &k,"Free parameter of the Harris detector"));
		};
		int maxCorners;
		double qualityLevel;
		double minDistance;
		int blockSize;
		bool useHarrisDetector;
		double k;
	};

	explicit KeyPointsGoodFeaturesToTrack(ParameterStructure& xr_params);
	virtual ~KeyPointsGoodFeaturesToTrack();
	MKCLASS("KeyPointsGoodFeaturesToTrack")
	MKCATEG("KeyPoints")
	MKDESCR("Extract keypoints of type GoodFeaturesToTrack on image")

	void Reset() override;

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
};


#endif
