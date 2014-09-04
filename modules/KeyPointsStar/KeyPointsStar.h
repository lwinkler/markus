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

#ifndef KEYPOINTS_Star_H
#define KEYPOINTS_Star_H

#include "ModuleKeyPoints.h"
#include "StreamObject.h"



/**
* @brief Extract different types of keypoints
*/
class KeyPointsStar : public ModuleKeyPoints
{
public:
	class Parameters : public ModuleKeyPoints::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) : ModuleKeyPoints::Parameters(x_confReader)
		{
			m_list.push_back(new ParameterInt("max_size"                 , 16 , 1 , 1000 , &maxSize              , ""));
			m_list.push_back(new ParameterInt("response_threshold"       , 30 , 1 , 1000 , &responseThreshold      , ""));
			m_list.push_back(new ParameterInt("line_threshold_projected" , 10 , 1 , 1000 , &lineThresholdProjected , ""));
			m_list.push_back(new ParameterInt("line_threshold_binarized" , 8  , 1 , 1000 , &lineThresholdBinarized , ""));
			m_list.push_back(new ParameterInt("suppress_non_max_size"    , 5  , 1 , 1000 , &suppressNonmaxSize     , ""));

			Init();
		};
		int maxSize;
		int responseThreshold;
		int lineThresholdProjected;
		int lineThresholdBinarized;
		int suppressNonmaxSize;
	};

	KeyPointsStar(const ConfigReader& x_configReader);
	~KeyPointsStar();
	MKCLASS("KeyPointsStar")
	MKDESCR("Extract key points of type Star")
	
	inline virtual const Parameters& GetParameters() const { return m_param;}
	void Reset();

private:
	inline virtual Parameters & RefParameters() {return m_param;}
	Parameters m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
};


#endif
