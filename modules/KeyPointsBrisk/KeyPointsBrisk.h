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

#ifndef KEYPOINTS_BRISK_H
#define KEYPOINTS_BRISK_H

#include "ModuleKeyPoints.h"
#include "StreamObject.h"



/**
* @brief Extract different types of keypoints
*/
class KeyPointsBrisk : public ModuleKeyPoints
{
public:
	class Parameters : public ModuleKeyPoints::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) : ModuleKeyPoints::Parameters(x_name)
		{
			AddParameter(new ParameterInt("threshold"       , 30 , 1 , 255 , &threshold    , ""));
			AddParameter(new ParameterInt("octaves"         , 3  , 1 , 100 , &octaves      , ""));
			AddParameter(new ParameterFloat("patternScale" , 1  , 0 , 10  , &patternScale , ""));

			// Limit size to accelerate unit tests
			RefParameterByName("width").SetRange(R"({"min":64, "max":1280})"_json);
			RefParameterByName("height").SetRange(R"({"min":48, "max":960})"_json);
			RefParameterByName("octaves").SetRange(R"({"min":1, "max":3})"_json);
		};
		int threshold;
		int octaves;
		float patternScale;
	};

	explicit KeyPointsBrisk(ParameterStructure& xr_params);
	virtual ~KeyPointsBrisk();
	MKCLASS("KeyPointsBrisk")
	MKCATEG("KeyPoints")
	MKDESCR("Extract key points of type Brisk")

	void Reset() override;

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
};


#endif
