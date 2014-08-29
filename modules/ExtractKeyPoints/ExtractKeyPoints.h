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

#ifndef EXTRACTKEYPOINTS_H
#define EXTRACTKEYPOINTS_H

#include "Module.h"
#include "StreamObject.h"



/**
* @brief Extract different types of keypoints
*/
class ExtractKeyPoints : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) : Module::Parameters(x_confReader)
		{
			m_list.push_back(new ParameterString("keypoint_type", "FAST", &keyPointType,"Type of keypoint"));

			// RefParameterByName("type").SetDefault("CV_8UC3");
			// RefParameterByName("type").SetRange("[CV_8UC1,CV_8UC3]");

			Init();
		};
		std::string keyPointType;
	};

	ExtractKeyPoints(const ConfigReader& x_configReader);
	~ExtractKeyPoints();
	MKCLASS("ExtractKeyPoints")
	MKDESCR("Extract different types of keypoints from image")
	
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
	//std::vector <Object> m_objectsKeyPoints;


	// state variables
	cv::FeatureDetector *m_detector;

#ifdef MARKUS_DEBUG_STREAMS
	// cv::RNG m_rng;
	cv::Mat m_debug;
#endif
};


#endif
