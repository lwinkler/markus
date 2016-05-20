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

#include "Module.h"
#include "Parameter.h"
#include "Event.h"
#include "StreamNum.h"

/**
* @brief Detect motion from an image where pixel value represents motion
*/
class MotionDetector : public Module
{
public:
	class Parameters : public Module::Parameters
	{

	public:
		Parameters(const ConfigReader& x_confReader) : Module::Parameters(x_confReader)
		{
			AddParameter(new ParameterFloat("motion_thres" , 0.1 , 0 , 1 , &motionThres , "Threshold for motion analysis"));
			AddParameter(new ParameterBool("propagate"     , 1   , 0 , 1 , &propagate   , "Threshold for motion analysis"));

			RefParameterByName("width").SetRange("[32:6400]");
			RefParameterByName("height").SetRange("[24:4800]");
			Init();
		}

		float motionThres;
		bool propagate;
	};

	MotionDetector(ParameterStructure& xr_params);
	virtual ~MotionDetector();
	MKCLASS("MotionDetector")
	MKCATEG("EventDetector")
	MKDESCR("Detect motion from an image where pixel value represents motion")

	virtual void Reset() override;
	virtual void ProcessFrame() override;
	virtual bool PropagateCondition() const override {return m_param.propagate || m_state;}

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	// input
	cv::Mat m_input;

	// output
	bool   m_state;
	Event  m_event;
	double m_value;

	// temp
	StreamNum<double>* mp_streamValues = nullptr;

	// debug
#ifdef MARKUS_DEBUG_STREAMS
	cv::Mat m_debug;
#endif

	static const cv::Scalar m_colorPlotBack;
	static const cv::Scalar m_colorPlotValue;
	static const cv::Scalar m_colorPlotThres;
};

#endif

