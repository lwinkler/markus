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

#ifndef BACKGROUNDSUBTRACTORMOG2_H
#define BACKGROUNDSUBTRACTORMOG2_H

#include "Module.h"
#include "opencv2/video/background_segm.hpp"


// default parameters of gaussian background detection algorithm from opencv
/*static const int defaultHistory2 = 500; // Learning rate; alpha = 1/defaultHistory2
static const float defaultVarThreshold2 = 4.0f*4.0f;
static const int defaultNMixtures2 = 5; // maximal number of Gaussians in mixture
static const float defaultBackgroundRatio2 = 0.9f; // threshold sum of weights for background test
static const float defaultVarThresholdGen2 = 3.0f*3.0f;
static const float defaultVarInit2 = 15.0f; // initial variance for new components
static const float defaultVarMax2 = 5*defaultVarInit2;
static const float defaultVarMin2 = 4.0f;
*/


class ConfigReader;


class BgrSubMOG2ParameterStructure : public ModuleParameterStructure
{
public:
	BgrSubMOG2ParameterStructure(const ConfigReader& x_confReader) : ModuleParameterStructure(x_confReader)
	{

		// This parameters should not change
		m_list.push_back(new ParameterInt  ("history",	500, 	1, 10000,	&history,	"Length of the history"));
		m_list.push_back(new ParameterFloat("var_thres",	16, 	1, 255,	&varThres,	"Threshold on the squared Mahalanobis distance to decide whether it is well described by the background model (selectivity of background) "));
		m_list.push_back(new ParameterBool  ("b_shadow_detection",	false, 	0, 1, &bShadowDetection,	"Enable shadow detection"));
		m_list.push_back(new ParameterDouble("learning_rate",	-1, 	-1, 1, &learningRate,	"Learning rate of the model"));

		RefParameterByName("type").SetDefault("CV_8UC3");


		ParameterStructure::Init();
	};
	int history;
	float varThres;
	bool bShadowDetection;

	double learningRate;
};

class BgrSubMOG2 : public Module
{
public:
	BgrSubMOG2(const ConfigReader& x_configReader);
	~BgrSubMOG2();
	
	virtual void ProcessFrame();
	void Reset();
private:
	BgrSubMOG2ParameterStructure m_param;
	inline virtual const ModuleParameterStructure& GetParameters() const { return m_param;};
	static log4cxx::LoggerPtr m_logger;
protected:
	cv::Mat m_input;
	// Background subtraction	
	cv::Mat m_foreground;
	cv::Mat m_background;
	cv::Mat m_foregroundWithShadows;
	//bool m_emptyBackgroundSubtractor;

	cv::BackgroundSubtractorMOG2* mp_mog2;
};


#endif
