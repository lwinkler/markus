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


/**
* @brief Perform background subtraction via Mixtures Of Gaussians (OpenCV MOG2)
*/
class BgrSubMOG2 : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) : Module::Parameters(x_confReader)
		{

			// This parameters should not change
			AddParameter(new ParameterInt  ("history",	500, 	1, 10000,	&history,	"Length of the history"));
			AddParameter(new ParameterFloat("var_thres",	16, 	1, 255,	&varThres,	"Threshold on the squared Mahalanobis distance to decide whether it is well described by the background model (selectivity of background) "));
			AddParameter(new ParameterBool  ("b_shadow_detection",	false, 	0, 1, &bShadowDetection,	"Enable shadow detection"));
			AddParameter(new ParameterDouble("learning_rate",	-1, 	-1, 1, &learningRate,	"Learning rate of the model"));

			RefParameterByName("type").SetDefault("CV_8UC3");
			RefParameterByName("type").SetRange("[CV_8UC3]");
		};
		int history;
		float varThres;
		bool bShadowDetection;
		double learningRate;
	};

	BgrSubMOG2(ParameterStructure& xr_params);
	virtual ~BgrSubMOG2();
	MKCLASS("BgrSubMOG2")
	MKCATEG("BackgroundSubtraction")
	MKDESCR("Perform background subtraction via Mixtures Of Gaussians (OpenCV MOG2)")

	virtual void ProcessFrame() override;
	virtual void Reset() override;

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	// input
	cv::Mat m_input;

	// output
	cv::Mat m_foreground;
	cv::Mat m_background;

	// temporary
	cv::Mat m_foregroundWithShadows;

	// state variables
	cv::BackgroundSubtractorMOG2* mp_mog2;
};


#endif
