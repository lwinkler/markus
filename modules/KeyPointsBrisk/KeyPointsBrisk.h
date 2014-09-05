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
		Parameters(const ConfigReader& x_confReader) : ModuleKeyPoints::Parameters(x_confReader)
		{
			m_list.push_back(new ParameterInt("threshold"             , 30     , 1 , 255     , &threshold         , ""));
			m_list.push_back(new ParameterInt("octaves"          , 3    , 1 , 100     , &octaves       , ""));
			m_list.push_back(new ParameterFloat("pattern_scales"   , 1   , 0 , 10       , &patternScale  , ""));

			Init();
		};
		int threshold;
		int octaves;
		float patternScale;
	};

	KeyPointsBrisk(const ConfigReader& x_configReader);
	~KeyPointsBrisk();
	MKCLASS("KeyPointsBrisk")
	MKDESCR("Extract key points of type Brisk")
	
	inline virtual const Parameters& GetParameters() const { return m_param;}
	void Reset();

private:
	inline virtual Parameters & RefParameters() {return m_param;}
	Parameters m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
};


#endif
