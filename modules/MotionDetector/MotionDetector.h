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

#ifndef MOTION_DETECTOR_H
#define MOTION_DETECTOR_H

#include <cv.h>
#include "Module.h"
#include "Parameter.h"
// #include "Timer.h"


/*! \class MotionDetector
 *  \brief Class containing methods/attributes of a slit camera
 *
 */


class MotionDetectorParameterStructure : public ModuleParameterStructure
{
	
public:
	MotionDetectorParameterStructure(const ConfigReader& x_confReader) : 
		ModuleParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterFloat(0, "motion_thres", 0.1, PARAM_FLOAT, 0, 1, &motionThres,	"Threshold for motion analysis"));

		ParameterStructure::Init();
	}
	
	float motionThres;
};

class MotionDetector : public Module
{
protected:
	virtual void ProcessFrame();
	MotionDetectorParameterStructure m_param;
	static const char * m_type;

	cv::Mat * m_input;
	cv::Mat * m_debug;

	bool m_state;

	static const cv::Scalar m_colorPlotBack;
	static const cv::Scalar m_colorPlotValue;
	static const cv::Scalar m_colorPlotThres;

public:
	MotionDetector(const ConfigReader& x_configReader);
	~MotionDetector(void);
	void Reset();


protected:
	inline virtual MotionDetectorParameterStructure& RefParameter() { return m_param;}
};

#endif

