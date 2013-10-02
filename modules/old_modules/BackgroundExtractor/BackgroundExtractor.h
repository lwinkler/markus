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

class ConfigReader;


class BackgroundExtractorParameterStructure : public ModuleParameterStructure
{
public:
	BackgroundExtractorParameterStructure(const ConfigReader& x_confReader) : ModuleParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterFloat("background_alpha",	0.02, 	PARAM_FLOAT, 0, 1,	&backgroundAlpha,	"Defines the speed at which the background adapts"));
		m_list.push_back(new ParameterFloat("foreground_thres", 	0.2, 	PARAM_FLOAT, 0, 1,	&foregroundThres,	"Threshold to accept a pixel as foreground"));
		m_list.push_back(new ParameterInt("foreground_filter_size", 3, 	PARAM_INT, 1, 7,	&foregroundFilterSize,	"Size of the filter to remove noise"));

		RefParameterByName("type").SetDefault("CV_32FC3");

		ParameterStructure::Init();
	};
	float backgroundAlpha;
	float foregroundThres;
	int foregroundFilterSize;
	int backgroundDepth;
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

	// Background subtraction	
#ifdef MARKUS_DEBUG_STREAMS
	cv::Mat* m_foreground;
#endif
	cv::Mat* m_foreground_rff;
	cv::Mat* m_background;
	cv::Mat* m_foreground_tmp1;
	cv::Mat* m_foreground_tmp2;
	cv::Mat* m_elementRemoveNoiseForeground;
	bool m_emptyBackgroundSubtraction;
	void UpdateBackground(cv::Mat* img);
	void UpdateBackgroundMask(cv::Mat* img, cv::Mat* x_mask);
	void ExtractForeground(cv::Mat* img);
	void ExtractForegroundMax(cv::Mat* img);
	void RemoveFalseForegroundNeigh();
	void RemoveNoiseForeground();

protected:
	cv::Mat * m_input;
	cv::Mat * m_output;
	
	static const char * m_type;
};


#endif
