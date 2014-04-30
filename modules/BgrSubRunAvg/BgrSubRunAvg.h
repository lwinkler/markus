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
		m_list.push_back(new ParameterFloat("background_alpha",	0.02, 	0, 1,	&backgroundAlpha,	"Defines the speed at which the background adapts"));
		m_list.push_back(new ParameterFloat("foreground_thres", 	0.2, 	0, 1,	&foregroundThres,	"Threshold to accept a pixel as foreground"));

		RefParameterByName("type").SetDefault("CV_32FC3");

		ParameterStructure::Init();
	};
	float backgroundAlpha;
	float foregroundThres;
};

/**
* @brief Perform a background subtraction using a running average
*/
class BgrSubRunAvg : public Module
{
public:
	BgrSubRunAvg(const ConfigReader& x_configReader);
	~BgrSubRunAvg();
	virtual const std::string& GetClass() const {static std::string cl = "BgrSubRunAvg"; return cl;}
	virtual const std::string& GetDescription() const {static std::string descr = "Perform a background subtraction using a running average"; return descr;}
	
	virtual void ProcessFrame();
	void Reset();
private:
	BgrSubRunAvgParameterStructure m_param;
	inline virtual const ModuleParameterStructure& GetParameters() const { return m_param;}
	static log4cxx::LoggerPtr m_logger;
protected:
	// input
	cv::Mat m_input;

	// output
	cv::Mat m_background;
	cv::Mat m_foreground;

	// temporary
	cv::Mat m_foreground_tmp;

	// state variables
	bool m_emptyBackgroundSubtractor;
	cv::Mat * m_accumulator;
};


#endif
