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

#ifndef BGRSUBRUNAVG_H
#define BGRSUBRUNAVG_H

#include "Module.h"

class ConfigReader;


class BgrSubRunAvgParameterStructure : public ModuleParameterStructure
{
public:
	BgrSubRunAvgParameterStructure(const ConfigReader& x_confReader) : ModuleParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterFloat(0, "background_alpha",	0.02, 	PARAM_FLOAT, 0, 1,	&backgroundAlpha,	"Defines the speed at which the background adapts"));
		m_list.push_back(new ParameterFloat(0, "foreground_thres", 	0.2, 	PARAM_FLOAT, 0, 1,	&foregroundThres,	"Threshold to accept a pixel as foreground"));

		RefParameterByName("type").SetDefault("CV_32FC3");

		ParameterStructure::Init();
	};
	float backgroundAlpha;
	float foregroundThres;
};

class BgrSubRunAvg : public Module
{
public:
	BgrSubRunAvg(const ConfigReader& x_configReader);
	~BgrSubRunAvg();
	
	virtual void ProcessFrame();
	void Reset();
		
private:
	BgrSubRunAvgParameterStructure m_param;
	inline virtual ModuleParameterStructure& RefParameter() { return m_param;};

	// Background subtraction	
	cv::Mat* m_foreground;
	cv::Mat* m_foreground_tmp;
	cv::Mat* m_background;
	bool m_emptyBackgroundSubtractor;

protected:
	cv::Mat * m_input;
	cv::Mat * m_tmp1;
	cv::Mat * m_tmp2;
	
	static const char * m_type;
};


#endif
