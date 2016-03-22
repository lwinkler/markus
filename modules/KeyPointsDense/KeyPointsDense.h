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
		Parameters(const ConfigReader& x_confReader) : ModuleKeyPoints::Parameters(x_confReader)
		{
			m_list.push_back(new ParameterFloat("init_feature_scale", 1,   1, 255, &initFeatureScale, ""));
			// Note: feature_scale_levels is memory consuming. Limited to 2 for unit testing purposes
			m_list.push_back(new ParameterInt("feature_scale_levels", 1,   1, 2, &featureScaleLevels, "Number of levels of scaling"));
			m_list.push_back(new ParameterFloat("feature_scale_mul" , 0.1, 0, 1, &featureScaleMul, "Multiplication factor between each level"));
			m_list.push_back(new ParameterInt("init_xy_step",         6  , 1, 255, &initXyStep, ""));
			m_list.push_back(new ParameterInt("init_img_bound",       0  , 0, 255, &initImgBound, ""));
			m_list.push_back(new ParameterBool("vary_xy_step_with_scale",   8, 0, 1, &varyXyStepWithScale, "The grid node size is multiplied if true"));
			m_list.push_back(new ParameterBool("vary_img_bound_with_scale", 0, 0, 1, &varyImgBoundWithScale, "Size of image boundary is multiplied if true"));


			Init();
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
