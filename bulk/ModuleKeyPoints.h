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
		Parameters(const ConfigReader& x_confReader) : Module::Parameters(x_confReader)
		{
			RefParameterByName("type").SetRange("[CV_8UC1]");
			Init();
		};
	};

	ModuleKeyPoints(const ConfigReader& x_configReader);
	~ModuleKeyPoints();
	// MKCLASS("ModuleKeyPoints")
	// MKDESCR("Extract different types of keyPoints from image")
	
	inline virtual const Parameters& GetParameters() const { return m_param;}
	virtual void ProcessFrame();
	void Reset();

private:
	inline virtual Parameters & RefParameters() {return m_param;}
	Parameters m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	// input
	cv::Mat m_input;
	std::vector <Object> m_objectsIn;

	// output
	std::vector <Object> m_objectsOut;

	// state variables
	cv::FeatureDetector* mp_detector;

#ifdef MARKUS_DEBUG_STREAMS
	cv::Mat m_debug;
#endif
};


#endif
