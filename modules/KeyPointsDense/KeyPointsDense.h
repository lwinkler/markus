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

#ifndef KEYPOINTS_DENSE_H
#define KEYPOINTS_DENSE_H

#include "ModuleKeyPoints.h"
#include "StreamObject.h"



/**
* @brief Extract different types of keypoints
*/
class KeyPointsDense : public ModuleKeyPoints
{
public:
	class Parameters : public ModuleKeyPoints::Parameters
	{
	public:
		Parameters(const std::string& x_name) : ModuleKeyPoints::Parameters(x_name)
		{
			AddParameter(new ParameterFloat("initFeatureScale", 1,   1, 255, &initFeatureScale, ""));
			// Note: feature_scale_levels is memory consuming. Limited to 2 for unit testing purposes
			AddParameter(new ParameterInt("featureScaleLevels", 1,   1, 2, &featureScaleLevels, "Number of levels of scaling"));
			AddParameter(new ParameterFloat("featureScaleMul" , 0.1, 0, 1, &featureScaleMul, "Multiplication factor between each level"));
			AddParameter(new ParameterInt("initXyStep",         6  , 1, 255, &initXyStep, ""));
			AddParameter(new ParameterInt("initImgBound",       0  , 0, 255, &initImgBound, ""));
			AddParameter(new ParameterBool("varyXyStepWithScale",   8, 0, 1, &varyXyStepWithScale, "The grid node size is multiplied if true"));
			AddParameter(new ParameterBool("varyImgBoundWithScale", 0, 0, 1, &varyImgBoundWithScale, "Size of image boundary is multiplied if true"));
		};
		float initFeatureScale;
		int featureScaleLevels;
		float featureScaleMul;
		int initXyStep;
		int initImgBound;
		bool varyXyStepWithScale;
		bool varyImgBoundWithScale;
	};

	KeyPointsDense(ParameterStructure& xr_params);
	virtual ~KeyPointsDense();
	MKCLASS("KeyPointsDense")
	MKCATEG("KeyPoints")
	MKDESCR("Extract key points of type Dense")

	virtual void Reset() override;

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
};


#endif
