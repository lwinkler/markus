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



/**
* @brief Extract different types of keypoints
*/
class KeyPointsMser : public ModuleKeyPoints
{
public:
	class Parameters : public ModuleKeyPoints::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) : ModuleKeyPoints::Parameters(x_confReader)
		{
			m_list.push_back(new ParameterInt("delta"             , 1   , 1 , 100 , &delta         , ""));
			m_list.push_back(new ParameterInt("min_area"          , 1   , 1 , 100 , &minArea       , ""));
			m_list.push_back(new ParameterInt("max_area"          , 1   , 1 , 100 , &maxArea       , ""));
			m_list.push_back(new ParameterFloat("max_variation"   , 1   , 1 , 255 , &maxVariation  , ""));
			m_list.push_back(new ParameterFloat("min_diversity"   , 0.1 , 0 , 1   , &minDiversity  , ""));
			m_list.push_back(new ParameterInt("max_evolution"     , 6   , 1 , 255 , &maxEvolution  , ""));
			m_list.push_back(new ParameterDouble("area_threshold" , 0   , 0 , 255 , &areaThreshold , ""));
			m_list.push_back(new ParameterDouble("min_margin"     , 1   , 0 , 1   , &minMargin     , ""));
			m_list.push_back(new ParameterInt("edge_blur_size"    , 0   , 0 , 1   , &edgeBlurSize  , ""));


			Init();
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

	KeyPointsMser(const ConfigReader& x_configReader);
	~KeyPointsMser();
	MKCLASS("KeyPointsMser")
	MKDESCR("Extract key points of type Mser")
	
	inline virtual const Parameters& GetParameters() const { return m_param;}
	void Reset();

private:
	inline virtual Parameters & RefParameters() {return m_param;}
	Parameters m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
};


#endif
