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

#ifndef RANDOM_EVENT_GENERATOR_H
#define RANDOM_EVENT_GENERATOR_H

#include "Input.h"
#include "Event.h"

/**
* @brief Generate an event with random features at each step
*/
class RandomEventGenerator : public Input
{
public:
	class Parameters : public Input::Parameters
	{
	public:
		Parameters(const std::string& x_name) : Input::Parameters(x_name)
		{
			AddParameter(new ParameterInt("nbFeatures", 	4, 	0, 	1000,	 &nbFeatures,	"Number of features per event"));
			AddParameter(new ParameterInt("randomSeed", 	0, 	0, 	INT_MAX, &randomSeed,	"Seed for random generator: 0 means seed is generated from timer"));
			AddParameter(new ParameterDouble("timeInterval", 	0, 	0, 	DBL_MAX, &timeInterval,	"Time interval between events [s]"));
			RefParameterByName("type").SetDefaultAndValue("CV_8UC3");
			RefParameterByName("fps").SetDefaultAndValue(5);
		}

	public:
		int nbFeatures;
		int randomSeed;
		double timeInterval;
	};

	RandomEventGenerator(ParameterStructure& xr_params);
	virtual ~RandomEventGenerator();
	MKCLASS("RandomEventGenerator")
	MKCATEG("Input")
	MKDESCR("Generate an event with random features at each step")

	void Capture() override;
	void Reset() override;

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	// output
	Event m_event;
	cv::Mat m_output;

	// state
	unsigned int m_seed;
	TIME_STAMP   m_timeNextEvent;
};

#endif
