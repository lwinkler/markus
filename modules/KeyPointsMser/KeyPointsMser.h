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

#ifndef KEYPOINTS_MSER_H
#define KEYPOINTS_MSER_H

#include "ModuleKeyPoints.h"
#include "StreamObject.h"


namespace mk {

/**
* @brief Extract different types of keypoints
*/
class KeyPointsMser : public ModuleKeyPoints
{
public:
	class Parameters : public ModuleKeyPoints::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) : ModuleKeyPoints::Parameters(x_name)
		{
			AddParameter(new ParameterInt("delta"             , 5     , 1 , 100     , &delta         , ""));
			AddParameter(new ParameterInt("minArea"          , 60    , 0 , 100     , &minArea       , ""));
			AddParameter(new ParameterInt("maxArea"          , 14400 , 1 , INT_MAX , &maxArea       , ""));
			AddParameter(new ParameterFloat("maxVariation"   , .25   , 0 , 1       , &maxVariation  , ""));
			AddParameter(new ParameterFloat("minDiversity"   , .2    , 0 , 1       , &minDiversity  , ""));
			AddParameter(new ParameterInt("maxEvolution"     , 200   , 1 , 1000    , &maxEvolution  , ""));
			AddParameter(new ParameterDouble("areaThreshold" , 1.01  , 1 , 2       , &areaThreshold , ""));
			AddParameter(new ParameterDouble("minMargin"     , .003  , 0 , 1       , &minMargin     , ""));
			AddParameter(new ParameterInt("edgeBlurSize"    , 5     , 0 , 100     , &edgeBlurSize  , ""));
		};
		int delta;
		int minArea;
		int maxArea;
		float maxVariation;
		float minDiversity;
		int maxEvolution;
		double areaThreshold;
		double minMargin;
		int edgeBlurSize;
	};

	explicit KeyPointsMser(ParameterStructure& xr_params);
	virtual ~KeyPointsMser();
	MKCLASS("KeyPointsMser")
	MKCATEG("KeyPoints")
	MKDESCR("Extract key points of type Mser")

	void Reset() override;

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
};


} // namespace mk
#endif
