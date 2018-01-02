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

#ifndef MK_BACKGROUNDSUBTRACTOR_H
#define MK_BACKGROUNDSUBTRACTOR_H

#include "Module.h"
#include "CreationFunction.h"
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
* @brief Perform background subtraction
*/
class BackgroundSubtractor : public Module
{
public:
	static cv::Ptr<cv::BackgroundSubtractor> create(const CreationFunction& x_funct);
	class Parameters : public Module::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) : Module::Parameters(x_name)
		{
			AddParameter(new ParameterT<CreationFunction>("create", R"({"name": "BackgroundSubtractorMOG2", "number": 0, "parameters":{}})"_json, &create, "The parameters to pass to the create method method of ORB, BRIEF, ..."));
			AddParameter(new ParameterDouble("learningRate",	-1, 	-1, 1, &learningRate,	"Learning rate of the model"));

			RefParameterByName("type").SetDefaultAndValue("CV_8UC3");
			RefParameterByName("type").SetRange(R"({"allowed":["CV_8UC3"]})"_json);
		};
		CreationFunction create;
		double learningRate;
	};

	explicit BackgroundSubtractor(ParameterStructure& xr_params);
	virtual ~BackgroundSubtractor();
	MKCLASS("BackgroundSubtractor")
	MKCATEG("BackgroundSubtraction")
	MKDESCR("Perform background subtraction")

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
	cv::Ptr<cv::BackgroundSubtractor> mp_bsub;
};


} // namespace mk
#endif
