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

#ifndef MODULE_KEYPOINTS_H
#define MODULE_KEYPOINTS_H

#include "Module.h"
#include "StreamObject.h"



/**
* @brief This class is a parent class for all module of keypoint extraction
* http://docs.opencv.org/modules/features2d/doc/common_interfaces_of_feature_detectors.html
*/
class ModuleKeyPoints : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		Parameters(const std::string& x_name) : Module::Parameters(x_name)
		{
			AddParameter(new ParameterString("descriptor", "", &descriptor, "The keypoint descriptor: ORB, BRIEF or Opponent. Leave empty for none.")); // TODO: It seems that only the original descriptor works

			RefParameterByName("type").SetRange("[CV_8UC1]");
			RefParameterByName("width").SetRange("[64:6400]");
			RefParameterByName("height").SetRange("[48:4800]");
		};
		std::string descriptor;
	};

	ModuleKeyPoints(ParameterStructure& xr_params);
	virtual ~ModuleKeyPoints();
	// MKCLASS("ModuleKeyPoints")
	// MKDESCR("Extract different types of keyPoints from image")

	virtual void ProcessFrame() override;
	virtual void Reset() override;

private:
	Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	// input
	cv::Mat m_input;
	std::vector <Object> m_objectsIn;

	// output
	std::vector <Object> m_objectsOut;

	// state variables
	cv::FeatureDetector     * mp_detector;
	cv::DescriptorExtractor * mp_descriptor;

#ifdef MARKUS_DEBUG_STREAMS
	cv::Mat m_debug;
#endif
};


#endif
