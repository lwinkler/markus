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

#ifndef BACKGROUNDEXTRACTOR_H
#define BACKGROUNDEXTRACTOR_H

#include "Module.h"
#include "Detector.h"

class ConfigReader;

class BackgroundExtractorParameterStructure : public ModuleParameterStructure
{
public:
	BackgroundExtractorParameterStructure(const ConfigReader& x_confReader) : ModuleParameterStructure(x_confReader)
	{
		//m_list.push_back(new ParameterInt(0, "input_blur_size",	1, 	PARAM_INT, 1, 7,	&detector.inputBlurSize,	"Strength of the blur applied to the input"));
		m_list.push_back(new ParameterFloat(0, "background_alpha",	0.02, 	PARAM_FLOAT, 0, 1,	&detector.backgroundAlpha,	"Defines the speed at which the background adapts"));
		m_list.push_back(new ParameterFloat(0, "foreground_thres", 	0.2, 	PARAM_FLOAT, 0, 1,	&detector.foregroundThres,	"Threshold to accept a pixel as foreground"));
		m_list.push_back(new ParameterInt(0, "foreground_filter_size", 3, 	PARAM_INT, 1, 7,	&detector.foregroundFilterSize,	"Size of the filter to remove noise"));

		RefParameterByName("type").SetDefault("CV_32FC3");

		ParameterStructure::Init();
	};
	DetectorParameter detector;
};

class BackgroundExtractor : public Module
{
public:
	BackgroundExtractor(const ConfigReader& x_configReader);
	~BackgroundExtractor();
	
	virtual void ProcessFrame();
	void Reset();
		
private:
	BackgroundExtractorParameterStructure m_param;
	inline virtual ModuleParameterStructure& RefParameter() { return m_param;};

protected:
	cv::Mat * m_input;
	cv::Mat * m_output;
	
	Detector detect;
	static const char * m_type;
};


#endif
