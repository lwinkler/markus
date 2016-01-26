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

#ifndef CLASSIFY_EVENTS
#define CLASSIFY_EVENTS

#include "Module.h"
#include "Event.h"
#include "StreamObject.h"

#include <list>

class ConfigReader;

/**
* @brief A virtual module to filter events based on user feedback
*/
class ModuleClassifyEvents : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) : Module::Parameters(x_confReader)
		{
			m_list.push_back(new ParameterDouble("validity_thres", 0.5, 0, 1, &validityThres, "Decision threshold to consider an event as valid [0 to 1]"));
			Init();
		}
		double validityThres;
	};

	ModuleClassifyEvents(ParameterStructure& xr_params);
	~ModuleClassifyEvents();

	virtual void ProcessFrame();
	void Reset();
	virtual void InvalidateLastEvent() = 0;
	virtual void ValidateLastEvent() = 0;
	virtual float PredictEventValidity(const Event& x_event) = 0;
	void PushEvent();
	void PopEvent();

protected:

	// input
	Event   m_eventIn;
	cv::Mat m_imageIn;

	// output
	Event   m_eventOut;
	Event   m_eventToValidate;
	cv::Mat m_imageToValidate;

	// state variables
	std::list<Event> m_events;
	std::list<cv::Mat>   m_images;

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;
};


#endif