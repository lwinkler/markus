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

#ifndef BACKGROUNDSUBTRACTORMOG_H
#define BACKGROUNDSUBTRACTORMOG_H

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
* @brief Perform background subtraction via Mixtures Of Gaussians (OpenCV MOG1)
*/
class BgrSubMOG : public Module
{
	class Parameters : public Module::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) : Module::Parameters(x_confReader)
		{
			// This parameters should not change
			m_list.push_back(new ParameterInt   ("history",          200,    1,   10000,     &history,         "Length of the history"));
			m_list.push_back(new ParameterInt   ("nmixtures",          5,    1,      10,     &nmixtures,       "Number of Gaussian mixtures"));
			m_list.push_back(new ParameterDouble("background_ratio", 0.7,    0,       1,     &backgroundRatio, "Background ratio"));
			m_list.push_back(new ParameterDouble("noise_sigma",       15,    0,    1000,     &noiseSigma,      "noise strength"));
			m_list.push_back(new ParameterDouble("learning_rate",     -1,    -1,       1,     &learningRate,    "Learning rate of the model"));

			Init();
			RefParameterByName("type").SetRange("[CV_8UC1,CV_8UC3]");
		};
		int history;
		int nmixtures;
		double backgroundRatio;
		double noiseSigma;

		double learningRate;
	};

public:
	BgrSubMOG(const ConfigReader& x_configReader);
	~BgrSubMOG();
	MKCLASS("BgrSubMOG")
	MKDESCR("Perform background subtraction via Mixtures Of Gaussians (OpenCV MOG1)")

	inline virtual const Parameters& GetParameters() const {return m_param;}
	virtual void ProcessFrame();
	void Reset();

private:
	inline virtual Parameters & RefParameters() {return m_param;}
	Parameters m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	// input
	cv::Mat m_input;

	// state variables
	cv::BackgroundSubtractorMOG* mp_mog;

	// output
	cv::Mat m_foreground;
	cv::Mat m_background;
};


#endif
