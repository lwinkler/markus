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

#ifndef KEYPOINTS_Fast_H
#define KEYPOINTS_Fast_H

#include "ModuleKeyPoints.h"
#include "StreamObject.h"



/**
* @brief Extract different types of keypoints
*/
class KeyPointsFast : public ModuleKeyPoints
{
public:
	class Parameters : public ModuleKeyPoints::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) : ModuleKeyPoints::Parameters(x_name)
		{
			AddParameter(new ParameterInt("threshold", 10, 0, 255, &threshold,"Threshold"));
			AddParameter(new ParameterBool("nonMaxSuppression", 0, 0, 1, &nonMaxSuppression,"if true, non-maximum suppression is applied to detected corners"));
		};
		int threshold;
		bool nonMaxSuppression;
		// type – one of the three neighborhoods as defined in the paper: FastFeatureDetector::TYPE_9_16, FastFeatureDetector::TYPE_7_12, FastFeatureDetector::TYPE_5_8
	};

	explicit KeyPointsFast(ParameterStructure& xr_params);
	virtual ~KeyPointsFast();
	MKCLASS("KeyPointsFast")
	MKCATEG("KeyPoints")
	MKDESCR("Extract keypoints of type FAST")

	void Reset() override;

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
};


#endif
