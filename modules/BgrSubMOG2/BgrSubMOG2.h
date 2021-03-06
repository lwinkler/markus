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

namespace mk {

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
		explicit Parameters(const std::string& x_name) : Module::Parameters(x_name)
		{

			// This parameters should not change
			AddParameter(new ParameterInt  ("history",	500, 	1, 10000,	&history,	"Length of the history"));
			AddParameter(new ParameterFloat("varThres",	16, 	1, 255,	&varThres,	"Threshold on the squared Mahalanobis distance to decide whether it is well described by the background model (selectivity of background) "));
			AddParameter(new ParameterBool  ("bShadowDetection",	0, 	0, 1, &bShadowDetection,	"Enable shadow detection"));
			AddParameter(new ParameterDouble("learningRate",	-1, 	-1, 1, &learningRate,	"Learning rate of the model"));

			RefParameterByName("type").SetDefaultAndValue("CV_8UC3");
			RefParameterByName("type").SetRange(R"({"allowed":["CV_8UC3"]})"_json);
		};
		int history;
		float varThres;
		bool bShadowDetection;
		double learningRate;
	};

	explicit BgrSubMOG2(ParameterStructure& xr_params);
	~BgrSubMOG2() override;
	MKCLASS("BgrSubMOG2")
	MKCATEG("BackgroundSubtraction")
	MKDESCR("Perform background subtraction via Mixtures Of Gaussians (OpenCV MOG2)")

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	void ProcessFrame() override;
	void Reset() override;

	// input
	cv::Mat m_input;

	// output
	cv::Mat m_foreground;
	cv::Mat m_background;

	// temporary
	cv::Mat m_foregroundWithShadows;

	// state variables
	cv::Ptr<cv::BackgroundSubtractorMOG2> mp_mog2;
};


} // namespace mk
#endif
