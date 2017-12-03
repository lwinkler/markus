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

#ifndef KEYPOINTS_ORB_H
#define KEYPOINTS_ORB_H

#include "ModuleKeyPoints.h"
#include "StreamObject.h"



/**
* @brief Extract different types of keypoints
*/
class KeyPointsOrb : public ModuleKeyPoints
{
public:
	class Parameters : public ModuleKeyPoints::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) : ModuleKeyPoints::Parameters(x_name)
		{
			AddParameter(new ParameterInt("nbFeatures"    , 500 , 1 , 1000 , &nbFeatures    , ""));
			AddParameter(new ParameterFloat("scaleFactor" , 1.2 , 1 , 1.2  , &scaleFactor   , ""));
			AddParameter(new ParameterInt("nbLevels"      , 8   , 1 , 10   , &nbLevels      , ""));
			AddParameter(new ParameterInt("edgeThreshold" , 31  , 2 , 100  , &edgeThreshold , ""));
			AddParameter(new ParameterInt("firstLevel"    , 0   , 0 , 5    , &firstLevel    , ""));
			AddParameter(new ParameterInt("wtaK"          , 2   , 0 , 100  , &wta_k         , ""));

			// Limit size to accelerate unit tests
			RefParameterByName("width").SetRange(R"({"min":64, "max":1280})"_json);
			RefParameterByName("height").SetRange(R"({"min":48, "max":960})"_json);
		};
		int nbFeatures;
		float scaleFactor;
		int nbLevels;
		int edgeThreshold;
		int firstLevel;
		int wta_k;
		// int scoreType=ORB::HARRIS_SCORE;
		// int patchSize=31 =~ edgeThreshold
	};

	explicit KeyPointsOrb(ParameterStructure& xr_params);
	virtual ~KeyPointsOrb();
	MKCLASS("KeyPointsOrb")
	MKCATEG("KeyPoints")
	MKDESCR("Extract key points of type Orb")

	void Reset() override;

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
};


#endif
